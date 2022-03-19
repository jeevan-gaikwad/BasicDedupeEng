#include "FileInterfaces.h"

FileReader::FileReader(std::string filename) {
	if (filename.empty()) {
		throw std::invalid_argument("Empty filename received");
	}
	fileStream_m.open(filename.c_str(), std::ios::binary);
	if (fileStream_m.is_open()) {
		logmsg(DEBUG, "File" << filename << " opened successfully!");
		filename_m = filename;
	}
	else {
		strStrm_t errMsg = STR("Failed to open input file" << filename << " for deduplication.");
		logmsg(ERROR, errMsg.str());
		throw FileIOException(errMsg.str());
	}
}


void FileReader::readNextDataBuff(Data& dataBuff) {
	if (!fileStream_m.eof()) {
		auto currOffset = fileStream_m.tellg();
		fileStream_m.read(dataBuff.buff, dataBuff.length);
		if (fileStream_m.gcount() < dataBuff.length) {
			logmsg(ERROR, "read less than " << dataBuff.length << " from file" << filename_m);
		}

		dataBuff.length = fileStream_m.gcount();
		dataBuff.offset = currOffset;

	}
	else {
		logmsg(DEBUG, "EOF occurred for file " << filename_m);
	}
}

bool FileReader::isEof(){
	return fileStream_m.eof();
}

FileReader::~FileReader() {
	if (fileStream_m.is_open()) {
		fileStream_m.close();
	}
}