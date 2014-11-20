#pragma once
#ifndef IOFILE_H_
#define IOFILE_H_

#include "stdafx.h"

SIZE_T ReadWholeFile(const string & fileName, char ** strRet);
SIZE_T GetLineFromString(const char * strVal, char * strRet);
void WriteWholeFile(const char * fileName, char * strRet, SIZE_T len);
void OutPutResult(FILE * fout, CResult * result, SIZE_T nReads, SIZE_T readID);

#endif /* IOFILE_H_ */
