#include "BayesNet.h"
#include "Benchmark.h"
#include "Assertion.h"

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <tuple>

using namespace std;
using namespace UnitTest;

void canSample() {
	BayesNet<> bn;

	map<vector<int>, map<int, double>> cpt;
	map<int, double> dist;
	dist.insert(make_pair(0, 1.0));
	dist.insert(make_pair(1, 0.0));
	cpt.insert(CondProb<>::CondCase(vector<int>(), dist));
	bn.add_node(0, CondProb<>(cpt));

	assertEquals(bn.sample_node(0), 0);

	cpt.clear();
	dist.clear();
	dist.insert(make_pair(0, 0.0));
	dist.insert(make_pair(1, 1.0));
	cpt.insert(CondProb<>::CondCase(vector<int>(), dist));
	bn.add_node(1, CondProb<>(cpt));

	assertEquals(bn.sample_node(1), 1);
}

void canAverage() {
	BayesNet<> bn;

	map<vector<int>, map<int, double>> cpt;
	map<int, double> dist;
	dist.insert(make_pair(0, 1.0));
	dist.insert(make_pair(1, 0.0));
	cpt.insert(CondProb<>::CondCase(vector<int>(), dist));
	bn.add_node(0, CondProb<>(cpt));

	cpt.clear();
	dist.clear();
	dist.insert(make_pair(0, 1.0));
	dist.insert(make_pair(1, 0.0));
	cpt.insert(CondProb<>::CondCase(vector<int>(), dist));
	bn.add_node(1, CondProb<>(cpt));

	cpt.clear();
	dist.clear();
	dist.insert(make_pair(1, 0.7));
	dist.insert(make_pair(0, 0.3));
	cpt.insert(CondProb<>::CondCase(vector<int> {0, 0}, dist));
	dist.insert(make_pair(1, 0.5));
	dist.insert(make_pair(0, 0.5));
	cpt.insert(CondProb<>::CondCase(vector<int> {0, 1}, dist));
	dist.insert(make_pair(1, 0.5));
	dist.insert(make_pair(0, 0.5));
	cpt.insert(CondProb<>::CondCase(vector<int> {1, 0}, dist));
	dist.insert(make_pair(1, 0.3));
	dist.insert(make_pair(0, 0.7));
	cpt.insert(CondProb<>::CondCase(vector<int> {1, 1}, dist));
	bn.add_node(2, {0, 1}, CondProb<int>({1, 2}, cpt));

	double ev = bn.average_value(2, 1024);
	assertTrue(ev > 0.6 && ev < 0.8);
}

void canMarginalizeNode() {
	BayesNet<> bn;

	map<vector<int>, map<int, double>> cpt;
	map<int, double> dist;
	dist.insert(make_pair(0, 0.7));
	dist.insert(make_pair(1, 0.3));
	cpt.insert(CondProb<>::CondCase(vector<int>(), dist));
	bn.add_node(0, CondProb<>(cpt));

	map<int, double> marginal_dist = bn.marginal_dist(0, 512);
	assertTrue(marginal_dist[0] > 0.65 && marginal_dist[0] < 0.75);
	assertTrue(marginal_dist[1] > 0.25 && marginal_dist[1] < 0.35);
}

void canMarkovBlanket() {
	BayesNet<> bn;

	map<vector<int>, map<int, double>> cpt;
	map<int, double> dist;
	dist.insert(make_pair(0, 1.0));
	dist.insert(make_pair(1, 0.0));
	cpt.insert(CondProb<>::CondCase(vector<int>(), dist));
	bn.add_node(0, CondProb<>(cpt));

	cpt.clear();
	dist.clear();
	dist.insert(make_pair(0, 1.0));
	dist.insert(make_pair(1, 0.0));
	cpt.insert(CondProb<>::CondCase(vector<int>(), dist));
	bn.add_node(1, CondProb<>(cpt));

	cpt.clear();
	dist.clear();
	dist.insert(make_pair(1, 0.7));
	dist.insert(make_pair(0, 0.3));
	cpt.insert(CondProb<>::CondCase(vector<int> {0, 0}, dist));
	dist.insert(make_pair(1, 0.5));
	dist.insert(make_pair(0, 0.5));
	cpt.insert(CondProb<>::CondCase(vector<int> {0, 1}, dist));
	dist.insert(make_pair(1, 0.5));
	dist.insert(make_pair(0, 0.5));
	cpt.insert(CondProb<>::CondCase(vector<int> {1, 0}, dist));
	dist.insert(make_pair(1, 0.3));
	dist.insert(make_pair(0, 0.7));
	cpt.insert(CondProb<>::CondCase(vector<int> {1, 1}, dist));
	bn.add_node(2, {0, 1}, CondProb<int>({1, 2}, cpt));

	cpt.clear();
	dist.clear();
	dist.insert(make_pair(1, 0.7));
	dist.insert(make_pair(0, 0.3));
	cpt.insert(CondProb<>::CondCase(vector<int> {0}, dist));
	dist.insert(make_pair(1, 0.5));
	dist.insert(make_pair(0, 0.5));
	cpt.insert(CondProb<>::CondCase(vector<int> {1}, dist));
	bn.add_node(3, {2}, CondProb<int>({2}, cpt));

	set<int> blanket = bn.markov_blanket(2);
	assertEquals(blanket, set<int> {2, 0, 1, 3});
}

