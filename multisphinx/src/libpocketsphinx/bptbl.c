/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ====================================================================
 * Copyright (c) 2008-2010 Carnegie Mellon University.  All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * This work was supported in part by funding from the Defense Advanced 
 * Research Projects Agency and the National Science Foundation of the 
 * United States of America, and the CMU Sphinx Speech Consortium.
 *
 * THIS SOFTWARE IS PROVIDED BY CARNEGIE MELLON UNIVERSITY ``AS IS'' AND 
 * ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY
 * NOR ITS EMPLOYEES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ====================================================================
 *
 */

/**
 * @file bptbl.c Forward search lattice for N-Gram search.
 */

/* System headers. */
#include <string.h>
#include <assert.h>

/* SphinxBase headers. */
#include <sphinxbase/ckd_alloc.h>
#include <sphinxbase/listelem_alloc.h>
#include <sphinxbase/err.h>

/* Local headers. */
#include "ngram_search.h"

bptbl_t *
bptbl_init(dict2pid_t *d2p, int n_alloc, int n_frame_alloc)
{
    bptbl_t *bptbl = ckd_calloc(1, sizeof(*bptbl));

    bptbl->d2p = dict2pid_retain(d2p);
    bptbl->n_alloc = n_alloc;
    bptbl->n_frame_alloc = n_frame_alloc;

    bptbl->ent = ckd_calloc(bptbl->n_alloc, sizeof(*bptbl->ent));
    bptbl->permute = ckd_calloc(bptbl->n_alloc, sizeof(*bptbl->permute));
    bptbl->word_idx = ckd_calloc(dict_size(d2p->dict),
                                 sizeof(*bptbl->word_idx));
    bptbl->bscore_stack_size = bptbl->n_alloc * 20;
    bptbl->bscore_stack = ckd_calloc(bptbl->bscore_stack_size,
                                     sizeof(*bptbl->bscore_stack));
    bptbl->ef_idx = ckd_calloc(bptbl->n_frame_alloc + 1,
                               sizeof(*bptbl->ef_idx));
    ++bptbl->ef_idx; /* Make ef_idx[-1] valid */
    bptbl->sf_idx = ckd_calloc(bptbl->n_frame_alloc,
                               sizeof(*bptbl->sf_idx));
    bptbl->valid_fr = bitvec_alloc(bptbl->n_frame_alloc);
    bptbl->frm_wordlist = ckd_calloc(bptbl->n_frame_alloc,
                                     sizeof(*bptbl->frm_wordlist));

    return bptbl;
}

void
bptbl_free(bptbl_t *bptbl)
{
    if (bptbl == NULL)
        return;
    dict2pid_free(bptbl->d2p);
    ckd_free(bptbl->word_idx);
    ckd_free(bptbl->ent);
    ckd_free(bptbl->permute);
    ckd_free(bptbl->bscore_stack);
    if (bptbl->ef_idx != NULL)
        ckd_free(bptbl->ef_idx - 1);
    ckd_free(bptbl->sf_idx);
    ckd_free(bptbl->frm_wordlist);
    bitvec_free(bptbl->valid_fr);
    ckd_free(bptbl);
}

void
bptbl_reset(bptbl_t *bptbl)
{
    int i;

    for (i = 0; i < bptbl->n_frame_alloc; ++i) {
        bptbl->sf_idx[i] = -1;
        bptbl->ef_idx[i] = -1;
    }
    bitvec_clear_all(bptbl->valid_fr, bptbl->n_frame_alloc);
    bptbl->first_invert_bp = 0;
    bptbl->n_frame = 0;
    bptbl->n_ent = 0;
    bptbl->bss_head = 0;
    bptbl->active_sf = 0;
}

void
dump_bptable(bptbl_t *bptbl, int start, int end)
{
    int i, j;

    if (end == -1)
        end = bptbl->n_ent;
    E_INFO("Backpointer table (%d entries from %d to %d):\n",
           bptbl->n_ent, start, end);
    for (j = i = start; i < end; ++i) {
        if (bptbl->ent[i].valid == FALSE)
            continue;
        ++j;
        E_INFO_NOFN("%-5d %-10s start %-3d end %-3d score %-8d bp %-3d\n",
                    i, dict_wordstr(bptbl->d2p->dict, bptbl->ent[i].wid),
                    bp_sf(bptbl, i),
                    bptbl->ent[i].frame,
                    bptbl->ent[i].score,
                    bptbl->ent[i].bp);
    }
    E_INFO("%d valid entries\n", j);
    E_INFO("End frame index from %d to %d:\n",
           bptbl->ent[start].frame, bptbl->ent[end-1].frame);
    for (i = bptbl->ent[start].frame; i <= bptbl->ent[end-1].frame; ++i) {
        E_INFO_NOFN("%d: %d\n", i, bptbl->ef_idx[i]);
        /* FIXME: this must go away once forward sorting is in place */
        assert(bptbl->ef_idx[i] >= bptbl->ef_idx[i-1]);
    }
}

