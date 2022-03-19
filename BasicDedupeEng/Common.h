#ifndef _COMMON_H
#define _COMMON_H

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <exception>
#include <sstream>

using status_t = short;
using strStrm_t = std::ostringstream;
const int BUFF_SIZE = 64 * 1024; // 64K

#define STR(x) std::ostringstream()<<x

#define print(x) std::cout<<x<<std::endl;

#define UNUSED_ARG(x) (void)x;

struct Data {
	char* buff;
	std::string fingerprint;
	long long  length; //length of the actual data.
	size_t offset; //offset in file

	Data();
	Data(long long buffLength);
	Data(size_t offset, long long buffLength);
	void allocateBuffer(long long buflen);
	~Data();
};

enum {
	SUCCESS = 0,
	UNKNOWN_ERROR,
	FILE_OPEN_ERR,
	FINGERPRINT_CALC_FAILED,
	FILE_ALREADY_EXISTS,
	FILE_CREATE_FAILED,
	FILE_READ_ERROR,
	FILE_WRITE_ERROR,
	INVALID_ARG,
	NO_DEDUPE_INFO_FOUND,
	FILE_EOF_REACHED,
	CREATE_FILE_FROM_ENG_FAILED,
};

using status_t = short;

struct DedupedDataInfo {
	std::string fingerprint;
	size_t offset;
	long long length;
	std::string dataFileName;
	size_t dataFileOffset;
};

class DedupeProcessingException: public std::exception
{
private:
	std::string execeptionMsg;
	int errorCode;

public:
	DedupeProcessingException(const std::string cause) {
		execeptionMsg = cause;
		errorCode = 0;
	}
	DedupeProcessingException(const std::ostringstream cause) {
		execeptionMsg = cause.str();
		errorCode = 0;
	}
	DedupeProcessingException(const int errCode, const std::string cause) {
		execeptionMsg = cause;
		errorCode = errCode;
	}
	DedupeProcessingException(const int errCode, const std::ostringstream cause) {
		execeptionMsg = cause.str();
		errorCode = errCode;
	}
	const char* what() {
		return execeptionMsg.c_str();
	}
	int getErrorCode() const { return errorCode; };
};

class CommonException : public std::exception
{
private:
	std::string execeptionMsg;
	int errorCode;

public:
	CommonException(const std::ostringstream cause) {
		execeptionMsg = cause.str();
		errorCode = 0;
	}
	CommonException(const std::string cause) {
		execeptionMsg = cause;
		errorCode = 0;
	}

	CommonException(const int errCode, const std::ostringstream cause) {
		execeptionMsg = cause.str();
		errorCode = errCode;
	}

	CommonException(const int errCode, std::string cause) {
		execeptionMsg = cause;
		errorCode = errCode;
	}

	const char* what() const {
		return execeptionMsg.c_str();
	}
	int getErrorCode() const { return errorCode; };
};

std::string getCurrentTimeAsString();

#endif