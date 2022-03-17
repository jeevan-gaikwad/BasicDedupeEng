#pragma once
#pragma warning(disable : 4996)
#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include "Common.h"
#include "DataFilesManager.h"

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

	/* Restore duduped file from engine using createFileFromEngine function
	*/
	status_t createFileFromEngine(const std::string& filename);
	status_t calculateFingerprint(const Data& dataBuff, DedupedDataInfo& dedupeDataBuff);

	std::string createFileMetaDirIfNotExists(const std::string& filename);

private:

	/* Fingerprint and associated data info*/
	std::unordered_map<std::string, DedupedDataInfo> map; 

	/* Single object to manager all data files. Good candidate for singleton*/
	DataFilesManager dataFilesManager_m;
	const std::string outputFileSuffix_m = "_recovered";
};