/**
 * Mark coaccessible entries in the backpointer table.
 *
 * @param bptbl Backpointer table
 * @param sf First frame (as per ef_idx) to mark entries in
 * @param ef Frame after last frame (as per ef_idx) to mark entries in
 * @param cf Current frame of search
 */
static void
bptbl_mark(bptbl_t *bptbl, int sf, int ef, int cf)
{
    int i, j, n_active_fr, last_gc_fr;

    /* Invalidate all backpointer entries up to active_sf - 1. */
    /* FIXME: actually anything behind active_sf - 1 is fair game. */
    E_INFO("Garbage collecting from %d to %d (%d to %d):\n",
           bptbl->ef_idx[sf], bptbl->ef_idx[ef], sf, ef);
    for (i = bptbl->ef_idx[sf];
         i < bptbl->ef_idx[ef]; ++i) {
        bptbl->ent[i].valid = FALSE;
    }

    /* Now re-activate all ones backwards reachable from the elastic
     * window. (make sure cf has been pushed!) */
    E_INFO("Finding coaccessible frames from backpointers from %d to %d\n",
           bptbl->ef_idx[ef], bptbl->ef_idx[cf]);
    /* Collect coaccessible frames from these backpointers */
    bitvec_clear_all(bptbl->valid_fr, cf);
    n_active_fr = 0;
    for (i = bptbl->ef_idx[ef];
         i < bptbl->ef_idx[cf]; ++i) {
        int32 bp = bptbl->ent[i].bp;
        if (bp != NO_BP) {
            int frame = bptbl->ent[bp].frame;
            if (bitvec_is_clear(bptbl->valid_fr, frame)) {
                bitvec_set(bptbl->valid_fr, frame);
                ++n_active_fr;
            }
        }
    }
    /* Track the last frame with outgoing backpointers for gc */
    last_gc_fr = ef - 1;
    while (n_active_fr > 0) {
        int next_gc_fr = 0;
        n_active_fr = 0;
        for (i = sf; i <= last_gc_fr; ++i) {
            if (bitvec_is_set(bptbl->valid_fr, i)) {
                bitvec_clear(bptbl->valid_fr, i);
                /* Add all backpointers in this frame (the bogus
                 * lattice generation algorithm) */
                for (j = bptbl->ef_idx[i];
                     j < bptbl->ef_idx[i + 1]; ++j) {
                    int32 bp = bptbl->ent[j].bp;
                    bptbl->ent[j].valid = TRUE;
                    if (bp != NO_BP) {
                        int frame = bptbl->ent[bp].frame;
                        if (bitvec_is_clear(bptbl->valid_fr, frame)) {
                            bitvec_set(bptbl->valid_fr, bptbl->ent[bp].frame);
                            ++n_active_fr;
                        }
                        if (frame > next_gc_fr)
                            next_gc_fr = frame;
                    }
                }
            }
        }
        E_INFO("last_gc_fr %d => %d\n", last_gc_fr, next_gc_fr);
        last_gc_fr = next_gc_fr;
    }
    E_INFO("Removed");
    for (i = bptbl->ef_idx[sf];
         i < bptbl->ef_idx[ef]; ++i) {
        if (bptbl->ent[i].valid == FALSE)
            E_INFOCONT(" %d", i);
    }
    E_INFOCONT("\n");
}

/**
 * Compact the backpointer table.
 *
 * @param bptbl Backpointer table.
 * @param eidx First index which cannot be compacted.
 */
