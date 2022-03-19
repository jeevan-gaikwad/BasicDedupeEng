#include "DedupeEngine.h"
#include "FileInterfaces.h"
#include <stdexcept>
#include <openssl/sha.h>
#include <cstdio>
#include <filesystem> //new C++17
#include <fstream>
#include "IndexFileManager.h"
#include "Logging.h"
#include "DataFilesManager.h"

/*
	Following function works with one index and one data file for each input file.
	Update it as and when we want to split them into multiple files
*/
status_t DedupeEngine::dedupeFile(const std::string& filename) {
	if (filename.empty()) {
		throw std::invalid_argument("Empty file name received to dedupe file func");
	}
	logmsg(INFO, "Working with file:" << filename);
	//1. check do we already have index file created for this file
	try {
		IndexFileManager indFlMgr(filename, std::ios_base::app);
		logmsg(DEBUG, "Created index file for file " << filename);

		FileReader fileReader(filename); //read input file
		Data dataBuff(BUFF_SIZE); //temporary buffer to read and write block of data
		DedupedDataInfo dedupeMetaData;
		while (!fileReader.isEof()) {
			fileReader.readNextDataBuff(dataBuff);
			if (calculateFingerprint(dataBuff, dedupeMetaData) == SUCCESS) {
				std::cout << "SHA1 fingerprint is:" << dedupeMetaData.fingerprint << std::endl;
				if (map.find(dedupeMetaData.fingerprint) != map.end()) {
					std::cout << "This block is already present" << std::endl;
				}
				else {
					//update index file and data file for the same & update dudupe meta data with data file offset
					dataFilesManager_m.appendDataBlock(dataBuff, dedupeMetaData);
					map.insert({ dedupeMetaData.fingerprint, dedupeMetaData }); //insert a pair
				}
				//always update the index file to track block info for the file
				indFlMgr.appendFileMetaData(dedupeMetaData);
			}
			else {
				logmsg(ERROR, "Fingerprint calculation failed.");
				return FINGERPRINT_CALC_FAILED;
			}
		}
	}
	catch (const  CommonException& e) {
		logmsg(ERROR, "Execption occured while processing the file" << (e.what()));
		return UNKNOWN_ERROR; //need to change
	}
	catch (const FileIOException& e) {
		logmsg(ERROR, "FileIO exception occurrred. " << e.getErrorCode());
		return UNKNOWN_ERROR;
	}

	logmsg(INFO, "Done with file:" << filename);

	return SUCCESS;
}

status_t DedupeEngine::calculateFingerprint(const Data& dataBuff, DedupedDataInfo& dedupeDataBuff) {
	unsigned char sha1Md[SHA_DIGEST_LENGTH];
	SHA1((unsigned char*)dataBuff.buff, dataBuff.length, sha1Md);
	dedupeDataBuff.offset = dataBuff.offset;
	dedupeDataBuff.length = dataBuff.length;
	dedupeDataBuff.fingerprint.clear();
	/*
	* Another way to do it. Which SHA1 does internally.
	int ret = SHA1_Init(&ctx);
	if (ret == 0) {
		std::cerr << "Failed to initialize SHA1 ctx" << std::endl;
		return FINGERPRINT_CALC_FAILED;
	}
	ret = SHA1_Update(&ctx,dataBuff.buff, dataBuff.size);
	if (ret == 0) {
		std::cerr << "Failed to update data buff dor SHA1" << std::endl;
		return FINGERPRINT_CALC_FAILED;
	}
	ret = SHA1_Final(sha1Md, &ctx);
	if (ret == 0) {
		std::cerr << "Failed to calculate SHA1 fingerprint" << std::endl;
		return FINGERPRINT_CALC_FAILED;
	}
	*/
	char temp[5];
	for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
		sprintf(temp, "%02x", sha1Md[i]); //convert to Hex representation
		dedupeDataBuff.fingerprint += temp;
	}
	logmsg(DEBUG, "Calculated checksum:" << dedupeDataBuff.fingerprint);
	return SUCCESS;
}
status_t DedupeEngine::createFileFromEngine(const std::string& filename) {

	/* 1. First we need to search for index file(meta data file)*/
	if (filename.empty()) {
		logmsg(ERROR, "Empty filename received for function " << __func__);
		return INVALID_ARG;
	}
	auto indexFilename = IndexFileManager::getIndexFilename(filename);
	if (!std::filesystem::exists(indexFilename)) {
		return NO_DEDUPE_INFO_FOUND;
	}
	try {

		/* 2. Now we need to read index file records and build the actual file*/
		IndexFileManager indFlMgr(filename, std::ios_base::in);
		//DataFileManager dataFlMgr(filename, std::ios_base::in); //open file for reading
		DedupedDataInfo dedupeDataBlockInfo;

		std::ofstream outputFile(filename + outputFileSuffix_m, std::ios_base::binary);

		/* 3. Now read the data block by block from the data file at offset and length*/
		Data dataBlock;
		while (indFlMgr.readNextIndexFileRecord(dedupeDataBlockInfo) != FILE_EOF_REACHED) {
			dataFilesManager_m.readDataBlock(dedupeDataBlockInfo, dataBlock);
			//now append this data block to output file at the right offset
			outputFile.seekp(dedupeDataBlockInfo.offset, std::ofstream::beg);
			outputFile.write(dataBlock.buff, dataBlock.length);
		}
		outputFile.close();
	}
	catch (CommonException& e) {
		logmsg(ERROR, "CommonException has occured while creating file from engine" << e.what());
		return CREATE_FILE_FROM_ENG_FAILED;
	}
	catch (FileIOException& e) {
		logmsg(ERROR, "FileIOException has occured while creating file from engine" << e.what());
		return CREATE_FILE_FROM_ENG_FAILED;
	}
	return SUCCESS;
}

//this function is expected to check or create if not exists the directory with filename + dedupe_file_suffix
std::string DedupeEngine::createFileMetaDirIfNotExists(const std::string& filename) {
	auto fileMetaDirName = filename + dedupe_file_suffix;
	std::filesystem::create_directory(std::filesystem::path(fileMetaDirName));
	return fileMetaDirName;
}


