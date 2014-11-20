#include "bandedGlobalAlignment.h"

#define I (i - 1)
#define J (j - 1)
#define P (p - 1)
#define Q (q - 1)
#define DIAG ('a')
#define UP ('b')
#define LEFT ('c')
#define slCOL(row, col) ((col) - L[(row)])
#define inf numeric_limits<int>::max()

enum MatchLabel {
	MATCH = 0, MISMATCH = 1, INDEL = 2
};

int w[] = { 0, -2, -3 };

MatchLabel charMatch(const char & a, const char & b) {
	if (a == b)
		return MATCH;
	else
		return MISMATCH;
}

void max(const int & s1, const int & s2, const int & s3, int & maxs, char & maxl) {
	/*if two of them are equal, then there are more than one optimal path*/
	if (s1 >= s2) {
		if (s1 >= s3) {
			maxs = s1;
			maxl = DIAG;
			return;
		} else {
			maxs = s3;
			maxl = LEFT;
			return;
		}
	} else {
		if (s2 >= s3) {
			maxs = s2;
			maxl = UP;
			return;
		} else {
			maxs = s3;
			maxl = LEFT;
			return;
		}
	}
}

void setLR(const int & k, const int & m, const int & n, vector<int> & L, vector<int> & R) {
	int d = m >= n ? m - n : n - m;
	if (m >= n) {
		for (int i = 0; i <= n; i++) {
			L[i] = i - k / 2 > 0 ? i - k / 2 : 0;
			R[i] = i + d + k / 2 < m ? i + d + k / 2 : m;
		}
	} else {
		for (int i = 0; i <= n; i++) {
			L[i] = i - d - k / 2 > 0 ? i - d - k / 2 : 0;
			R[i] = i + k / 2 < m ? i + k / 2 : m;
		}
	}
}

bool bandedGlobalAlign(const int & k, const string & U, const string & V, vector<int> & L,
		vector<int> & R, vector<vector<int> > & s, vector<vector<char> > & l, int & NumOfError) {
	int n = U.size();
	int m = V.size();
	int d = m >= n ? m - n : n - m;
	setLR(k, m, n, L, R);

	for (int i = 0; i <= n; i++) {
		for (int j = 0; j < R[i] - L[i] + 1; j++) {
			s[i][j] = 0;
			l[i][j] = DIAG;
		}
	}

	s[0][0] = 0;
	for (int j = L[0]; j <= R[0]; j++) {
		s[0][slCOL(0, j)] = j * w[INDEL];
		l[0][slCOL(0, j)] = LEFT;
	}

	char maxl;
	int s1, s2, s3, s1Col, s2Col, s3Col, maxs;
	for (int i = 1; i <= n; i++) {
		s[i][0] = i * w[INDEL];
		l[i][0] = UP;
		for (int j = L[i]; j <= R[i]; j++) {
			s1 = -inf;
			s2 = -inf;
			s3 = -inf;
			s1Col = slCOL(i - 1, j) - 1;
			s2Col = slCOL(i - 1, j);
			s3Col = slCOL(i, j) - 1;

			if (j - 1 >= L[i - 1] && j - 1 <= R[i - 1]) {
				s1 = s[i - 1][s1Col] + w[charMatch(U[I], V[J])];
			}
			if (j >= L[i - 1] && j <= R[i - 1]) {
				s2 = s[i - 1][s2Col] + w[INDEL];
			}
			if (j - 1 >= L[i] && j - 1 <= R[i]) {
				s3 = s[i][s3Col] + w[INDEL];
			}

			max(s1, s2, s3, maxs, maxl);
			s[i][slCOL(i, j)] = maxs;
			l[i][slCOL(i, j)] = maxl;
		}
	}

	int nDiff = 0;
	int p = n, q = m;
	//rU.clear();
	//rV.clear();
	while (p >= 0 && q >= 0 && (p + q != 0)) { // trace back from s[n][m] to s[0][0]
		if (l[p][slCOL(p, q)] == DIAG) {
			//rU.push_back(U[P]);
			//rV.push_back(V[Q]);
			if (U[P] != V[Q])
				nDiff++;
			p = p - 1;
			q = q - 1;
		} else if (l[p][slCOL(p, q)] == UP) {
			//rU.push_back(U[P]);
			//rV.push_back('-');
			p = p - 1;
			nDiff++;
		} else if (l[p][slCOL(p, q)] == LEFT) {
			//rU.push_back('-');
			//rV.push_back(V[Q]);
			q = q - 1;
			nDiff++;
		}

		if (nDiff > d + k) {
			return false;
		}
	}

	NumOfError = nDiff;
	return true;
}

bool CountDiff(const char * strU, const char * strV, const int & len, const int & nMaxNunofErro,
		int & NumOfError) {
	NumOfError = 0;
	for (int i = 0; i < len; i++) {
		if (strU[i] != strV[i]) {
			NumOfError++;
			if (NumOfError > nMaxNunofErro)
				return false;
		}
	}
	return true;
}
bool runBandedGlobalAlignment(const char * U, const char * V, vector<int> & L, vector<int> & R,
		vector<vector<int> > & s, vector<vector<char> > & l, const int & maxError,
		int & NumOfError) {
	NumOfError = 0;
	int m = strlen(U);
	int n = strlen(V);
	if (m == n) {
		return CountDiff(U, V, m, maxError, NumOfError);
	}
	for (int k = 1;; k *= 2) {
		if (k > maxError)
			return false;

		if (bandedGlobalAlign(k, U, V, L, R, s, l, NumOfError)) {
			if (NumOfError <= maxError)
				return true;
			return false;
		}
	}
	return false;
}
