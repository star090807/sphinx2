/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ====================================================================
 * Copyright (c) 2008 Carnegie Mellon University.  All rights 
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
 * @file sbthread.c
 * @brief Simple portable thread functions
 * @author David Huggins-Daines <dhuggins@cs.cmu.edu>
 */

#include "sbthread.h"
#include "ckd_alloc.h"
#include "err.h"

/*
 * Platform-specific parts: threads, mutexes, and signals.
 */
#ifdef _WIN32
#include <windows.h>
#else /* POSIX */
#include <pthread.h>
#include <sys/time.h>
struct sbthread_s {
    cmd_ln_t *config;
    sbmsgq_t *msgq;
    sbthread_main func;
    void *arg;
    pthread_t th;
};

struct sbmsg_s {
    size_t len;
    void *data;
};

struct sbmsgq_s {
    sbmsg_t *msgs;
    size_t depth;
    size_t out, nmsg;
    pthread_mutex_t mtx;
    pthread_cond_t cond;
};

struct sbevent_s {
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    int signalled;
};

struct sbmtx_s {
    pthread_mutex_t mtx;
};

static void *
sbthread_internal_main(void *arg)
{
    sbthread_t *th = (sbthread_t *)arg;
    int rv;

    rv = (*th->func)(th);
    return (void *)(long)rv;
}

cmd_ln_t *
sbthread_config(sbthread_t *th)
{
    return th->config;
}

void *
sbthread_arg(sbthread_t *th)
{
    return th->arg;
}

sbmsgq_t *
sbthread_msgq(sbthread_t *th)
{
    return th->msgq;
}

sbthread_t *
sbthread_start(cmd_ln_t *config, sbthread_main func, void *arg)
{
    sbthread_t *th;
    int rv;

    th = ckd_calloc(1, sizeof(*th));
    th->config = config;
    th->func = func;
    th->arg = arg;
    th->msgq = sbmsgq_init(256);
    if ((rv = pthread_create(&th->th, NULL, &sbthread_internal_main, th)) != 0) {
        E_ERROR("Failed to create thread: %d\n", rv);
        sbthread_free(th);
        return NULL;
    }
    return th;
}

sbmsg_t *
sbthread_send(sbthread_t *th, size_t len, void *data)
{
    return sbmsgq_send(th->msgq, len, data);
}

int
sbthread_wait(sbthread_t *th)
{
    void *exit;
    int rv;

    /* It has already been joined. */
    if (th->th == (pthread_t)-1)
        return -1;

    rv = pthread_join(th->th, &exit);
    if (rv != 0) {
        E_ERROR("Failed to join thread: %d\n", rv);
        return -1;
    }
    th->th = (pthread_t)-1;
    return (int)(long)exit;
}

void
sbthread_free(sbthread_t *th)
{
    sbthread_wait(th);
    sbmsgq_free(th->msgq);
    ckd_free(th);
}

sbmsgq_t *
sbmsgq_init(size_t depth)
{
    sbmsgq_t *msgq;

    msgq = ckd_calloc(1, sizeof(*msgq));
    msgq->msgs = ckd_calloc(depth, sizeof(*msgq->msgs));
    msgq->depth = depth;
    if (pthread_cond_init(&msgq->cond, NULL) != 0) {
        ckd_free(msgq);
        ckd_free(msgq->msgs);
        return NULL;
    }
    if (pthread_mutex_init(&msgq->mtx, NULL) != 0) {
        pthread_cond_destroy(&msgq->cond);
        ckd_free(msgq);
        ckd_free(msgq->msgs);
        return NULL;
    }
    return msgq;
}

void
sbmsgq_free(sbmsgq_t *msgq)
{
    pthread_mutex_destroy(&msgq->mtx);
    pthread_cond_destroy(&msgq->cond);
    ckd_free(msgq->msgs);
    ckd_free(msgq);
}

sbmsg_t *
sbmsgq_send(sbmsgq_t *q, size_t len, void *data)
{
    size_t in;

    /* Lock the condition variable while we manipulate nmsg. */
    pthread_mutex_lock(&q->mtx);
    if (q->nmsg == q->depth) {
        pthread_mutex_unlock(&q->mtx);
        return NULL;
    }
    in = (q->out + q->nmsg) % q->depth;
    q->msgs[in].len = len;
    q->msgs[in].data = data;
    ++q->nmsg;
    /* Signal the condition variable. */
    pthread_cond_signal(&q->cond);
    /* Unlock it, we have nothing else to do. */
    pthread_mutex_unlock(&q->mtx);
    return q->msgs + in;
}

