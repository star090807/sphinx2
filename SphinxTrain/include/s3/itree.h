/* ====================================================================
 * Copyright (c) 1994-2000 Carnegie Mellon University.  All rights 
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
 * File: itree.h
 * 
 * Description: 
 * 
 * Author: 
 *	Eric H. Thayer (eht@cs.cmu.edu)
 * 
 *********************************************************************/

#ifndef ITREE_H
#define ITREE_H

#include <s3/prim_type.h>

typedef uint32 cell_id_t;
typedef uint32 cell_index_t;

#define NULL_INDEX	(0xffffffff)
#define NO_ID		(0xffffffff)

typedef struct {
    cell_id_t id;		/* id of node */
    cell_index_t child;		/* index of first child context */
    cell_index_t sib;		/* index of next sibling in current context */
} cell_t;

typedef struct {
    cell_t *cell;
    uint32 n_cell;
    uint32 max_n_cell;
} itree_t;

itree_t *
itree_new(uint32 n_cell_hint);

cell_index_t itree_find(itree_t *t,
			cell_index_t *end,
			cell_index_t start,
			cell_id_t id);

cell_index_t
itree_add_sib(itree_t *t,
	      cell_index_t end,
	      cell_id_t id);

cell_index_t
itree_add_child(itree_t *t,
		cell_index_t parent,
		cell_id_t id);

cell_index_t
itree_add_tri(itree_t *t,
	      cell_id_t left_context,
	      cell_id_t right_context,
	      cell_id_t posn,
	      cell_id_t tri_id);

cell_index_t
itree_find_tri(itree_t *t,
	       cell_id_t left_context,
	       cell_id_t right_context,
	       cell_id_t posn);

cell_index_t
itree_child(itree_t *t,
	    cell_index_t parent);

cell_id_t
itree_enum_init(itree_t *t);

cell_id_t
itree_enum(void);

#endif /* ITREE_H */ 


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
 * Revision 1.3  1996/03/04  15:55:43  eht
 * Added ability to walk the index trees
 *
 * Revision 1.2  1995/10/09  20:55:35  eht
 * Changes needed for prim_type.h
 *
 *
 */