static void
bptbl_compact(bptbl_t *bptbl, int eidx)
{
    int src, dest, ef;
    int sidx = bptbl->first_invert_bp;

    ef = bptbl->ent[sidx].frame;
    E_INFO("compacting from %d to %d (%d to %d)\n",
           sidx, eidx, ef, bptbl->ent[eidx].frame);
    for (dest = src = sidx; src < eidx; ++src) {
        /* Update all ef_idx including missing frames (there are many) */
        while (ef <= bptbl->ent[src].frame)
            bptbl->ef_idx[ef++] = dest;
        if (bptbl->ent[src].valid) {
            if (dest != src) {
                /* Track the first compacted entry. */
                if (bptbl->first_invert_bp == sidx)
                    bptbl->first_invert_bp = dest;
                bptbl->ent[dest] = bptbl->ent[src];
                bptbl->ent[src].valid = FALSE;
            }
            E_INFO("permute %d => %d\n", src, dest);
            bptbl->permute[src] = dest;
            ++dest;
        }
        else {
            E_INFO("permute %d => -1\n", src);
            bptbl->permute[src] = -1;
        }
    }
    if (eidx == bptbl->n_ent)
        bptbl->n_ent = dest;
}

/**
 * Sort retired backpointer entries by start frame.
 *
 * @param bptbl Backpointer table.
 * @param sidx Index of first unordered entry.
 * @param eidx Index of last unordered entry.
 */
static int
bptbl_forward_sort(bptbl_t *bptbl, int sidx, int eidx)
{
    return 0;
}

/**
 * Remap backpointers in backpointer table.
 *
 * @param bptbl Backpointer table.
 * @param eidx Index of first backpointer which cannot be remapped.
 */
static void
bptbl_invert(bptbl_t *bptbl, int eidx)
{
    int sidx = bptbl->first_invert_bp;
    int i;

    for (i = sidx; i < bptbl->n_ent; ++i) {
        if (bptbl->ent[i].bp >= sidx && bptbl->ent[i].bp < eidx) {
            if (bptbl->ent[i].bp != bptbl->permute[bptbl->ent[i].bp])
                E_INFO("invert %d => %d in %d\n",
                       bptbl->ent[i].bp, bptbl->permute[bptbl->ent[i].bp], i);
            bptbl->ent[i].bp = bptbl->permute[bptbl->ent[i].bp];
            assert(bp_sf(bptbl, i) < bptbl->ent[i].frame);
        }
    }
}

static void
bptbl_gc(bptbl_t *bptbl, int oldest_bp, int frame_idx)
{
    int prev_active_sf, active_sf;

    /* active_sf is the first frame which is still active in search
     * (i.e. for which outgoing word arcs can still be generated).
     * Therefore, any future backpointer table entries will not point
     * backwards to any backpointers before active_sf, and thus any
     * backpointers which are not reachable from those exiting in
     * active_sf will never be reachable. */
    prev_active_sf = bptbl->active_sf;
    active_sf = bptbl->ent[oldest_bp].frame;
    assert(active_sf >= prev_active_sf);
    if (active_sf <= prev_active_sf + 1)
        return;
    /* If there is nothing to GC then finish up */
    if (bptbl->ef_idx[prev_active_sf] == bptbl->ef_idx[active_sf]) {
        bptbl->active_sf = active_sf;
        return;
    }

    bptbl_mark(bptbl, prev_active_sf, active_sf, frame_idx);
    E_INFO("before compaction\n");
    dump_bptable(bptbl, 0, -1);
    bptbl_compact(bptbl, bptbl->ef_idx[active_sf]);
    E_INFO("after compaction\n");
    dump_bptable(bptbl, 0, -1);
    bptbl_forward_sort(bptbl, bptbl->ef_idx[prev_active_sf],
                       bptbl->ef_idx[active_sf]);
    bptbl_invert(bptbl, bptbl->ef_idx[active_sf]);
    E_INFO("after inversion\n");
    dump_bptable(bptbl, 0, -1);
    bptbl->active_sf = active_sf;
}

int
bptbl_push_frame(bptbl_t *bptbl, int oldest_bp, int frame_idx)
{
    E_INFO("pushing frame %d, oldest bp %d in frame %d\n",
           frame_idx, oldest_bp, oldest_bp == NO_BP ? -1 : bptbl->ent[oldest_bp].frame);
    bptbl->ef_idx[frame_idx] = bptbl->n_ent;
    bptbl_gc(bptbl, oldest_bp, frame_idx);
    if (frame_idx >= bptbl->n_frame_alloc) {
        bptbl->n_frame_alloc *= 2;
        bptbl->ef_idx = ckd_realloc(bptbl->ef_idx - 1,
                                    (bptbl->n_frame_alloc + 1)
                                    * sizeof(*bptbl->ef_idx));
        bptbl->sf_idx = ckd_realloc(bptbl->sf_idx,
                                    bptbl->n_frame_alloc
                                    * sizeof(*bptbl->sf_idx));
        bptbl->valid_fr = bitvec_realloc(bptbl->valid_fr, bptbl->n_frame_alloc);
        bptbl->frm_wordlist = ckd_realloc(bptbl->frm_wordlist,
                                          bptbl->n_frame_alloc
                                          * sizeof(*bptbl->frm_wordlist));
        ++bptbl->ef_idx; /* Make bptableidx[-1] valid */
    }
    return bptbl->n_ent;
}

