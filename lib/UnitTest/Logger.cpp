#include "Logger.h"
#include <iostream>
#include <chrono>

using namespace UnitTest;

Logger& Logger::instance() {
	static Logger instance;
	return instance;
}

void Logger::log(LogType type, std::string message) {
	history_.push_back(Log(type, message));
	std::clog << " [" << type << "] " << message;
}

void Logger::externalLog(std::ostream& os, LogType type, std::string message) {
	history_.push_back(Log(type, message));
	os << " [" << type << "] " << message;
}

Logger::Log::Log(LogType type, std::string message) :
	type(type), message(message) {}