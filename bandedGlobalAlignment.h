#pragma once
#ifndef BANDEDGLOBALALIGNMENT_H_
#define BANDEDGLOBALALIGNMENT_H_

#include "stdafx.h"

bool runBandedGlobalAlignment(const char * U, const char * V, vector<int> & L, vector<int> & R,
		vector<vector<int> > & s, vector<vector<char> > & l, const int & maxError, int & NumOfError);

#endif /* BANDEDGLOBALALIGNMENT_H_ */
