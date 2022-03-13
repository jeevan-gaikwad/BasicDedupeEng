#include "Common.h"
#include <ctime>
#include <iomanip>
#include <time.h>
#include <new> // for std::nothrow


std::string getCurrentTimeAsString() {
	struct tm timeInfo;
	time_t currTime = time(nullptr);
	localtime_s(&timeInfo, &currTime);
	return (std::ostringstream() << std::put_time(&timeInfo, "%d-%m-%Y %H:%M:%S")).str();
}

Data::Data(size_t offset, long long buffLength):offset(offset), length(buffLength) {
	allocateBuffer(buffLength);
}
Data::Data(long long buffLength) {
	allocateBuffer(buffLength);
}

Data::Data() {
	allocateBuffer(BUFF_SIZE);
}

Data::~Data() {
	if (buff != nullptr) {
		delete buff;
	}
}

void Data::allocateBuffer(long long buflen) {
	if (buflen > 0) {
		buff = new(std::nothrow) char[buflen];
		if (buff == nullptr) {
			throw DedupeProcessingException("Failed to allocate memory. System does not have sufficient memory");
		}
		length = buflen;
	}
}