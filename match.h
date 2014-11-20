#ifndef MATCHING_H_
#define MATCHING_H_

#include "hash.h"
#include "refin.h"
#include "option.h"
#include "stdafx.h"
#include "seed.h"
#include "bandedGlobalAlignment.h"

void Matching(const Option & opt, const CReference * refGenome, const CHashTable * hashTable);

#endif /* MATCHING_H_ */
