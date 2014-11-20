#include "refin.h"

int RemoveNonACGTNBase(char * strRef, SIZE_T refLen) {
	/* This function removes all non-ACGTN characters */
	char strRet[MAX_LINE_LEN];
	SIZE_T j = 0;
	for (SIZE_T i = 0; i < refLen; i++) {
		if (strRef[i] == '>') {
			i += GetLineFromString(&strRef[i], strRet);
		} else if (isACGT(strRef[i]) || strRef[i] == 'N' || strRef[i] == 'n') {
			strRef[j++] = toupper(strRef[i]);
		}
	}
	strRef[j] = 0;

	/* change all 'N' to A,C,G,T with the same probability*/
	srand(time(NULL));
	int r = 0;
	for (SIZE_T i = 0; i < j; i++) {
		if (strRef[i] == 'N') {
			r = rand() % 4;
			strRef[i] = getNT(r);
		}
	}

	return j;
}

//void genTestData(char * strRef, SIZE_T len) {
//	FILE * fread = fopen("NC_008253_len63_100.fa", "wb");
//	FILE * fans = fopen("NC_008253_len63_ans_100.txt", "wb");
//
//	srand(time(NULL));
//
//	for (SIZE_T i = 0; i < 10; i++) {
//		int r = rand() % len;
//		fprintf(fread, ">read%d\n", i);
//		for (SIZE_T j = r, n = 0; j < len && n < 63; n++, j++) {
//			fprintf(fread, "%c", strRef[j]);
//		}
//		fprintf(fread, "\n");
//		fprintf(fans, "read%d %d\n", i, r);
//	}
//	fclose(fread);
//	fclose(fans);
//}

void RefEncodeToBits(CReference * refGenome, const char * strRef) {
	/* This function transfers the genome to binary format. The binary format genome
	 * is stored as many InBits, which has two WORD_SIZE. For each character A(00),C(01),
	 * G(10),T(11), the upper bit stores in ub, and lower bit stores in lb.
	 * */

	refGenome->nRefSizeInWordSize = (refGenome->nRefSize - 1) / wordSize + 1;
	MEMORY_ALLOCATE_CHECK(refGenome->refInBits = (InBits *) malloc(sizeof(InBits) * refGenome->nRefSizeInWordSize));
	char strReads[wordSize + 1];
	for (SIZE_T i = 0; i < refGenome->nRefSizeInWordSize - 1; i++) {
		memcpy(&strReads, &(strRef[i * wordSize]), wordSize);
		strReads[wordSize] = 0;
		EncodeRead(strReads, &(refGenome->refInBits[i]), wordSize);
	}
	SIZE_T codesize = (refGenome->nRefSizeInWordSize - 1) * wordSize;
	SIZE_T remSize = refGenome->nRefSize - codesize;
	memcpy(strReads, &(strRef[codesize]), (SIZE_T) remSize);
	strReads[remSize] = 0;
	EncodeRead(strReads, &(refGenome->refInBits[refGenome->nRefSizeInWordSize - 1]), remSize);
}

void GetReference(CReference * refGenome, Option & opt) {
	char * strRef;
	INFO("Read reference genome from", opt.refFile);
	SIZE_T refLen = ReadWholeFile(opt.refFile, &strRef);
	INFO("The length of reference file is", refLen);
	refGenome->nRefSize = RemoveNonACGTNBase(strRef, refLen);
	//cout << refGenome->nRefSize << endl;
	//genTestData(strRef, refGenome->nRefSize);
	RefEncodeToBits(refGenome, strRef);
	//cout << "size = " << refGenome->nRefSize << endl;
	free(strRef);
}
