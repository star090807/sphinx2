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
 * @file sphinx_am_fst.cc
 *
 * Build finite-state transducers from Sphinx acoustic model definition files.
 */

#include <fst/fstlib.h>

namespace sphinxbase {
#include <cmd_ln.h>
#include <hash_table.h>
#include <err.h>
#include <ckd_alloc.h>
#include "mdef.h"
};

#include "fstprinter.h"

using namespace fst;
using namespace sphinxbase;
typedef VectorFst<LogArc> LogVectorFst;
typedef StdArc::StateId StateId;
typedef StdArc::Label Label;

static const arg_t args[] = {
    { "-mdef",
      ARG_STRING,
      NULL,
      "Model definition input file" },
    { "-binfst",
      ARG_STRING,
      NULL,
      "Binary FST output file" },
    { "-txtfst",
      ARG_STRING,
      NULL,
      "Text FST output file" },
    { NULL, 0, NULL, NULL }
};


int
main(int argc, char *argv[])
{
    return 0;
}