#ifndef ASSERTION_H
#define ASSERTION_H

#include <functional>
#include <map>
#include <string>
#include <sstream>
#include <iostream>

namespace UnitTest {
	class AssertionFailureException {};

	void assertTrue(bool expr) {
		if (!expr)
			throw AssertionFailureException();
	}

	void assertFalse(bool expr) {
		if (expr)
			throw AssertionFailureException();
	}

	template <typename T>
	void assertEquals(const T& op1, const T& op2) {
		if (op1 != op2)
			throw AssertionFailureException();
	}
	
	template <typename T>
	void assertArrayEquals(const T* op1, const T* op2, size_t sz) {
		for (size_t i = 0; i < sz; ++i)
			if (op1[i] != op2[i])
				throw AssertionFailureException();
	}

	template <typename T>
	void assertNullptr(const T& op1) {
		if (op1 != nullptr)
			throw AssertionFailureException();
	}

	class TestRunner {
	public:
		TestRunner();
		void runTest(std::string, std::function<void()>);
		void runTests();
	private:
		std::map<std::string, std::function<void()>> tests_;
	};

	TestRunner::TestRunner() : tests_() {}

	void TestRunner::runTest(std::string name, std::function<void()> test) {
		tests_.insert(make_pair(name, test));
	}

	void TestRunner::runTests() {
		std::ostringstream success_oss;
		success_oss << "SUCCESSFUL EXECUTIONS" << std::endl;
		std::ostringstream failure_oss;
		failure_oss << "FAILURE EXECUTIONS" << std::endl;
		unsigned int success_count = 0;
		for (std::pair<std::string, std::function<void()>> p : tests_) {
			try {
				p.second();
				success_oss << "\t" << p.first << std::endl;
				++success_count;
			} catch (AssertionFailureException e) {
				failure_oss << "\t" << p.first << std::endl;
			}
		}
		std::cout << success_oss.str() << std::endl;
		if (success_count < tests_.size())
			std::cout << failure_oss.str() << std::endl;
		std::cout << success_count << "/" << tests_.size() 
				<< " test cases completed successfully" << std::endl;
	}
};

#endif
