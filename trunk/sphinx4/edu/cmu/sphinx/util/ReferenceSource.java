/*
 * Copyright 1999-2002 Carnegie Mellon University.  
 * Portions Copyright 2002 Sun Microsystems, Inc.  
 * Portions Copyright 2002 Mitsubishi Electronic Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 * 
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL 
 * WARRANTIES.
 *[B
 */
package edu.cmu.sphinx.util;

import java.util.List;

/**
 * A source of reference texts.
 */
public interface ReferenceSource {

    /**
     * Returns a list of reference text.
     */
    public List getReferences();
}
