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

/* following logmsg macro should be used to log the messages */
#define logmsg(x,y) gLog.log(logLevel::x,STR(y))

enum logLevel {
	INFO,
	DEBUG,
	ERROR
};

const std::string getLogLevelName(logLevel lLevel);

class Logger {
public:
	Logger();
	
	Logger(const logLevel level) : currLogLevel(level) { }
	void log(logLevel lLevel, std::ostringstream str);
	~Logger();
private:
	logLevel currLogLevel;
	std::ostream* outputStream  = nullptr;
	std::ofstream logFile;
};

#endif