#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include <functional>
#include <string>

/// Foundation for a unit test
class TestRunner {
protected:
	void runTest(std::function<void()> test, std::string testName);
	virtual void runTests() = 0;
};

#endif
