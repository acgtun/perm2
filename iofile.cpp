#include "iofile.h"

SIZE_T GetLineFromString(const char * strVal, char * strRet) {
	SIZE_T i;
	for (i = 0; strVal[i] != 0; i++) {
		//	cout << strVal[i] << endl;
		if (0xA == strVal[i] || 0xD == strVal[i]) {
			//	cout << "again" << endl;
			break;
		}
	}
	memcpy(strRet, strVal, i);
	strRet[i] = 0;
	return i;
}

SIZE_T ReadWholeFile(const string & fileName, char **strRet) {
	//LOG_INFO;
	FILE * fin = fopen(fileName.c_str(), "rb");
	FILE_OPEN_CHECK(fin);
	//LOG_INFO;
	fseek(fin, 0, SEEK_END);
	SIZE_T size = ftell(fin);
	MEMORY_ALLOCATE_CHECK(*strRet = (char*) malloc(sizeof(char) * (size + 1)));
	fseek(fin, 0, SEEK_SET);
	FREAD_CHECK(fread(*strRet, 1, size, fin), size);
	fclose(fin);
	return size;
}

void WriteWholeFile(const string & fileName, char * strRet, SIZE_T len) {
	FILE * fout = fopen(fileName.c_str(), "wb");
	FILE_OPEN_CHECK(fout);
	fwrite(strRet, 1, len, fout);
	fclose(fout);
}