void canSampleNetwork() {
	BayesNet<> bn;

	map<vector<int>, map<int, double>> cpt;
	map<int, double> dist;
	dist.insert(make_pair(0, 1.0));
	dist.insert(make_pair(1, 0.0));
	cpt.insert(CondProb<>::CondCase(vector<int>(), dist));
	bn.add_node(0, CondProb<>(cpt));

	assertEquals(bn.sample_node(0), 0);

	cpt.clear();
	dist.clear();
	dist.insert(make_pair(0, 0.0));
	dist.insert(make_pair(1, 1.0));
	cpt.insert(CondProb<>::CondCase(vector<int>(), dist));
	bn.add_node(1, CondProb<>(cpt));

	map<int, int> values;
	values[0] = 0;
	values[1] = 1;
	assertEquals(bn.sample(), values);
}

void canMarginalizeNetwork() {
	BayesNet<> bn;

	map<vector<int>, map<int, double>> cpt;
	map<int, double> dist;
	dist.insert(make_pair(0, 0.7));
	dist.insert(make_pair(1, 0.3));
	cpt.insert(CondProb<>::CondCase(vector<int>(), dist));
	bn.add_node(0, CondProb<>(cpt));

	map<int, int> values;
	values.insert(make_pair(0, 0));
	map<std::map<int, int>, double> marginal_dist = bn.marginal_dist(values, 512, SampleStrategy::GIBBS);
	assertTrue(marginal_dist[values] > 0.65 && marginal_dist[values] < 0.75);
}

void run(unsigned int size, unsigned int select) {
	BayesNet<> bn;

	map<vector<int>, map<int, double>> cpt;
	map<int, double> dist;

	map<vector<int>, map<int, double>> sinkCPT;

	for (int i = 0; i < size; i++) {
		if (i == select) {
			dist.insert(make_pair(0, 0.0));
			dist.insert(make_pair(1, 1.0));
			cpt.insert(CondProb<>::CondCase(vector<int>(), dist));
			bn.add_node(i, CondProb<>(cpt));

			cpt.clear();

			dist.insert(make_pair(0, 0.0));
			dist.insert(make_pair(1, 1.0));
			vector<int> parents(size, 0);
			parents[i] = 1;
			sinkCPT.insert(CondProb<>::CondCase(parents, dist));
		} else {
			dist.insert(make_pair(0, 1.0));
			dist.insert(make_pair(1, 0.0));
			cpt.insert(CondProb<>::CondCase(vector<int>(), dist));
			bn.add_node(i, CondProb<>(cpt));

			cpt.clear();

			dist.insert(make_pair(0, 1.0));
			dist.insert(make_pair(1, 0.0));
			vector<int> parents(size, 0);
			parents[i] = 1;
			sinkCPT.insert(CondProb<>::CondCase(parents, dist));
		}

		cpt.clear();
		dist.clear();
	}

	set<int> parents;
	for (int i = 0; i < size; i++)
		parents.insert(i);
	bn.add_node(size, parents, CondProb<>(sinkCPT));

	map<int, int> values;
	for (int i = 0; i < size; i++)
		values.insert(make_pair(0, 0));
	values.insert(make_pair(select, 1));
	map<std::map<int, int>, double> marginal_dist = bn.marginal_dist(values, 512, SampleStrategy::GIBBS);
	assertEquals(marginal_dist[values], (double)1.0);
}

void runTests() {
	TestRunner runner;

	// Bayesian Node Tests
	runner.runTest("Can Sample Node", canSample);
	runner.runTest("Can Average Node Value", canAverage);
	runner.runTest("Can Marginalize Node", canMarginalizeNode);
	runner.runTest("Can obtain Markov Blanket of Node", canMarkovBlanket);
	
	// Bayesian Network Tests
	runner.runTest("Can Sample Network", canSampleNetwork);
	runner.runTest("Can Marginalize Network", canMarginalizeNetwork);

	ostringstream oss;
	for (int i = 100; i < 1000; i += 100) {
		oss << "Marginalization of Bayesian Network of Size (" << i << ")";
		for (int j = 8; j <= 64; j *= 2)
			runner.runTest(oss.str(), bind(run, i, j));
		oss.str("");
	}

	runner.runTests();
}

void benchmark() {
	vector<Benchmark> benchmarks;
	ostringstream oss;
	for (int i = 100; i < 1000; i += 100) {
		oss << "Marginalization of Bayesian Network of Size (" << i << ")";
		cout << "COMPUTING " << oss.str() << endl;
		Benchmark b(oss.str());
		b.start();
		for (int j = 8; j <= 64; j *= 2)
			run(i, j);
		b.stop();
		benchmarks.push_back(b);
		oss.str("");
	}

	for (Benchmark mark : benchmarks)
		cout << mark.toString(",") << "ms" << endl;
}

int main(int argc, char* argv[])
{
	// Uncomment for running tests
	// runTests();
	benchmark();
	return 0;
}
