/* ====================================================================
 * Copyright (c) 1997-2000 Carnegie Mellon University.  All rights 
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
 * 3. The names "Sphinx" and "Carnegie Mellon" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. To obtain permission, contact 
 *    sphinx@cs.cmu.edu.
 *
 * 4. Products derived from this software may not be called "Sphinx"
 *    nor may "Sphinx" appear in their names without prior written
 *    permission of Carnegie Mellon University. To obtain permission,
 *    contact sphinx@cs.cmu.edu.
 *
 * 5. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by Carnegie
 *    Mellon University (http://www.speech.cs.cmu.edu/)."
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
/*********************************************************************
 *
 * File: heap.h
 * 
 * Description: 
 * 
 * Author: 
 * 
 *********************************************************************/

#ifndef HEAP_H
#define HEAP_H

#include <s3/prim_type.h>

#ifndef NO_KEY
#define NO_KEY (uint32)(0xffffffff)
#endif

/* Functions for heaps of 32-bit float values */
typedef int (*hcmpfn32b_t)(float32 a, float32 b);

int
heap32b_max_comp(float32 a, float32 b);

int
heap32b_min_comp(float32 a, float32 b);

void
heapify32b(float32 *a, uint32 *k, uint32 i, uint32 sz, hcmpfn32b_t fn);

void
heap32b_a2h(float32 *a, uint32 *k, uint32 sz, hcmpfn32b_t fn);

uint32
heap32b_ins(float32 *a,  uint32 *k, uint32 sz,
	    float32 a_v, uint32 k_v, hcmpfn32b_t fn);

uint32
heap32b_extr_top(float32 *out_a, uint32 *out_k,
		 float32 *a, uint32 *k, uint32 sz, hcmpfn32b_t fn);

/* Functions for heaps of 64-bit float values */
typedef int (*hcmpfn64b_t)(float64 a, float64 b);

int
heap64b_max_comp(float64 a, float64 b);

int
heap64b_min_comp(float64 a, float64 b);

void
heapify64b(float64 *a, uint32 *k, uint32 i, uint32 sz, hcmpfn64b_t fn);

void
heap64b_a2h(float64 *a, uint32 *k, uint32 sz, hcmpfn64b_t fn);

uint32
heap64b_ins(float64 *a,  uint32 *k, uint32 sz,
	    float64 a_v, uint32 k_v, hcmpfn64b_t fn);

uint32
heap64b_extr_top(float64 *out_a, uint32 *out_k,
		 float64 *a, uint32 *k, uint32 sz, hcmpfn64b_t fn);

#endif /* HEAP_H */ 


/*
 * Log record.  Maintained by RCS.
 *
 * $Log$
 * Revision 1.2  2000/09/29  22:35:12  awb
 * *** empty log message ***
 * 
 * Revision 1.1  2000/09/24 21:38:30  awb
 * *** empty log message ***
 *
 * Revision 1.1  97/07/16  11:39:10  eht
 * Initial revision
 * 
 *
 */
