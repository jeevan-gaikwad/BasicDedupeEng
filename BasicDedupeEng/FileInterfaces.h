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

	const char* what() const {
		return execeptionMsg.c_str();
	}
	int getErrorCode() const { return errorCode; };
};

class FileReader {

public:
	FileReader(std::string filename);
	
	void readNextDataBuff(Data& dataBuff);
	
	bool isEof();
	
	~FileReader();

private:
	std::ifstream fileStream_m;
	std::string filename_m;
};

