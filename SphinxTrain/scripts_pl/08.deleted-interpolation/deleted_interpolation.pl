#!/usr/bin/perl
## ====================================================================
##
## Copyright (c) 1996-2000 Carnegie Mellon University.  All rights 
## reserved.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
##
## 1. Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer. 
##
## 2. Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in
##    the documentation and/or other materials provided with the
##    distribution.
##
## 3. The names "Sphinx" and "Carnegie Mellon" must not be used to
##    endorse or promote products derived from this software without
##    prior written permission. To obtain permission, contact 
##    sphinx@cs.cmu.edu.
##
## 4. Products derived from this software may not be called "Sphinx"
##    nor may "Sphinx" appear in their names without prior written
##    permission of Carnegie Mellon University. To obtain permission,
##    contact sphinx@cs.cmu.edu.
##
## 5. Redistributions of any form whatsoever must retain the following
##    acknowledgment:
##    "This product includes software developed by Carnegie
##    Mellon University (http://www.speech.cs.cmu.edu/)."
##
## THIS SOFTWARE IS PROVIDED BY CARNEGIE MELLON UNIVERSITY ``AS IS'' AND 
## ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
## THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
## PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY
## NOR ITS EMPLOYEES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
## SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
## LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
## DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
## THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
## OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
## ====================================================================
##
## Author: Ricky Houghton
##

my $index = 0;
if (lc($ARGV[0]) eq '-cfg') {
    $cfg_file = $ARGV[1];
    $index = 2;
} else {
    $cfg_file = "etc/sphinx_train.cfg";
}

if (! -s "$cfg_file") {
    print ("unable to find default configuration file, use -cfg file.cfg or create etc/sphinx_train.cfg for default\n");
    exit -3;
}
require $cfg_file;

# this script runs deleted interpolation on a bunch of semi-cont
# hmm buffers. You need 2 or more buffers to run this!!

my $nsenones = "$CFG_N_TIED_STATES";

my $INTERP = "$CFG_BIN_DIR/delint";
my $cilambda = 0.9;

# up to 99 buffers
my $cd_hmmdir = "$CFG_BASE_DIR/model_parameters/$CFG_EXPTNAME.cd_semi_"."$CFG_N_TIED_STATES";
$bwaccumdir 	     = "";
for (<${CFG_BASE_DIR}/bwaccumdir/${CFG_EXPTNAME}_buff_*>) {
    $bwaccumdir .= " $_";
}

my $hmm_dir = "$CFG_BASE_DIR/model_parameters/$CFG_EXPTNAME.cd_semi_"."$CFG_N_TIED_STATES"."_delinterp";
mkdir ($hmm_dir,0777) unless -d $hmm_dir;

system("cp $cd_hmmdir/means $hmm_dir/means");
system("cp $cd_hmmdir/variances $hmm_dir/variances");
system("cp $cd_hmmdir/transition_matrices $hmm_dir/transition_matrices");
my $mixwfn = "$hmm_dir/mixture_weights";

my $moddeffn = "$CFG_BASE_DIR/model_architecture/$CFG_EXPTNAME.$CFG_N_TIED_STATES.mdef";

my $logdir = "$CFG_BASE_DIR/logdir/08.deleted_interpolation";
mkdir ($logdir,0777) unless -d $logdir;
my $logfile = "$logdir/$CFG_EXPTNAME.deletedintrep-${nsenones}.log";

$| = 1; # Turn on autoflushing
&ST_Log ("MODULE: 08 deleted interpolation\n");
&ST_Log ("    Cleaning up directories: logs...\n");
system ("rm  -rf $logdir/*");

open LOG,"> $logfile";

if (open PIPE,"$INTERP -accumdirs $bwaccumdir -moddeffn $moddeffn -mixwfn $mixwfn -cilambda $cilambda -feat $CFG_FEATURE -ceplen $CFG_VECTOR_LENGTH -maxiter 4000 2>&1 2>&1 |") {
    while ($line = <PIPE>) {
       print LOG $line;
    }
    close PIPE;
    close LOG;
}

exit 0;
