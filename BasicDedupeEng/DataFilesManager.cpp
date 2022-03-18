#include "Common.h"
#include "DataFilesManager.h"
#include "Logging.h"
#include "FileInterfaces.h"

DataFilesManager::DataFilesManager() {
	openAllDataFiles(dataFilesDir_m);
}

/* This 'generateNextFilename' function strictly generates integer based increasing number
*/
std::string DataFilesManager::generateNextFilename(const std::string& currentFilename) {
	if (currentFilename.empty()) {
		//no previous file exists. create file with name 1
		return std::string(FIRST_FILE_NAME);
	}
	else {
		long currFileNum = std::stoi(currentFilename);
		++currFileNum;
		return (std::ostringstream() << currFileNum).str();
	}
}
std::string DataFilesManager::appendDataBlock(const Data& dataBlock, DedupedDataInfo& dedupeInfoOut) {
	
	//check if the current data file is opened or max data file size limit is reached
	if (!currentOutputDataFile_strm.is_open() || (long)currentOutputDataFile_strm.tellp() >= getDataFileSizeLimit()) {

		//close the current file and create new one
		currentOutputDataFile_strm.close();

		std::string newFilename = generateNextFilename(currentDatafileOutputFilename_m);
		std::string newFilePath = dataFilesDir_m + "\\" + newFilename;
		currentOutputDataFile_strm.open(newFilePath, std::ios_base::binary);
		if (!currentOutputDataFile_strm.is_open()) {
			throw FileIOException(STR("Failed to open/create data file " << newFilename));
		}
		currentDatafileOutputFilename_m = newFilename;
		//make entry in the map so that immediate reads can be served
		dataFilenameMap_m.emplace(newFilename, std::ifstream(newFilename, std::ios_base::binary));
	}
	
	//logmsg(DEBUG, "Continue to write to existing file " << currentDatafileOutputFilename_m);

	//write raw
	dedupeInfoOut.dataFileOffset = currentOutputDataFile_strm.tellp();
	currentOutputDataFile_strm.write(dataBlock.buff, dataBlock.length);
	if (currentOutputDataFile_strm.bad()) {
		throw FileIOException(STR("Failed to write data block to data file" << currentDatafileOutputFilename_m));
	}
	dedupeInfoOut.dataFileName = currentDatafileOutputFilename_m;
	/*logmsg(INFO, "Updated data file for record offset " << dataBlock.offset
		<< " length " << dataBlock.length);*/
	return "sample";
}

DataFilesManager::~DataFilesManager() {
	if (currentOutputDataFile_strm.is_open()) {
		currentOutputDataFile_strm.close();
	}
}

/// <summary>
///	THis function is expected to receive dedupe block info with data filename, offset and length to read from the opened file. This function will return actual data in 'buff'
/// This function throws FileIOException in case of failures
/// </summary>
/// <param name="dataBlock"></param>
void DataFilesManager::readDataBlock(const DedupedDataInfo& dedupeDataBlockInfo,  Data& dataBlockOut) {
	if (dataBlockOut.length == 0) {
		return; //nothing to do
	}
	auto filename = dedupeDataBlockInfo.dataFileName;
	//find input file stream in opened data files map.
	if (dataFilenameMap_m.find(filename) != dataFilenameMap_m.end()) {
		//file mapping is present. Operate on the input file stream
		if (!dataFilenameMap_m[filename].is_open()) { //operate on the value
			throw FileIOException("Input data file is not open");
		}
		//seek to the right position provided in dedupe block info
		dataFilenameMap_m[filename].seekg(dedupeDataBlockInfo.dataFileOffset, std::ios_base::beg);
		//read actual data
		dataFilenameMap_m[filename].read(dataBlockOut.buff, dedupeDataBlockInfo.length);
		//when we're reading at specific offset and length, we're expected to read full length else its an error
		if (dataFilenameMap_m[filename].gcount() < dedupeDataBlockInfo.length) {
			logmsg(DEBUG, "Data file reading failed. Less data has been read than expected.");
			throw FileIOException(STR("Failed to read the data file " << filename));
		}
		else {
			logmsg(DEBUG, "Data file " << filename << " read successfull at offset "<< 
				dedupeDataBlockInfo.offset << " & length " << dedupeDataBlockInfo.length);
			dataBlockOut.offset = dedupeDataBlockInfo.offset;
			dataBlockOut.length = dedupeDataBlockInfo.length;
		}

		if (dataFilenameMap_m[filename].eof()) {
			logmsg(DEBUG, "Data file " << filename << " EOF has been reached");
		}
		if (dataFilenameMap_m[filename].bad()) {
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
	std::string latestFilename;
	for (std::filesystem::directory_entry dataFileEntry : std::filesystem::directory_iterator(dirPath)) {
		if (dataFileEntry.is_regular_file()) {
			//insert into out filename & file if stream map
			latestFilename = dataFileEntry.path().stem().string();
			dataFilenameMap_m[latestFilename] = std::ifstream{ dataFileDir + "\\" + latestFilename, std::ios_base::binary}; //open file at run time and put into the map
			logmsg(DEBUG, "Opened and inserted " << dataFileEntry);
		}
	}
	//now open latest data file to continue writing
	if (!latestFilename.empty()) {
		currentDatafileOutputFilename_m = latestFilename;
		currentOutputDataFile_strm.open(dataFileDir + "\\" + latestFilename, std::ios_base::app| std::ios_base::binary);
		if (!currentOutputDataFile_strm.is_open()) {
			logmsg(ERROR, "Failed to open data file " << latestFilename);
			throw FileIOException(STR("Failed to open data file " << latestFilename));
		}
	}
	else {
		logmsg(DEBUG, "No previous data files are present. Will be created when we write actual data");
	}
	logmsg(DEBUG, "Done " << __func__);

}


size_t DataFilesManager::getDataFileSizeLimit()
{
	return DATA_FILE_SIZE_LIMIT;
}
