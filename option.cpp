#include "option.h"

void PrintSynopsis() {
	printf(
			"The input command is incorrect.\nFor more info, please check: http://code.google.com/p/perm/\n");
}

int GetIntVal(int argc, const char** argv, const char * str, SIZE_T & nVal) {
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], str) == 0 && i + 1 <= argc - 1) {
			if (argv[i + 1][0] != '-') {
				nVal = (SIZE_T) atoi(argv[i + 1]);
				return 1;
			}
		}
	}
	return 0;
}

int GetStrVal(int argc, const char** argv, const char* str, string & strVal) {
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], str) == 0 && i + 1 <= argc - 1) {
			if (argv[i + 1][0] != '-') {
				strVal = argv[i + 1];
				return 1;
			}
		}
	}
	return 0;
}

int ChkStrExist(int argc, const char** argv, const char* str) {
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], str) == 0) {
			return 1;
		}
	}
	return 0;
}

void GetNameBeforeDot(const string & strFile, string & fileName) {
	int pos = strFile.find_last_of('.');
	fileName = strFile.substr(0, pos);
}

void GetReadLength(Option & opt) {
	ifstream fin(opt.readsFile.c_str());
	if (!fin.good()) {
		printf("--ERROR INFO-- reads file open error. %s\n", opt.readsFile.c_str());
		exit (EXIT_FAILURE);
	}
	opt.nNumOfreads = 0;
	string strLine;
	while (getline(fin, strLine)) {
		if (strLine[0] == '>') {
			opt.nNumOfreads++;
			continue;
		}
		opt.readLen = strLine.size();
		cout << "--INFO-- Full sensitivity for " << opt.mapOpt.nMaxMismatch << " errors." << endl;
		cout << "--INFO-- The length of the reads is " << opt.readLen << endl;
		if (opt.readLen / HASHSEEDLEN <= opt.mapOpt.nMaxMismatch) {
			//printf("--ERROR INFO-- seed length is not proper. please set shorter seed length.\n");
			//exit (EXIT_FAILURE);
		}
		opt.mapOpt.ems = opt.readLen / HASHSEEDLEN - opt.mapOpt.nMaxMismatch;
		opt.nSeed = (opt.readLen - 1) / HASHSEEDLEN + 1;
		if (opt.nSeed < 1) {
			//printf("--ERROR INFO-- seed length is not proper. please set shorter seed length.\n");
			//exit (EXIT_FAILURE);
		}
		MEMORY_ALLOCATE_CHECK(opt.seedStartPos = (SIZE_T * ) malloc(sizeof(SIZE_T) * opt.nSeed));
		for (SIZE_T i = 0; i < opt.nSeed - 1; i++) {
			opt.seedStartPos[i] = HASHSEEDLEN * i;
		}
		opt.seedStartPos[opt.nSeed - 1] = opt.readLen - HASHSEEDLEN;
		cout << "--INFO-- Seeds start positions are ";
		for (SIZE_T i = 0; i < opt.nSeed; i++) {
			cout << opt.seedStartPos[i] << ", ";
		}
		cout << "respectively" << endl;
		cout << "--INFO-- The matched positions should have at least " << opt.mapOpt.ems
				<< " exact seeds for each read." << endl;
		break;
	}

	while (getline(fin, strLine)) {
		if (strLine[0] == '>') {
			opt.nNumOfreads++;
			continue;
		}
	}
	fin.close();
	INFO("There are", opt.nNumOfreads, "reads in the reads file");
}

void PrintOutVersion() {
	printf("####################################################\n");
	printf("#                                                  #\n");
	printf("#  seed-extension-aligner                          #\n");
	printf("#  Haifeng Chen (haifengc at usc dot edu)          #\n");
	printf("#  University of Southern California               #\n");
	printf("#  Feb 5, 2014                                     #\n");
	printf("#                                                  #\n");
	printf("####################################################\n");
	printf("\n");
}

void GetParameter(int argc, const char* argv[], Option & opt) {
	if (argc <= 2 || argv == NULL)
		PrintSynopsis();
	else {
		PrintOutVersion();
		cout << "--INFO-- Input command:";
		for (int i = 0; i < argc; i++) {
			cout << " " << argv[i];
		}
		cout << endl;

		opt.refFile = argv[1];
		cout << "--INFO-- The reference file is " << opt.refFile << endl;

		opt.bIndexExist = 0;
		string indexsf = ".sdindex";
		if (opt.refFile.size() > indexsf.size()
				&& opt.refFile.substr(opt.refFile.size() - indexsf.size()) == indexsf) {
			opt.bIndexExist = 1;
		}
		opt.readsFile = argv[2];
		cout << "--INFO-- The reads file is " << opt.readsFile << "." << endl;

		if (GetIntVal(argc, argv, "-v", opt.mapOpt.nMaxMismatch) == 0)
			opt.mapOpt.nMaxMismatch = 2;

		if (GetStrVal(argc, argv, "-o", opt.outputFile) == 0) {
			string fileName;
			GetNameBeforeDot(opt.readsFile, fileName);
			opt.outputFile = fileName;
			opt.outputFile += ".sam";
		}
		opt.bSaveIndex = 0;
		if (ChkStrExist(argc, argv, "-s")) {
			opt.bSaveIndex = 1;
		}
		if (GetStrVal(argc, argv, "-s", opt.indexFile) == 0 && opt.bSaveIndex == 1) {
			string fileName;
			GetNameBeforeDot(opt.refFile, fileName);
			opt.indexFile = fileName;
			opt.indexFile += indexsf;
		}
		GetReadLength(opt);
	}
}
