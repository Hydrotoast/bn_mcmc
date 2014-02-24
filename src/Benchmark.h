#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <chrono>
#include <ostream>

class Benchmark {
public:
	Benchmark(std::string name);

	void start();
	void stop();
	std::string toString() const;
	std::string toString(std::string delim) const;
private:
	std::string name_;

	std::chrono::steady_clock::time_point start_;
	long long duration_;
};

std::ostream& operator<<(std::ostream& os, const Benchmark& bm);

#endif
