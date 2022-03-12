#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <stdexcept>
#include <filesystem>
#include <exception>
#include <stdexcept>

#include "Common.h"
#include "Logging.h"

using fileMode_t = std::ios_base::openmode;

class FileIOException //: public CommonException
{
	//CUrrently we dont anything special for FileIOException. CommonException is good enough
	//new C++ 11
	//using CommonException::CommonException; //Somehow this is not working. need to check. Error. reference to a deleted function.
private:
	std::string execeptionMsg;
	int errorCode;

public:
	FileIOException(const std::ostringstream cause) {
		execeptionMsg = cause.str();
		errorCode = 0;
	}
	FileIOException(const std::string cause) {
		execeptionMsg = cause;
		errorCode = 0;
	}

	FileIOException(const int errCode, const std::ostringstream cause) {
		execeptionMsg = cause.str();
		errorCode = errCode;
	}

	FileIOException(const int errCode, std::string cause) {
		execeptionMsg = cause;
		errorCode = errCode;
	}

	const char* what() {
		return execeptionMsg.c_str();
	}
	int getErrorCode() const { return errorCode; };
};

class FileReader {

public:
	FileReader(std::string filename) {
		if (filename.empty()) {
			throw std::invalid_argument("Empty filename received");
		}
		fileStream_m.open(filename.c_str(), std::ios::binary);
		if (fileStream_m.is_open()) {
			logmsg(DEBUG, "File" << filename << " opened successfully!");
			filename_m = filename;
		}
		else {
			strStrm_t errMsg = STR("Failed to open input file" << filename <<" for deduplication.");
			logmsg(ERROR, errMsg.str());
			throw FileIOException(errMsg.str());
		}
	}
	
	void readNextDataBuff(Data& dataBuff) {
		if (!fileStream_m.eof()) {
			auto currOffset = fileStream_m.tellg();
			fileStream_m.read(dataBuff.buff, dataBuff.length);
			if (fileStream_m.gcount() < dataBuff.length) {
				logmsg(ERROR, "read less than " << dataBuff.length << " from file" << filename_m);
			}

			dataBuff.length = fileStream_m.gcount();
			dataBuff.offset = currOffset;

			//if (fileStream_m.good()) {
			//	if (fileStream_m.gcount() < dataBuff.length) {
			//		logmsg(ERROR, "read less than " << dataBuff.length <<" from file" << filename_m);
			//		dataBuff.length = fileStream_m.gcount();
			//	}
			//	else {
			//		logmsg(DEBUG, "Read success at offset " << fileStream_m.tellg() << " of length " << BUFF_SIZE);
			//		dataBuff.offset = currOffset;
			//	}
			//}
			//else {
			//	strStrm_t errMsg = STR("Failed to read data block from the input file " << filename_m);
			//	logmsg(ERROR, errMsg.str());
			//	throw FileIOException(errMsg.str());
			//}
		}
		else {
			logmsg(DEBUG, "EOF occurred for file " << filename_m);
		}
	}
	bool isEof() {
		return fileStream_m.eof();
	}
	~FileReader() {
		if (fileStream_m.is_open()) {
			fileStream_m.close();
		}
	}

private:
	std::ifstream fileStream_m;
	std::string filename_m;
};

