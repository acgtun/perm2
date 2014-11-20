#include "match.h"


//////////////////////////////////////////////////////////////////////////////////
/*
 * seed length L
 * read length r
 * number of error e
 * number of seeds r/L
 * number of exact matched seeds ems = r/L - e
 * so if the region has less than ems, then this candidate region don't need to do pair-wise alignment
 * for these pair-wise alignemnt, we need to use the information we got in the first step,
 *  (1) for the two ends, we do semi-global alignment
 *  (2) for the mid of read, we do global alignment, full use the information we got in the first step
 */
//////////////////////////////////////////////////////////////////////////////////
void reverseCompliment(char * strRead_rev, const char * strRead, const SIZE_T & len) {
	for (SIZE_T i = 0; i < len; i++) {
		strRead_rev[i] = complimentBase(strRead[len - i - 1]);
	}
	strRead_rev[len] = 0;
}

void GetPos(const CHashTable * hashTable, const SIZE_T & hashValue, vector<pair<SIZE_T, SIZE_T> > & candPos,
		SIZE_T & candPosID, const SIZE_T & seedID) {
	SIZE_T l = hashTable->counter[hashValue];
	if (hashTable->counter[hashValue + 1] == 0)
		return;
	SIZE_T u = hashTable->counter[hashValue + 1] - 1;
	if (l > u)
		return;
	if (u - l + 1 > 10000)
		return;

	for (SIZE_T i = l; i <= u; i++) {
		candPos[candPosID++] = pair<SIZE_T, SIZE_T>(hashTable->index[i], seedID);
	}
}

void UsingSeedGenCandidate(const Option & opt, const CHashTable * hashTable, const char * strRead,
		vector<pair<SIZE_T, SIZE_T> > & candPos, SIZE_T & candPosID) {
	candPosID = 0;
	for (SIZE_T i = 0; i < opt.nSeed - 1; i++) {
		SIZE_T hashValue = GetHashValue(&strRead[i * HASHSEEDLEN], HASHSEEDLEN);
		GetPos(hashTable, hashValue, candPos, candPosID, i);
	}
	SIZE_T hashValue = GetHashValue(&strRead[opt.readLen - HASHSEEDLEN], HASHSEEDLEN);
	GetPos(hashTable, hashValue, candPos, candPosID, opt.nSeed - 1);
}

void FindRegion(const Option & opt, const vector<pair<SIZE_T, SIZE_T> > & candPos, const SIZE_T & candPosID,
		vector<bool> & candPosFlag, const int & start, vector<SIZE_T> & oneRegion, SIZE_T & oneRegionID) {
	oneRegionID = 0;
	oneRegion[oneRegionID++] = start;
	candPosFlag[start] = true;
	SIZE_T seedID_pre = start, posDis, thDis;
	for (SIZE_T i = start + 1; i < candPosID; i++) {
		if (candPos[i].second <= candPos[seedID_pre].second)
			continue;
		posDis = candPos[i].first - candPos[seedID_pre].first;
		thDis = opt.seedStartPos[candPos[i].second] - opt.seedStartPos[candPos[seedID_pre].second];
		if (posDis > thDis + opt.mapOpt.nMaxMismatch)
			break;
		if (posDis + opt.mapOpt.nMaxMismatch < thDis)
			continue;
		oneRegion[oneRegionID++] = i;
		candPosFlag[i] = true;
		seedID_pre = i;
	}
}

bool Candidate_CMP(const pair<SIZE_T, int> & a, const pair<SIZE_T, int> & b) {
	return a.first < b.first;
}

bool VerifyRegion(const Option & opt, const CReference * refGenome, const char * strRead,
		vector<pair<SIZE_T, SIZE_T> > & candPos, vector<bool> & candPosFlag, vector<int> & L, vector<int> & R,
		vector<vector<int> > & s, vector<vector<char> > & l, const vector<SIZE_T> & oneRegion,
		const SIZE_T & oneRegionID, SIZE_T & totalError) {

	char strU[MAX_LINE_LEN], strV[MAX_LINE_LEN];
	int cpl, NumOfError = 0;
	totalError = 0;
	InBits r;

	totalError = 0;
	if (candPos[oneRegion[0]].second != 0) {
		cpl = candPos[oneRegion[0]].second * HASHSEEDLEN;
		memcpy(strU, strRead, cpl);
		strU[cpl] = 0;
		SIZE_T lV = GetKmer(refGenome, candPos[oneRegion[0]].first - cpl, cpl, &r);
		DecodeRead(strV, lV, &r);
		NumOfError = 0;
		if (runBandedGlobalAlignment(strU, strV, L, R, s, l, opt.mapOpt.nMaxMismatch, NumOfError) == false)
			return false;
		totalError += NumOfError;
		if (totalError > opt.mapOpt.nMaxMismatch)
			return false;
	}

	for (SIZE_T j = 1; j < oneRegionID; j++) {
		int seedGap = candPos[oneRegion[j]].second - candPos[oneRegion[j - 1]].second;
		if (seedGap != 1) {
			cpl = (seedGap - 1) * HASHSEEDLEN;
			memcpy(strU, &strRead[(candPos[oneRegion[j - 1]].second + 1) * HASHSEEDLEN], cpl);
			strU[cpl] = 0;
			SIZE_T lV = GetKmer(refGenome, candPos[oneRegion[j - 1]].first + HASHSEEDLEN, cpl, &r);
			DecodeRead(strV, lV, &r);
			NumOfError = 0;
			if (runBandedGlobalAlignment(strU, strV, L, R, s, l, opt.mapOpt.nMaxMismatch, NumOfError) == false)
				return false;
			totalError += NumOfError;
			if (totalError > opt.mapOpt.nMaxMismatch)
				return false;
		}
	}

	if (candPos[oneRegion[oneRegionID - 1]].second != opt.nSeed - 1) {
		cpl = opt.readLen - (candPos[oneRegion[oneRegionID - 1]].second + 1) * HASHSEEDLEN;
		memcpy(strU, &strRead[(candPos[oneRegion[oneRegionID - 1]].second + 1) * HASHSEEDLEN], cpl);
		strU[cpl] = 0;
		SIZE_T lV = GetKmer(refGenome, candPos[oneRegion[oneRegionID - 1]].first + HASHSEEDLEN, cpl, &r);
		DecodeRead(strV, lV, &r);
		NumOfError = 0;
		if (runBandedGlobalAlignment(strU, strV, L, R, s, l, opt.mapOpt.nMaxMismatch, NumOfError) == false)
			return false;
		totalError += NumOfError;
		if (totalError > opt.mapOpt.nMaxMismatch)
			return false;
	}
	return true;
}

