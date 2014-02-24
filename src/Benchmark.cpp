#include "Benchmark.h"
#include <sstream>

Benchmark::Benchmark(std::string name) : name_(name), duration_(0)
	{}

void Benchmark::start() {
	start_ = std::chrono::steady_clock::now();
}

void Benchmark::stop() {
	duration_ = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::steady_clock::now() - start_).count();
}

std::string Benchmark::toString() const {
	std::ostringstream oss;
	oss << name_ << ": " << duration_;
	return oss.str();
}

std::string Benchmark::toString(std::string delim) const {
	std::ostringstream oss;
	oss << name_ << delim << duration_;
	return oss.str();
}

std::ostream& operator<<(std::ostream& os, const Benchmark& bm) {
	os << bm.toString();
	return os;
}
