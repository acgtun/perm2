#pragma once
#ifndef PARAMETER_H_
#define PARAMETER_H_

#include "stdafx.h"
#include "bitscode.h"
#include <string>

using namespace std;

typedef struct {
	/* mapping parameter */
	SIZE_T nMaxMismatch;
	SIZE_T ems; // number of exact mapped seeds
} MapOpt;

typedef struct {
	//input and out parameter
	string readsFile;
	string refFile;

	SIZE_T nNumOfreads;
	SIZE_T readLen;
	SIZE_T nSeed;
	SIZE_T * seedStartPos;
	string outputFile;
	string indexFile;
	int bSaveIndex; // Default is false
	int bIndexExist;

	MapOpt mapOpt;

	/////////////////////////////////
	//set<SIZE_T> setConsectiveN;
} Option;

void PrintSynopsis();
void GetParameter(int argc, const char* argv[], Option & opt);

#endif /* PARAMETER_H_ */