bp_t *
bptbl_enter(bptbl_t *bptbl, int32 w, int frame_idx, int32 path,
            int32 score, int rc)
{
    int32 i, rcsize, *bss;
    bp_t *be;

    /* This might happen if recognition fails. */
    if (bptbl->n_ent == NO_BP) {
        E_ERROR("No entries in backpointer table!");
        return NULL;
    }

    /* Expand the backpointer tables if necessary. */
    if (bptbl->n_ent >= bptbl->n_alloc) {
        bptbl->n_alloc *= 2;
        bptbl->ent = ckd_realloc(bptbl->ent,
                                 bptbl->n_alloc
                                 * sizeof(*bptbl->ent));
        bptbl->permute = ckd_realloc(bptbl->permute,
                                     bptbl->n_alloc
                                     * sizeof(*bptbl->permute));
        E_INFO("Resized backpointer table to %d entries\n", bptbl->n_alloc);
    }
    if (bptbl->bss_head >= bptbl->bscore_stack_size
        - bin_mdef_n_ciphone(bptbl->d2p->mdef)) {
        bptbl->bscore_stack_size *= 2;
        bptbl->bscore_stack = ckd_realloc(bptbl->bscore_stack,
                                          bptbl->bscore_stack_size
                                          * sizeof(*bptbl->bscore_stack));
        E_INFO("Resized score stack to %d entries\n", bptbl->bscore_stack_size);
    }

    bptbl->word_idx[w] = bptbl->n_ent;
    be = &(bptbl->ent[bptbl->n_ent]);
    be->wid = w;
    be->frame = frame_idx;
    be->bp = path;
    be->score = score;
    be->s_idx = bptbl->bss_head;
    be->valid = TRUE;
    bptbl_fake_lmstate(bptbl, bptbl->n_ent);

    /* DICT2PID */
    /* Get diphone ID for final phone and number of ssids corresponding to it. */
    be->last_phone = dict_last_phone(bptbl->d2p->dict,w);
    if (dict_is_single_phone(bptbl->d2p->dict, w)) {
        be->last2_phone = -1;
        rcsize = 1;
    }
    else {
        be->last2_phone = dict_second_last_phone(bptbl->d2p->dict, w);
        rcsize = dict2pid_rssid(bptbl->d2p, be->last_phone, be->last2_phone)->n_ssid;
    }
    /* Allocate some space on the bptbl->bscore_stack for all of these triphones. */
    for (i = rcsize, bss = bptbl->bscore_stack + bptbl->bss_head; i > 0; --i, bss++)
        *bss = WORST_SCORE;
    bptbl->bscore_stack[bptbl->bss_head + rc] = score;
    E_INFO("Entered bp %d sf %d ef %d active_sf %d\n", bptbl->n_ent,
           bp_sf(bptbl, bptbl->n_ent), frame_idx, bptbl->active_sf);
    assert(bp_sf(bptbl, bptbl->n_ent) >= bptbl->active_sf);

    bptbl->n_ent++;
    bptbl->bss_head += rcsize;

    return be;
}

void
bptbl_fake_lmstate(bptbl_t *bptbl, int32 bp)
{
    int32 w, prev_bp;
    bp_t *be;

    assert(bp != NO_BP);

    be = &(bptbl->ent[bp]);
    prev_bp = bp;
    w = be->wid;

    while (dict_filler_word(bptbl->d2p->dict, w)) {
        prev_bp = bptbl->ent[prev_bp].bp;
        if (prev_bp == NO_BP)
            return;
        w = bptbl->ent[prev_bp].wid;
    }

    be->real_wid = dict_basewid(bptbl->d2p->dict, w);

    prev_bp = bptbl->ent[prev_bp].bp;
    be->prev_real_wid =
        (prev_bp != NO_BP) ? bptbl->ent[prev_bp].real_wid : -1;
}