void MappingOneRead(const Option & opt, const CReference * refGenome, const CHashTable * hashTable,
		const char * strRead, vector<pair<SIZE_T, SIZE_T> > & candPos, vector<bool> & candPosFlag, vector<int> & L,
		vector<int> & R, vector<vector<int> > & s, vector<vector<char> > & l) {
	//cout << "Read: " << strRead << endl;
	SIZE_T candPosID = 0;
	UsingSeedGenCandidate(opt, hashTable, strRead, candPos, candPosID);
	sort(candPos.begin(), candPos.begin() + candPosID, Candidate_CMP);

	for (SIZE_T i = 0; i < candPosID; i++) {
		candPosFlag[i] = false;
	}

	vector<SIZE_T> oneRegion(opt.nSeed, 0);
	SIZE_T oneRegionID = 0;
//	int curRegionID = 0;
	for (SIZE_T i = 0; i < candPosID; i++) {
		if (candPosFlag[i] == true)
			continue;
		FindRegion(opt, candPos, candPosID, candPosFlag, i, oneRegion, oneRegionID);
		if (oneRegionID < opt.mapOpt.ems)
			continue;
		if (candPos[oneRegion[oneRegionID - 1]].second + 1 - opt.nSeed == 0 && (oneRegionID < opt.mapOpt.ems + 1))
			continue;
		//printf("region %d: ", curRegionID++);

	//	for (SIZE_T j = 0; j < oneRegionID; j++) {
	//		cout << candPos[oneRegion[j]].first << " ";
	//	}
		//cout << " *** ";
		//for (SIZE_T j = 0; j < oneRegionID; j++) {
		//	cout << candPos[oneRegion[j]].second << " ";
		//}

		SIZE_T totalError = 0;
		if (oneRegionID == opt.nSeed) {
			cout << "(" << candPos[oneRegion[0]].first - candPos[oneRegion[0]].second * HASHSEEDLEN << ", 0)";
		} else if (VerifyRegion(opt, refGenome, strRead, candPos, candPosFlag, L, R, s, l, oneRegion, oneRegionID,
					totalError)) {
			cout << "(" << candPos[oneRegion[0]].first - candPos[oneRegion[0]].second * HASHSEEDLEN << ", " << totalError << ")";
		} else{}
	}
}

void Matching(const Option & opt, const CReference * refGenome, const CHashTable * hashTable) {
	int readID = 0;
	char * strReads;
	SIZE_T readsLen = ReadWholeFile(opt.readsFile, &strReads);
	char strRead[MAX_LINE_LEN], strRead_rev[MAX_LINE_LEN];

	vector<int> L(HASHSEEDLEN * 5 + 2, 0);
	vector<int> R(HASHSEEDLEN * 5 + 2, 0);
	vector<vector<int> > s(HASHSEEDLEN * 5 + 2, vector<int>(130, 0)); //number of error cannot be more than 100
	vector<vector<char> > l(HASHSEEDLEN * 5 + 2, vector<char>(130, 0));

	vector<pair<SIZE_T, SIZE_T> > candPos(opt.nSeed * 10000, pair<SIZE_T, SIZE_T>(0, 0)); //candidatePosition平均不能大于5000
	vector<bool> candPosFlag(opt.nSeed * 10000, false);
	//LOG_INFO;
	SIZE_T readLen;
	for (SIZE_T i = 0; i < readsLen; i++) {
		readLen = GetLineFromString(&strReads[i], strRead);
		i += readLen;
		if (strRead[0] == '>')
			continue;
		if (readLen != opt.readLen) {
			printf("read length is not identical. please check the reads file.\n");
			exit(EXIT_FAILURE);
		}
		cout << "read " << readID++ << ": ";

		MappingOneRead(opt, refGenome, hashTable, strRead, candPos, candPosFlag, L, R, s, l);
		reverseCompliment(strRead_rev, strRead, opt.readLen);
		MappingOneRead(opt, refGenome, hashTable, strRead_rev, candPos, candPosFlag, L, R, s, l);
		cout << endl;
	}
	free(strReads);
}
