#pragma once
#include "Common.h"
#include <fstream>

class DataFileManager
{

public:
	void createOrOpenDataFile(const std::string& fileName);
	void appendDataBlock(const Data& dataBlock);
	void readDataBlock(const Data& dataBlock);

	std::string getDataFilename(const std::string filename);
	DataFileManager(const std::string filename, std::ios_base::openmode fileOpenMode);
	~DataFileManager();
private:
	std::ofstream outputDataFileStrm_m;
	std::string filename_m;
	std::ifstream dataFileInputStrm_m;
};