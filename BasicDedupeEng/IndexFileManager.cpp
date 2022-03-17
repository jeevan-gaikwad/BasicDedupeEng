#include "IndexFileManager.h"
#include <cassert>

void IndexFileManager::createOrOpenIndexFile(const std::string& filename)
{
	if (filename.empty()) {
		throw std::invalid_argument("Empty file dir received");
	}

	auto indFilename = getIndexFilename(filename);
	indexFileOutStrm_m.open(indFilename, std::ofstream::out); //for now always create new index file , std::ofstream::app);
	if (!indexFileOutStrm_m.is_open()) {
		throw FileIOException("Failed to open/create index file");
	}
	indFilename_m = indFilename;

}

IndexFileManager::IndexFileManager(const std::string filename, std::ios_base::openmode fileOpenMode)
{
	if (filename.empty()) {
		throw std::invalid_argument("Empty filename received");
	}

	auto indexFileName = getIndexFilename(filename);

	if (fileOpenMode == std::ios_base::in) {
		//open file in read mode
		indexFileReadStrm_m.open(indexFileName, std::ios_base::in);
		if (!indexFileReadStrm_m.is_open()) {
			logmsg(ERROR, "Failed to open index file " << indexFileName << " for reading");
			throw FileIOException(STR("Failed to open index file " << indexFileName << " for reading"));
		}
		logmsg(DEBUG, "Index file " << indexFileName << " opened succesfully for reading");
	}

	else if (fileOpenMode == std::ios_base::out || fileOpenMode == std::ios_base::app) {

		indexFileOutStrm_m.open(indexFileName, fileOpenMode);// | std::ios_base::binary);
		if (!indexFileOutStrm_m.is_open()) {
			logmsg(ERROR, "Failed to open index file " << indexFileName << " for reading");
			throw FileIOException(STR("Failed to open index file " << indexFileName << " for reading"));
		}
		logmsg(DEBUG, "Index file " << indexFileName << " opened succesfully for writting");
	}
	indFilename_m = indexFileName;
}

IndexFileManager::~IndexFileManager()
{
	if (indexFileOutStrm_m.is_open()) {
		indexFileOutStrm_m.close();
	}
	if (indexFileReadStrm_m.is_open()) {
		indexFileReadStrm_m.close();
	}
}

void IndexFileManager::appendFileMetaData(const DedupedDataInfo& dedupeDataInfo) {
	if (!indexFileOutStrm_m.is_open()) {
		throw FileIOException(STR("File" << indFilename_m << " is not open/created"));
	}
	//write raw. std::endl is a record separator here
	indexFileOutStrm_m << dedupeDataInfo.fingerprint<<std::endl << dedupeDataInfo.offset << std::endl << dedupeDataInfo.length << std::endl <<
		dedupeDataInfo.dataFileName <<std::endl << dedupeDataInfo.dataFileOffset<< std::endl;
	
	if (indexFileOutStrm_m.bad()) {
		throw FileIOException(STR("Failed to write index record to file"<< indFilename_m));
	}
	logmsg(INFO, "Updated index file for record " << dedupeDataInfo.fingerprint << " offset " << dedupeDataInfo.offset
		<< " length " << dedupeDataInfo.length << " datafile:" << dedupeDataInfo.dataFileName);
}

std::string IndexFileManager::getIndexFilename(const std::string& filename) {
	if (filename.empty()) {
		throw std::invalid_argument("Empty filename received");
	}
	return indFilenamePrefix_m + filename + indFilenameSuffix_m;
}

status_t IndexFileManager::readNextIndexFileRecord(DedupedDataInfo& dedupeDataInfo) {
	DedupedDataInfo localDedupeInfo;
	if (!indexFileReadStrm_m.is_open()) {
		indexFileReadStrm_m.open(indFilename_m, std::ifstream::binary);
		if (!indexFileReadStrm_m.is_open()) {
			throw FileIOException(STR("Failed to open index file" << getIndexFilename(indFilename_m)));
		}
		logmsg(INFO, "Successfully opened dedupe info index file " << getIndexFilename(indFilename_m));
	}
	dedupeDataInfo.fingerprint.clear();
	indexFileReadStrm_m >> dedupeDataInfo.fingerprint >> dedupeDataInfo.offset >> dedupeDataInfo.length 
		>> dedupeDataInfo.dataFileName >> dedupeDataInfo.dataFileOffset;

	//indexFileReadStrm_m.read((char*)&localDedupeInfo, sizeof(DedupedDataInfo));
	if (indexFileReadStrm_m.bad() && indexFileReadStrm_m.gcount() < sizeof(DedupedDataInfo)) {
		throw FileIOException("Failed to read DedupeDataInfo record from the index file");
	}

	//assert(indexFileReadStrm_m.gcount() == sizeof(DedupedDataInfo));

	logmsg(DEBUG, "Succesfully read index file record- " << dedupeDataInfo.fingerprint << " " << dedupeDataInfo.
		dataFileName << " offset:" << dedupeDataInfo.offset << " len:" << dedupeDataInfo.length);
	if (indexFileReadStrm_m.eof()) {
		return FILE_EOF_REACHED;
	}
	else {
		return SUCCESS;
	}
}
