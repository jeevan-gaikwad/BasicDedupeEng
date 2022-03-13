#include "Common.h"
#include "DataFileManager.h"
#include "Logging.h"
#include "FileInterfaces.h"

void DataFilesManager::createOrOpenDataFile(const std::string& filename) {

	if (filename.empty()) {
		logmsg(ERROR, "Empty filename received to function " << __func__);
		throw std::invalid_argument("Empty filename received");
	}
	(void)filename; //unused for now. Creating single data file

	std::string dataFilename = getDataFilename(filename);
	outputDataFileStrm_m.open(dataFilename, std::ofstream::binary); //change this mode to append later
	if (!outputDataFileStrm_m.is_open()) {
		logmsg(ERROR, "Failed to open output data file " << dataFilename);
	}
	filename_m = dataFilename;
	logmsg(DEBUG, "Output data file " << dataFilename << " opened successfully");

}
std::string DataFilesManager::getDataFilename(const std::string filename) {

	UNUSED_ARG(filename);
	return "data_files\\data1"; //For now. Make it dynamic later
}
void DataFilesManager::appendDataBlock(const Data& dataBlock) {
	if (!outputDataFileStrm_m.is_open()) {
		throw FileIOException(STR("File" << filename_m << " is not open/created"));
	}
	//write raw
	outputDataFileStrm_m.write(dataBlock.buff, dataBlock.length);
	if (outputDataFileStrm_m.bad()) {
		throw FileIOException(STR("Failed to write data block to data file" << filename_m));
	}
	logmsg(INFO, "Updated data file for record offset " << dataBlock.offset
		<< " length " << dataBlock.length);
	//outputDataFileStrm_m.flush();
}

DataFilesManager::~DataFilesManager() {
	if (outputDataFileStrm_m.is_open()) {
		outputDataFileStrm_m.close();
	}
}

DataFilesManager::DataFilesManager(const std::string filename, std::ios_base::openmode fileOpenMode): filename_m(filename) {
	if (filename.empty()) {
		throw std::invalid_argument("Empty filename received");
	}
	
	auto dataFileName = getDataFilename(filename);

	if (fileOpenMode == std::ios_base::in) {
		//open file in read mode
		dataFileInputStrm_m.open(dataFileName, fileOpenMode|std::ios_base::binary);
		if (!dataFileInputStrm_m.is_open()) {
			logmsg(ERROR, "Failed to open data file " << filename << " for reading");
			throw FileIOException(STR("Failed to open data file " << filename << " for reading"));
		}
		logmsg(DEBUG, "Data file " << filename << " opened succesfully for reading");
	}

	else if (fileOpenMode == std::ios_base::out || fileOpenMode == std::ios_base::app) {
		outputDataFileStrm_m.open(dataFileName, fileOpenMode|std::ios_base::binary);
		if (!outputDataFileStrm_m.is_open()) {
			logmsg(ERROR, "Failed to open data file " << filename << " for reading");
			throw FileIOException(STR("Failed to open data file " << filename << " for reading"));
		}
		logmsg(DEBUG, "Data file " << filename << " opened succesfully for writting");
	}
}
/// <summary>
///	THis function is expected to receive offset and length to read from the open file. This function will return actual data in 'buff'
/// This function throws FileIOException in case of failures
/// </summary>
/// <param name="dataBlock"></param>
void DataFilesManager::readDataBlock(DedupedDataInfo dedupeDataBlockInfo, 
	const Data& dataBlock) {
	if (dataBlock.length == 0) {
		return; //nothing to do
	}
	if (!dataFileInputStrm_m.is_open()) {
		throw FileIOException("Input data file is not open");
	}
	//seek to the right position
	dataFileInputStrm_m.seekg(dataBlock.offset, std::ios_base::beg);
	//read actual data
	dataFileInputStrm_m.read(dataBlock.buff, dataBlock.length);
	//when we're reading at specific offset and length, we're expected to read full length else its an error
	if (dataFileInputStrm_m.gcount() < dataBlock.length ) {
		logmsg(DEBUG, "Data file reading failed. Less data has been read than expected.");
		throw FileIOException(STR("Failed to read the data file " << filename_m));
	}
	if (dataFileInputStrm_m.eof()) {
		logmsg(DEBUG, "Data file " << filename_m << " EOF has been reached");
	}
	if (dataFileInputStrm_m.bad()) {
		logmsg(ERROR, "There was an error while reading the data file " << filename_m);
	}
}