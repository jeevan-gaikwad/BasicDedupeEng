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

void DataFilesManager::appendDataBlock(const Data& dataBlock) {
	if (!currentOutputDataFile_strm.is_open()) {
		throw FileIOException(STR("File" << currentDatafileOutputFilename_m << " is not open/created"));
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
	for (std::filesystem::directory_entry dataFileEntry : std::filesystem::directory_iterator(dirPath)) {
		if (dataFileEntry.is_regular_file()) {
			//insert into out filename & file if stream map
			dataFilenameMap[dataFileEntry.path().stem().string()] = std::ifstream{ dataFileEntry.path(), std::ios_base::binary }; //open file at run time and put into the map
			logmsg(DEBUG, "Opened and inserted " << dataFileEntry);
		}
	}
	logmsg(DEBUG, "Done " << __func__);

}
