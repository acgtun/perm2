#pragma once
#ifndef STDAFX_H_
#define STDAFX_H_

#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <map>
#include <set>
#include <limits>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

#define TEST

typedef unsigned int SIZE_T;

const double GB = 1024 * 1024 * 1024;

#define HASHSEEDLEN 13     /* use the first 13 characters as the hash value */
#define NO_OF_BUCKET 0x4000000  /* number of bucket in the hash table, 4^BITS_FOR_HASHING */
#define BUCKET_MAX_SIZE 10000

#define TYPE_NEW_LINE 0
#define TYPE_SPACE 1
#define TYPE_NOTHING 2

#define MAX_LINE_LEN 1024
#define MIN_READ_LEN 30
#define MAX_READ_LEN 200

typedef unsigned long long WORD_SIZE; /* no matter 32-bit or 64-bit, make sure they are identical. */
#define wordSize (8 * sizeof(WORD_SIZE))

inline void MemoryAllocateCheck(void * pointer, const char * file, int line) {
  if (pointer == NULL) {
    printf("Memory allocate error in %s at line %d\n", file, line);
    exit(EXIT_FAILURE);
  }
}

inline void FileOpenCheck(FILE * pfile, const char * file, int line) {
  if (pfile == NULL) {
    printf("File open error in %s at line %d\n", file, line);
    exit(EXIT_FAILURE);
  }
}

inline void printWORD_SIZE(WORD_SIZE word, SIZE_T len) {
  /* print the WORD_SIZE as binary format */
  for (SIZE_T i = 0; i < len; i++) {
    if ((word & 0x01) == 1) {
      printf("1");
    } else {
      printf("0");
    }
    word >>= 1;
  }
  printf("\n");
}

inline void checkReadLen(const SIZE_T & len, const SIZE_T & nReadsNum,
                         const char * file, int line) {
  if (len > MAX_READ_LEN || len < MIN_READ_LEN) {
    cout << "The length of read " << nReadsNum + 1 << " is not between "
        << MIN_READ_LEN << " and " << MAX_READ_LEN << ". It will be ignored."
        << "--- in " << file << " at line " << line << endl;
  }
}

//#define INFO(msg, file) cout << msg << " " << file << endl
#define HANDLE_ERROR(err) (HandleError( err, __FILE__, __LINE__ ))
#define LOG_INFO printf("--- %s:%s:%d\n",  __FILE__, __func__, __LINE__)
#define FILE_OPEN_CHECK(pfile) (FileOpenCheck( pfile, __FILE__, __LINE__))
#define CHECK_READ_LEN(len, nReadsNum) (checkReadLen(len, nReadsNum, __FILE__, __LINE__))
#define MEMORY_ALLOCATE_CHECK(pointer)  (MemoryAllocateCheck(pointer, __FILE__, __LINE__))
#define LOG_INFO_CPP cout << "-----" << __FILE__ << " " << __func__ << " " << __LINE__ << endl

#define FREAD_CHECK(func, size) { \
	SIZE_T s = func; \
	if(s != size) { \
		printf("read file error. --- %s:%s:%d\n", __FILE__, __func__, __LINE__); \
		exit(EXIT_FAILURE); \
	} \
}

#define TIME_INFO(func, msg) { \
	clock_t start_t, end_t; \
	start_t = clock(); \
	func; \
	end_t = clock(); \
	printf("--INFO-- %s takes %.3lf seconds.\n", msg, (double) ((end_t - start_t) / CLOCKS_PER_SEC )); \
}

typedef struct {
  SIZE_T nMismatch;  //it's very dangerous.
  SIZE_T nStartPos;
  char org_rev;
} CResult;

inline int isACGT(const char & nt) {
  switch (nt) {
    case 'a':
    case 'c':
    case 'g':
    case 't':
    case 'A':
    case 'C':
    case 'G':
    case 'T':
      return 1;
    default:
      return 0;
  }
}

inline void INFO(const string & msg) {
  cout << "--INFO-- " << msg << "." << endl;
}
inline void INFO(const string & msg, const string & val) {
  cout << "--INFO-- " << msg << " " << val << "." << endl;
}
inline void INFO(const string & msg, const SIZE_T &val) {
  cout << "--INFO-- " << msg << " " << val << "." << endl;
}
inline void INFO(const string & msg, const SIZE_T & val1, const string & val2) {
  cout << "--INFO-- " << msg << " " << val1 << " " << val2 << "." << endl;
}

#endif /* STDAFX_H_ */
