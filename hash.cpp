#include "hash.h"

void CountBucketSize(const CReference * refGenome, CHashTable * hashTable) {
  //LOG_INFO;
  InBits r;
  for (SIZE_T i = 0; i < refGenome->nRefSize; i++) {
    if (GetKmer(refGenome, i, HASHSEEDLEN, &r) < HASHSEEDLEN)
      break;
    hashTable->counter[GetHashValue(r)]++;
  }

  for (SIZE_T i = 1; i <= NO_OF_BUCKET; i++) {
    hashTable->counter[i] += hashTable->counter[i - 1];
  }

  hashTable->nSizeIndex = hashTable->counter[NO_OF_BUCKET];
  INFO("The size of Hash Table Index Array is", hashTable->nSizeIndex);

  for (SIZE_T i = NO_OF_BUCKET - 1; i >= 1; i--) {
    hashTable->counter[i] = hashTable->counter[i - 1];
  }
  hashTable->counter[0] = 0;
}

void HashToBucket(const CReference * refGenome, CHashTable * hashTable) {
  INFO("The memory of Hash Table Index Array is",
       sizeof(SIZE_T) * (hashTable->nSizeIndex / GB), "GB");
  MEMORY_ALLOCATE_CHECK(
      hashTable->index = (SIZE_T *) malloc(
          sizeof(SIZE_T) * hashTable->nSizeIndex));
  InBits r;
  for (SIZE_T i = 0; i < refGenome->nRefSize; i++) {
    if (GetKmer(refGenome, i, HASHSEEDLEN, &r) < HASHSEEDLEN)
      break;
    hashTable->index[hashTable->counter[GetHashValue(r)]++] = i;
  }

  for (SIZE_T i = NO_OF_BUCKET - 1; i >= 1; i--) {
    hashTable->counter[i] = hashTable->counter[i - 1];
  }
  hashTable->counter[0] = 0;
}

//void TestHashTalbe(const CReference * refGenome, const CHashTable * hashTable) {
//	return;
//	LOG_INFO;
//	InBits r;
//	ofstream fsee("test_out_sort.txt");
//	for (SIZE_T i = 0; i < NO_OF_BUCKET; i++) {
//		//if (i != 0 && i % 93 != 0)
//		//continue;
//		SIZE_T start = hashTable->counter[i];
//		if (hashTable->counter[i + 1] <= start)
//			continue;
//		for (SIZE_T j = start; j < hashTable->counter[i + 1]; j++) {
//			SIZE_T s = GetKmer(refGenome, hashTable->index[j], HASHSEEDLEN, &r);
//			char strRead[HASHSEEDLEN + 1];
//			DecodeRead(strRead, s, &r);
//			fsee << "j=" << j << " i=" << i << " " << strRead << " gpos= " << hashTable->index[j] << "e" << endl;
//		}
//		fsee << endl;
//	}
//	fsee.close();
//	LOG_INFO;
//}

void WriteIndexAndRef(const Option & opt, const CReference * refGenome,
                      const CHashTable * hashTable) {
  FILE * fout = fopen(opt.indexFile.c_str(), "wb");
  INFO("write reference and index to", opt.indexFile);
  fwrite(&(refGenome->nRefSize), sizeof(SIZE_T), 1, fout);
  fwrite(&(refGenome->nRefSizeInWordSize), sizeof(SIZE_T), 1, fout);
  fwrite(refGenome->refInBits, sizeof(InBits), refGenome->nRefSizeInWordSize,
         fout);
  fwrite(&(hashTable->nSizeCounter), sizeof(SIZE_T), 1, fout);
  fwrite(hashTable->counter, sizeof(SIZE_T), hashTable->nSizeCounter, fout);
  fwrite(&(hashTable->nSizeIndex), sizeof(SIZE_T), 1, fout);
  fwrite(hashTable->index, sizeof(SIZE_T), hashTable->nSizeIndex, fout);
  fclose(fout);
}

void ReadIndexAndRef(const Option & opt, CReference * refGenome,
                     CHashTable * hashTable) {
  printf("\n");
  INFO("Read reference index from", opt.refFile);

  FILE * fin = fopen(opt.refFile.c_str(), "rb");
  FILE_OPEN_CHECK(fin);
  FREAD_CHECK(fread(&(refGenome->nRefSize), sizeof(SIZE_T), 1, fin), 1);
  FREAD_CHECK(fread(&(refGenome->nRefSizeInWordSize), sizeof(SIZE_T), 1, fin),
              1);
  MEMORY_ALLOCATE_CHECK(
      refGenome->refInBits = (InBits *) malloc(
          sizeof(InBits) * refGenome->nRefSizeInWordSize));
  FREAD_CHECK(
      fread(refGenome->refInBits, sizeof(InBits), refGenome->nRefSizeInWordSize,
            fin),
      refGenome->nRefSizeInWordSize);

  FREAD_CHECK(fread(&(hashTable->nSizeCounter), sizeof(SIZE_T), 1, fin), 1);
  MEMORY_ALLOCATE_CHECK(
      hashTable->counter = (SIZE_T *) malloc(
          sizeof(SIZE_T) * hashTable->nSizeCounter));
  FREAD_CHECK(
      fread(hashTable->counter, sizeof(SIZE_T), hashTable->nSizeCounter, fin),
      hashTable->nSizeCounter);

  FREAD_CHECK(fread(&(hashTable->nSizeIndex), sizeof(SIZE_T), 1, fin), 1);
  MEMORY_ALLOCATE_CHECK(
      hashTable->index = (SIZE_T *) malloc(
          sizeof(SIZE_T) * hashTable->nSizeIndex));
  FREAD_CHECK(
      fread(hashTable->index, sizeof(SIZE_T), hashTable->nSizeIndex, fin),
      hashTable->nSizeIndex);
  fclose(fin);
}

void BuildHashTable(const Option & opt, const CReference * refGenome,
                    CHashTable * hashTable) {
  printf("\n");
  INFO("Build hash table...");
  /* 64M buckets, 256 MB  4^13, 67108864, 0100000000000000000000000000 */
  hashTable->nSizeCounter = NO_OF_BUCKET;
  INFO("The size of Hash Table Counter Array is",
       sizeof(SIZE_T) * (hashTable->nSizeCounter + 1) / GB, "GB");
  SIZE_T sizeCounter = sizeof(SIZE_T) * (hashTable->nSizeCounter + 1);
  MEMORY_ALLOCATE_CHECK(hashTable->counter = (SIZE_T *) malloc(sizeCounter));
  memset(hashTable->counter, 0x00, sizeCounter);

  TIME_INFO(CountBucketSize(refGenome, hashTable), "Count bucket size"); /* count each bucket size */
  TIME_INFO(HashToBucket(refGenome, hashTable), "Hash to bucket"); /* put each element into a bucket */
  //TIME_INFO(TestHashTalbe(refGenome, hashTable), "test the hash table");
  if (opt.bSaveIndex == 1) {
    TIME_INFO(WriteIndexAndRef(opt, refGenome, hashTable),
              "Write reference and index");
  }
}
