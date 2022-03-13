#pragma once
#include "Common.h"
#include <fstream>
#include <unordered_map>

class DataFilesManager
{

public:
	void createOrOpenDataFile(const std::string& fileName);
	void appendDataBlock(const Data& dataBlock);
	void readDataBlock(const Data& dataBlock);

	//std::string getDataFilename(const std::string filename);
	//DataFilesManager(const std::string filename, std::ios_base::openmode fileOpenMode);
	DataFilesManager();
	~DataFilesManager();
private:
	
	void openAllDataFiles(const std::string& datafileDir);

	std::ofstream outputDataFileStrm_m;
	std::string filename_m;
	//std::ifstream dataFileInputStrm_m;

	std::unordered_map<std::string, std::ifstream> dataFilenameMap; // data file name and opened data file to read
	const std::string dataFilesDir = "data_files"; //relative path
};