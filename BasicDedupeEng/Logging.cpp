#include "Logging.h"

Logger gLog; //global def




Logger::~Logger() {
	if (logFile.is_open()) {
		logFile.close();
	}
}

const std::string getLogLevelName(logLevel lLevel) {
	switch (lLevel) {
	case INFO:
		return "INFO";
	case DEBUG:
		return "DEBUG";
	case ERROR:
		return "ERROR";
	default:
		return "DEFAULT";
	}
}

void Logger::log(logLevel lLevel, std::ostringstream str) {
	if (currLogLevel >= lLevel || lLevel == ERROR) {
		*outputStream << getCurrentTimeAsString() << ": [" << getLogLevelName(lLevel) << "] " << str.str() <<std::endl;
		outputStream->flush();
	}
	else {
		std::cout << "CUrrent log level is higher" << std::endl;
	}
}


