#include "Common.h"
#include "DataFilesManager.h"
#include "Logging.h"
#include "FileInterfaces.h"

//void DataFilesManager::createOrOpenDataFile(const std::string& filename) {
//
//	if (filename.empty()) {
//		logmsg(ERROR, "Empty filename received to function " << __func__);
//		throw std::invalid_argument("Empty filename received");
//	}
//	(void)filename; //unused for now. Creating single data file
//
//	std::string dataFilename = getDataFilename(filename);
//	currentOutputDataFile_strm.open(dataFilename, std::ofstream::binary); //change this mode to append later
//	if (!currentOutputDataFile_strm.is_open()) {
//		logmsg(ERROR, "Failed to open output data file " << dataFilename);
//	}
//	filename_m = dataFilename;
//	logmsg(DEBUG, "Output data file " << dataFilename << " opened successfully");
//
//}
//std::string DataFilesManager::getDataFilename(const std::string filename) {
//
//	UNUSED_ARG(filename);
//	return "data_files\\data1"; //For now. Make it dynamic later
//}
DataFilesManager::DataFilesManager() {
	openAllDataFiles(dataFilesDir_m);
}
void DataFilesManager::appendDataBlock(const Data& dataBlock) {
	
	//check if the current data file is opened or max data file size limit is reached
	if (!currentOutputDataFile_strm.is_open() || currentOutputDataFile_strm.tellp() >= getDataFileSizeLimit()) {
		//close the current file and create new one
		currentOutputDataFile_strm.close();
		auto newFilename = "" + std::atoi(currentDatafileOutputFilename_m.c_str()) + 1;
		currentOutputDataFile_strm.open(newFilename, std::ios_base::binary);
		if (!currentOutputDataFile_strm.is_open()) {
			throw FileIOException(STR("Failed to open/create data file " << newFilename));
		}
		dataFilenameMap.emplace(newFilename, currentOutputDataFile_strm);
	}
	//write raw
	currentOutputDataFile_strm.write(dataBlock.buff, dataBlock.length);
	if (currentOutputDataFile_strm.bad()) {
		throw FileIOException(STR("Failed to write data block to data file" << currentDatafileOutputFilename_m));
	}
	logmsg(INFO, "Updated data file for record offset " << dataBlock.offset
		<< " length " << dataBlock.length);
	//outputDataFileStrm_m.flush();
}

DataFilesManager::~DataFilesManager() {
	if (currentOutputDataFile_strm.is_open()) {
		currentOutputDataFile_strm.close();
	}
}

//DataFilesManager::DataFilesManager(const std::string filename, std::ios_base::openmode fileOpenMode): filename_m(filename) {
//	if (filename.empty()) {
//		throw std::invalid_argument("Empty filename received");
//	}
//	
//	auto dataFileName = getDataFilename(filename);
//
//	if (fileOpenMode == std::ios_base::in) {
//		//open file in read mode
//		dataFileInputStrm_m.open(dataFileName, fileOpenMode|std::ios_base::binary);
//		if (!dataFileInputStrm_m.is_open()) {
//			logmsg(ERROR, "Failed to open data file " << filename << " for reading");
//			throw FileIOException(STR("Failed to open data file " << filename << " for reading"));
//		}
//		logmsg(DEBUG, "Data file " << filename << " opened succesfully for reading");
//	}
//
//	else if (fileOpenMode == std::ios_base::out || fileOpenMode == std::ios_base::app) {
//		outputDataFileStrm_m.open(dataFileName, fileOpenMode|std::ios_base::binary);
//		if (!outputDataFileStrm_m.is_open()) {
//			logmsg(ERROR, "Failed to open data file " << filename << " for reading");
//			throw FileIOException(STR("Failed to open data file " << filename << " for reading"));
//		}
//		logmsg(DEBUG, "Data file " << filename << " opened succesfully for writting");
//	}
//}

/// <summary>
///	THis function is expected to receive dedupe block info with data filename, offset and length to read from the opened file. This function will return actual data in 'buff'
/// This function throws FileIOException in case of failures
/// </summary>
/// <param name="dataBlock"></param>
void DataFilesManager::readDataBlock(const DedupedDataInfo& dedupeDataBlockInfo,  const Data& dataBlock) {
	if (dataBlock.length == 0) {
		return; //nothing to do
	}
	auto filename = dedupeDataBlockInfo.dataFileName;
	//find input file stream in opened data files map.
	if (dataFilenameMap.find(filename) != dataFilenameMap.end()) {
		//file mapping is present. Operate on the input file stream
		if (!dataFilenameMap[filename].is_open()) { //operate on the value
			throw FileIOException("Input data file is not open");
		}
		//seek to the right position
		dataFilenameMap[filename].seekg(dataBlock.offset, std::ios_base::beg);
		//read actual data
		dataFilenameMap[filename].read(dataBlock.buff, dataBlock.length);
		//when we're reading at specific offset and length, we're expected to read full length else its an error
		if (dataFilenameMap[filename].gcount() < dataBlock.length) {
			logmsg(DEBUG, "Data file reading failed. Less data has been read than expected.");
			throw FileIOException(STR("Failed to read the data file " << filename));
		}
		if (dataFilenameMap[filename].eof()) {
			logmsg(DEBUG, "Data file " << filename << " EOF has been reached");
		}
		if (dataFilenameMap[filename].bad()) {
			logmsg(ERROR, "There was an error while reading the data file " << filename);
		}
	}
	else {
		auto errMsg("Invalid data block received. no data file associated with it");
		logmsg(ERROR, errMsg);
		throw DedupeProcessingException(errMsg);
	}
	
	
}

void DataFilesManager::openAllDataFiles(const std::string& dataFileDir) {
	if (dataFileDir.empty()) {
		throw std::invalid_argument("Empty data file dir name received");
	}
	logmsg(DEBUG, "Opening data file dir " << dataFileDir);
	std::filesystem::path dirPath{ dataFileDir };
	std::string lastOpenedFilename;
	for (std::filesystem::directory_entry dataFileEntry : std::filesystem::directory_iterator(dirPath)) {
		if (dataFileEntry.is_regular_file()) {
			//insert into out filename & file if stream map
			lastOpenedFilename = dataFileEntry.path().stem().string();
			dataFilenameMap[lastOpenedFilename] = std::ifstream{ dataFileEntry.path(), std::ios_base::binary }; //open file at run time and put into the map
			logmsg(DEBUG, "Opened and inserted " << dataFileEntry);
		}
	}
	currentDatafileOutputFilename_m = lastOpenedFilename;
	logmsg(DEBUG, "Done " << __func__);

}

size_t DataFilesManager::getDataFileSizeLimit()
{
	return DATA_FILE_SIZE_LIMIT;
}
