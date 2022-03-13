#pragma once
#pragma warning(disable : 4996)
#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include "Common.h"

const std::string dedupe_file_suffix =  "_dedupe";


class DedupeEngine {

public:
	/* 'initDedupeEngine' shall build the fingerprint cache and open all datafiles
	*  Currently this is single threaded, later this can be changed to do that things parallely using MT
	*/
	status_t initDedupeEngine();

	/* 'dedupeFile' file function dudupes the given file.
	*  It creates an index files under 'index_files' dir and a data file with unique blocks
	*/
	status_t dedupeFile(const std::string& fileName);
	status_t createFileFromEngine(const std::string& filename);
	status_t calculateFingerprint(const Data& dataBuff, DedupedDataInfo& dedupeDataBuff);
	//following 3 functions are expected to store file(s) metadata on the disk. dir/file
	std::string createFileMetaDirIfNotExists(const std::string& filename);

private:
	
	std::unordered_map<std::string, DedupedDataInfo> map; //fingerprint and associated data info
	DataFilesManager dataFileManager;
};

