#ifndef _DATA_FILES_MANAGAR_H
#define _DATA_FILES_MANAGAR_H

#include "Common.h"
#include <fstream>
#include <unordered_map>

const size_t DATA_FILE_SIZE_LIMIT = 16 * 1048576;//16MB


class DataFilesManager
{

public:
	std::string appendDataBlock(const Data& dataBlock);
	void readDataBlock(const DedupedDataInfo& dedupeInfo, Data& dataBlockOut);

	/* Constuctor will open all existing data files one by one and populate the map*/
	DataFilesManager();
	~DataFilesManager();

	std::string getCurrentDataFilename() { return currentDatafileOutputFilename_m; }
private:
	
	void openAllDataFiles(const std::string& datafileDir);

	size_t getDataFileSizeLimit();

	std::string generateNextFilename(const std::string& currentFilename);

	std::ofstream currentOutputDataFile_strm;
	std::string currentDatafileOutputFilename_m;

	std::unordered_map<std::string, std::ifstream> dataFilenameMap_m; // data file name and opened data file to read
	const std::string dataFilesDir_m = "data_files"; //relative path
	const char* FIRST_FILE_NAME = "1";
};
#endif