/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.8
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package edu.cmu.pocketsphinx;

public class NGramModel {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected NGramModel(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(NGramModel obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        sphinxbaseJNI.delete_NGramModel(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public NGramModel(String path) {
    this(sphinxbaseJNI.new_NGramModel(path), true);
  }

  public void write(String path, SWIGTYPE_p_ngram_file_type_t ftype) {
    sphinxbaseJNI.NGramModel_write(swigCPtr, this, path, SWIGTYPE_p_ngram_file_type_t.getCPtr(ftype));
  }

  public SWIGTYPE_p_ngram_file_type_t strToType(String str) {
    return new SWIGTYPE_p_ngram_file_type_t(sphinxbaseJNI.NGramModel_strToType(swigCPtr, this, str), true);
  }

  public String typeToStr(int type) {
    return sphinxbaseJNI.NGramModel_typeToStr(swigCPtr, this, type);
  }

  public void recode(String src, String dst) {
    sphinxbaseJNI.NGramModel_recode(swigCPtr, this, src, dst);
  }

  public void casefold(int kase) {
    sphinxbaseJNI.NGramModel_casefold(swigCPtr, this, kase);
  }

  public SWIGTYPE_p_int32 size() {
    return new SWIGTYPE_p_int32(sphinxbaseJNI.NGramModel_size(swigCPtr, this), true);
  }

  public SWIGTYPE_p_int32 addWord(String word, SWIGTYPE_p_float32 weight) {
    return new SWIGTYPE_p_int32(sphinxbaseJNI.NGramModel_addWord(swigCPtr, this, word, SWIGTYPE_p_float32.getCPtr(weight)), true);
  }

  public SWIGTYPE_p_int32 addClass(String c, SWIGTYPE_p_float32 w, SWIGTYPE_p_p_char words, SWIGTYPE_p_float32 weights, SWIGTYPE_p_int32 nwords) {
    return new SWIGTYPE_p_int32(sphinxbaseJNI.NGramModel_addClass(swigCPtr, this, c, SWIGTYPE_p_float32.getCPtr(w), SWIGTYPE_p_p_char.getCPtr(words), SWIGTYPE_p_float32.getCPtr(weights), SWIGTYPE_p_int32.getCPtr(nwords)), true);
  }

}
