#pragma once
#ifndef SEED_H_
#define SEED_H_

#include "stdafx.h"
#include "refin.h"

SIZE_T GetHashValue(const InBits & r);
SIZE_T GetHashValue(const char * strVal, const int & len);
SIZE_T GetKmer(const CReference * refGenome, const SIZE_T & nRefStart, SIZE_T kmerLen, InBits * r);

#endif /* SEED_H_ */
