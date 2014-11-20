#pragma once
#ifndef HASH_H_
#define HASH_H_

#include "stdafx.h"
#include "refin.h"
#include "bitscode.h"
#include "seed.h"

typedef struct {
	SIZE_T * counter;
	SIZE_T * index;
	SIZE_T nSizeCounter;
	SIZE_T nSizeIndex;
} CHashTable;

void BuildHashTable(const Option & opt, const CReference * refGenome, CHashTable * hashTable);
void ReadIndexAndRef(const Option & opt, CReference * refGenome, CHashTable * hashTable);

#endif /* HASH_H_ */
