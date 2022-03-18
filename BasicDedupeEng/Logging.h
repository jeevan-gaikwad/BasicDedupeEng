#pragma once

#ifndef _LOGGING_H
#define _LOGGING_H

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include "Common.h"

class Logger;

const std::string logFilename = "dedupeEng_logs.txt";
extern Logger gLog;

enum logLevel {
	INFO,
	DEBUG,
	ERROR
};

const std::string getLogLevelName(logLevel lLevel);

class Logger {
public:
	Logger() {
		currLogLevel = logLevel::INFO;
		logFile.open(logFilename, std::ofstream::app);
		if (!logFile.is_open()) {
			//throw CommonException(STR("Failed to open" << logFilename));
			std::cerr<<"Failed to open log file. Writing logs to stdout";
			outputStream = &std::cout;
		}
		else {
			print("log file" << logFilename << " is created");
			logFile.rdbuf()->pubsetbuf(0, 0); // Disable buffering to immediately flush log statements to the file
			outputStream = &logFile;
		}
	}
	Logger(const logLevel level) {
		this->currLogLevel = level;

	}
	template<typename T>
	Logger& operator<<(const T& str);
	/*template<typename T>
	void log(logLevel log, const T& str);*/
	void log(logLevel lLevel, std::ostringstream str);
	~Logger();
private:
	logLevel currLogLevel;
	std::ostream* outputStream  = nullptr;
	std::ofstream logFile;
};

template<typename T>
inline Logger& Logger::operator <<(const T& str) {
	(*outputStream) << str;
	return *this;
}

/*
template<typename T>
inline void Logger::log(logLevel lLevel, const T& str)
{
	if (currLogLevel >= lLevel || lLevel == ERROR) {
		*this << getCurrentTimeAsString() << ": [" << getLogLevelName(lLevel) << "] " << str;
		*outputStream << std::endl;
	}
	else {
		std::cout << "CUrrent log level is higher" << std::endl;
	}
}
*/


#define logmsg(x,y) gLog.log(logLevel::x,STR(y))

#endif