#pragma once
#include "Common.h"
#include <fstream>
#include <unordered_map>

const size_t DATA_FILE_SIZE_LIMIT = 16 * 1048576;//16MB

class DataFilesManager
{

public:
	//void createOrOpenDataFile(const std::string& fileName);
	void appendDataBlock(const Data& dataBlock);
	void readDataBlock(const DedupedDataInfo& dedupeInfo, const Data& dataBlock);

	//std::string getDataFilename(const std::string filename);
	//DataFilesManager(const std::string filename, std::ios_base::openmode fileOpenMode);
	DataFilesManager();
	~DataFilesManager();
private:
	
	void openAllDataFiles(const std::string& datafileDir);

	size_t getDataFileSizeLimit();

	std::ofstream currentOutputDataFile_strm;
	std::string currentDatafileOutputFilename_m;
	//std::ifstream dataFileInputStrm_m;

	std::unordered_map<std::string, std::ifstream> dataFilenameMap; // data file name and opened data file to read
	const std::string dataFilesDir_m = "data_files"; //relative path
};