static int
cond_timed_wait(pthread_cond_t *cond, pthread_mutex_t *mtx, int sec, int nsec)
{
    int rv;
    if (sec == -1) {
        rv = pthread_cond_wait(cond, mtx);
    }
    else {
        struct timeval now;
        struct timespec end;

        gettimeofday(&now, NULL);
        end.tv_sec = now.tv_sec + sec;
        end.tv_nsec = now.tv_usec * 1000 + nsec;
        if (end.tv_nsec > (1000*1000*1000)) {
            sec += end.tv_nsec / (1000*1000*1000);
            end.tv_nsec = end.tv_nsec % (1000*1000*1000);
        }
        rv = pthread_cond_timedwait(cond, mtx, &end);
    }
    return rv;
}

sbmsg_t *
sbmsgq_wait(sbmsgq_t *q, int sec, int nsec)
{
    size_t out;

    /* Lock the condition variable while we manipulate nmsg. */
    pthread_mutex_lock(&q->mtx);
    if (q->nmsg == 0) {
        /* Unlock the condition variable and wait for a signal. */
        cond_timed_wait(&q->cond, &q->mtx, sec, nsec);
        /* Condition variable is now locked again. */
    }
    out = q->out;
    ++q->out;
    --q->nmsg;
    if (q->out == q->depth)
        q->out = 0;
    /* Unlock the condition variable, we are done. */
    pthread_mutex_unlock(&q->mtx);
    return q->msgs + out;
}

sbevent_t *
sbevent_init(void)
{
    sbevent_t *evt;
    int rv;

    evt = ckd_calloc(1, sizeof(*evt));
    if ((rv = pthread_mutex_init(&evt->mtx, NULL)) != 0) {
        E_ERROR("Failed to initialize mutex: %d\n", rv);
        ckd_free(evt);
        return NULL;
    }
    if ((rv = pthread_cond_init(&evt->cond, NULL)) != 0) {
        E_ERROR_SYSTEM("Failed to initialize mutex: %d\n", rv);
        pthread_mutex_destroy(&evt->mtx);
        ckd_free(evt);
        return NULL;
    }
    return evt;
}

void
sbevent_free(sbevent_t *evt)
{
    pthread_mutex_destroy(&evt->mtx);
    pthread_cond_destroy(&evt->cond);
    ckd_free(evt);
}

int
sbevent_signal(sbevent_t *evt)
{
    int rv;

    pthread_mutex_lock(&evt->mtx);
    evt->signalled = TRUE;
    rv = pthread_cond_signal(&evt->cond);
    pthread_mutex_unlock(&evt->mtx);
    return rv;
}

int
sbevent_wait(sbevent_t *evt, int sec, int nsec)
{
    int rv = 0;

    /* Lock the mutex before we check its signalled state. */
    pthread_mutex_lock(&evt->mtx);
    /* If it's not signalled, then wait until it is. */
    if (!evt->signalled) {
        if (sec == -1) {
            rv = pthread_cond_wait(&evt->cond, &evt->mtx);
        }
        else {
            struct timeval now;
            struct timespec end;

            gettimeofday(&now, NULL);
            end.tv_sec = now.tv_sec + sec;
            end.tv_nsec = now.tv_usec * 1000 + nsec;
            if (end.tv_nsec > (1000*1000*1000)) {
                sec += end.tv_nsec / (1000*1000*1000);
                end.tv_nsec = end.tv_nsec % (1000*1000*1000);
            }
            rv = pthread_cond_timedwait(&evt->cond, &evt->mtx, &end);
        }
    }
    /* Set its state to unsignalled if we were successful. */
    if (rv == 0)
        evt->signalled = FALSE;
    /* And unlock its mutex. */
    pthread_mutex_unlock(&evt->mtx);

    return rv;
}

sbmtx_t *
sbmtx_init(void)
{
    sbmtx_t *mtx;

    mtx = ckd_calloc(1, sizeof(*mtx));
    if (pthread_mutex_init(&mtx->mtx, NULL) != 0) {
        ckd_free(mtx);
        return NULL;
    }
    return mtx;
}

int
sbmtx_trylock(sbmtx_t *mtx)
{
    return pthread_mutex_trylock(&mtx->mtx);
}

int
sbmtx_lock(sbmtx_t *mtx)
{
    return pthread_mutex_lock(&mtx->mtx);
}

int
sbmtx_unlock(sbmtx_t *mtx)
{
    return pthread_mutex_unlock(&mtx->mtx);
}

void
sbmtx_free(sbmtx_t *mtx)
{
    pthread_mutex_destroy(&mtx->mtx);
    ckd_free(mtx);
}

#endif /* not WIN32 */

void *
sbmsg_unpack(sbmsg_t *msg, size_t *out_len)
{
    if (out_len) *out_len = msg->len;
    return msg->data;
}
