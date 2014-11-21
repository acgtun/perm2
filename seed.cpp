#include "seed.h"

SIZE_T GetHashValue(const InBits & r) {
  return (SIZE_T)((r.ub << HASHSEEDLEN) + r.lb);
}

SIZE_T GetHashValue(const char * strVal, const int & len) {
  if (len != HASHSEEDLEN)
    return 0;
  InBits r;
  EncodeRead(strVal, &r, HASHSEEDLEN);
  return GetHashValue(r);
}

SIZE_T GetKmer(const CReference * refGenome, const SIZE_T & nRefStart,
               SIZE_T kmerLen, InBits * r) {
  /* This function get kmerLen characters from the genome, start from position nRefstart*/
  if (nRefStart + kmerLen > refGenome->nRefSize) {
    kmerLen = refGenome->nRefSize - nRefStart;
  }

  r->ub = 0;
  r->lb = 0;

  SIZE_T indexInWords = nRefStart / wordSize;
  SIZE_T bitsShift = nRefStart % wordSize;
  /* the bitsShift bits are delete from the refInBits[indexInWords]
   * get (WORD_SIZE - bitsShift) bits */
  r->ub = refGenome->refInBits[indexInWords].ub >> bitsShift;
  r->lb = refGenome->refInBits[indexInWords].lb >> bitsShift;

  /* kmer in two WORDSIZE, here kmerLen should less than WORD_SIZE */
  if (bitsShift != 0) {
    /* delete the high  (wordSize - bitsShift) bits, and get (bitsShift) bits */
    r->ub |= (refGenome->refInBits[indexInWords + 1].ub
        << (wordSize - bitsShift));
    r->lb |= (refGenome->refInBits[indexInWords + 1].lb
        << (wordSize - bitsShift));
  }

  SIZE_T elimatedBits = wordSize - kmerLen;
  r->ub <<= elimatedBits;
  r->lb <<= elimatedBits;
  r->ub >>= elimatedBits;
  r->lb >>= elimatedBits;
  //printWORD(r->ub, kmerLen);
  //printWORD(r->lb, kmerLen);
  //cout << r->ub << endl;
  //cout << r->lb << endl;
  //cout << endl;
  return kmerLen;
}
