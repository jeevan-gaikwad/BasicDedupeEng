#include "Logging.h"

Logger gLog; //global def

Logger::Logger() {
	currLogLevel = logLevel::INFO;
	logFile.open(logFilename, std::ofstream::app);
	if (!logFile.is_open()) {
		//throw CommonException(STR("Failed to open" << logFilename));
		std::cerr << "Failed to open log file. Writing logs to stdout";
		outputStream = &std::cout;
	}
	else {
		print("log file" << logFilename << " is created");
		logFile.rdbuf()->pubsetbuf(0, 0); // Disable buffering to immediately flush log statements to the file
		outputStream = &logFile;
	}
}
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


