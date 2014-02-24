#ifndef LOGGER_H
#define LOGGER_H

#include <ostream>
#include <chrono>
#include <list>
#include <string>

namespace UnitTest {
	enum LogType {
		INFO, WARN, ERRO
	};

	class Logger {
	public:
		Logger& instance();
		void log(LogType type, std::string message);
		void externalLog(std::ostream& os, LogType type, std::string message);
	private:
		class Log {
		public:
			LogType type;
			std::string message;

			Log(LogType type, std::string message);
		};

		Logger();
		Logger(const Logger&);
		Logger& operator=(const Logger&);

		std::list<Log> history_;
		std::ostream os_;
	};
};

#endif