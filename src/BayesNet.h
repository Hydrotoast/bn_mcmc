#ifndef BAYES_NET_H
#define BAYES_NET_H

#include "CondProb.h"

#include <vector>
#include <map>
#include <set>
#include <tuple>

#include <iostream>
#include <cassert>
#include <type_traits>
#include <random>
#include <algorithm>
#include <functional>

// Potential errors
class SampleError {};
class DuplicateNodeException {};

// Sampling strategies for methods that use
// stochastic processes
enum class SampleStrategy { GIBBS, MH };

// Templated Bayesian network as a graph
template <
	typename NodeType = int,
	typename ValueType = int,
	typename DistType = std::map<ValueType, double>
>
class BayesNet
{
public:
	// constructor
	BayesNet();

	// mutators
	
	// Add a node with a specified label and conditional probability table
	void add_node(NodeType node_id,
		const CondProb<NodeType, ValueType, DistType>& condProb);

	// Add a node with a specified label, parents, and a conditional
	// probability table
	void add_node(NodeType node_id,
		const std::set<NodeType>& parents, 
		const CondProb<NodeType, ValueType, DistType>& condProb);

	// Clamp a node to a value
	void observe(NodeType node_id, ValueType value);

	// Clamp a set of nodes to a value
	void observe(const std::map<NodeType, ValueType> evidence);

	// accessors
	std::set<ValueType> markov_blanket(NodeType node_id);

	// generators
	std::map<NodeType, ValueType> sample();
	ValueType sample_node(NodeType node_id);
	ValueType sample_node(NodeType node_id, std::vector<ValueType> parent_values);

	// stochastic sampling algorithms
	std::vector<std::map<NodeType, ValueType>> gibbs_sample(unsigned int count, unsigned int burn_in);
	std::vector<std::map<NodeType, ValueType>> metropolis_sample(unsigned int count, unsigned int burn_in);

	// inference queries
	ValueType expected_value(NodeType node_id, unsigned int count);
	float average_value(NodeType node_id, unsigned int count);
	DistType marginal_dist(NodeType node_id, unsigned int count);

	// Resolve a marginal distribution for a query involving
	// where specified nodes hold specified values in q
	std::map<std::map<NodeType, ValueType>, double> marginal_dist(
		std::map<NodeType, ValueType> q, 
		unsigned int count,
		SampleStrategy strat);
private:
	DistType normalize_dist(
		const std::map<ValueType, int>& hist, 
		unsigned int count);

	double sample_probability(std::map<NodeType, ValueType> sample);

	int numNodes_;
	std::set<NodeType> nodes_;
	std::map<ValueType, std::set<NodeType>> parents_;
	std::map<ValueType, std::set<NodeType>> children_;
	std::set<NodeType> sinks_;
	std::map<ValueType, CondProb<NodeType, ValueType, DistType>> probabilities_;
	std::map<NodeType, ValueType> observations_;
};

template <typename NodeType, typename ValueType, typename DistType>
BayesNet<NodeType, ValueType, DistType>::BayesNet() :
	numNodes_(0),
	parents_(),
	probabilities_()
{}

template <typename NodeType, typename ValueType, typename DistType>
void BayesNet<NodeType, ValueType, DistType>::add_node(NodeType node_id, 
	const CondProb<NodeType, ValueType, DistType>& condProb) {
	auto it = nodes_.find(node_id);
	if (it != nodes_.end())
		throw DuplicateNodeException();
	
	nodes_.insert(node_id);

	// Update parents
	parents_[node_id] = std::set<NodeType>();
	
	// Update probabilities
	probabilities_[node_id] = condProb;

	// Update sinks
	sinks_.insert(node_id);

	++numNodes_;
}

template <typename NodeType, typename ValueType, typename DistType>
void BayesNet<NodeType, ValueType, DistType>::add_node(NodeType node_id, 
	const std::set<NodeType>& parents, 
	const CondProb<NodeType, ValueType, DistType>& condProb) {
	auto it = nodes_.find(node_id);
	if (it != nodes_.end())
		throw DuplicateNodeException();
	
	nodes_.insert(node_id);

	// Update parents
	parents_[node_id] = parents;
	for (ValueType parent : parents)
		children_[parent].insert(node_id);
	
	// Update probabilities
	probabilities_[node_id] = condProb;

	// Update sinks
	for (NodeType parent : parents) {
		auto it = sinks_.find(parent);
		if (it != sinks_.end())
			sinks_.erase(it++);
	}
	sinks_.insert(node_id);

	++numNodes_;
}

template <typename NodeType, typename ValueType, typename DistType>
void BayesNet<NodeType, ValueType, DistType>::observe(NodeType node_id, ValueType value) {
	observations_[node_id] = value;
}

template <typename NodeType, typename ValueType, typename DistType>
void BayesNet<NodeType, ValueType, DistType>::observe(std::map<NodeType, ValueType> evidence) {
	observations_.insert(evidence.begin(), evidence.end());
}

template <typename NodeType, typename ValueType, typename DistType>
std::set<ValueType> BayesNet<NodeType, ValueType, DistType>::markov_blanket(NodeType node_id) {
	std::set<ValueType> blanket;
	blanket.insert(node_id);
	blanket.insert(parents_[node_id].begin(), parents_[node_id].end());
	for (NodeType child : children_[node_id]) {
		blanket.insert(child);
		blanket.insert(parents_[child].begin(), parents_[child].end());
	}
	return blanket;
}

template <typename NodeType, typename ValueType, typename DistType>
std::map<NodeType, ValueType> BayesNet<NodeType, ValueType, DistType>::sample() {
	std::map<NodeType, ValueType> samples;
	for (NodeType node : nodes_)
		samples.insert(std::make_pair(node, sample_node(node)));
	return samples;
}

template <typename NodeType, typename ValueType, typename DistType>
ValueType BayesNet<NodeType, ValueType, DistType>::sample_node(NodeType node_id) {
	// handle the observed case
	auto it = observations_.find(node_id);
	if (it != observations_.end())
		return it->second;

	double value;
	std::vector<ValueType> values;
	for (NodeType parent : parents_[node_id])
		values.push_back(sample_node(parent));
	DistType cond_dist = probabilities_[node_id].get_distribution(values);
	DistType probs;
	double sum = 0;
	for (std::pair<ValueType, double> cond_prob : cond_dist) {
		sum += cond_prob.second;
		probs.insert(std::make_pair(cond_prob.first, sum));
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(0, 1);
	double prob = dist(gen);

	for (std::pair<ValueType, double> cond_prob : probs)
		if (prob <= cond_prob.second)
			return cond_prob.first;
	throw SampleError();
}

template <typename NodeType, typename ValueType, typename DistType>
ValueType BayesNet<NodeType, ValueType, DistType>::sample_node(NodeType node_id,
	std::vector<ValueType> parent_values) {
	// handle the observed case
	auto it = observations_.find(node_id);
	if (it != observations_.end())
		return it->second;

	DistType cond_dist = probabilities_[node_id].get_distribution(parent_values);
	DistType probs;
	double sum = 0;
	for (std::pair<ValueType, double> cond_prob : cond_dist) {
		sum += cond_prob.second;
		probs.insert(std::make_pair(cond_prob.first, sum));
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(0, 1);
	double prob = dist(gen);

	for (std::pair<ValueType, double> cond_prob : probs)
		if (prob <= cond_prob.second)
			return cond_prob.first;
	throw SampleError();
}

template <typename NodeType, typename ValueType, typename DistType>
std::vector<std::map<NodeType, ValueType>> BayesNet<NodeType, ValueType, DistType>::gibbs_sample(
	unsigned int count,
	unsigned int burn_in) {
	std::vector<std::map<NodeType, ValueType>> chain;
	chain.push_back(sample());

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> int_dist(0, nodes_.size() - 1);

	for (int i = 1; i < count + burn_in; i++) {
		auto it = nodes_.begin();
		std::advance(it, int_dist(gen));

		std::map<NodeType, ValueType> candidate(chain.back().begin(), chain.back().end());
		std::vector<ValueType> parent_values;
		for (NodeType parent : parents_[*it])
			parent_values.push_back(chain.back().find(parent)->second);
		candidate[*it] = sample_node(*it, parent_values);

		chain.push_back(candidate);
	}

	return std::vector<std::map<NodeType, ValueType>>(&chain[burn_in], &chain[count+burn_in]);
}

template <typename NodeType, typename ValueType, typename DistType>
std::vector<std::map<NodeType, ValueType>> BayesNet<NodeType, ValueType, DistType>::metropolis_sample(
	unsigned int count,
	unsigned int burn_in) {
	std::vector<std::map<NodeType, ValueType>> chain;
	chain.push_back(sample());

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> real_dist(0, 1);
	std::normal_distribution<> norm_dist(5, 2);
	std::uniform_int_distribution<> int_dist(0, nodes_.size() - 1);

	for (int i = 1; i < count + burn_in; i++) {
		auto it = nodes_.begin();
		std::advance(it, int_dist(gen));
		std::map<NodeType, ValueType> candidate(chain.back());
		std::vector<ValueType> parent_values;
		for (NodeType parent : parents_[*it])
			parent_values.push_back(chain.back().find(parent)->second);
		candidate[*it] = sample_node(*it, parent_values);
		double aprob = std::min<double>(1, sample_probability(
			candidate) / sample_probability(chain.back()));

		if (int_dist(gen) < aprob)
			chain.push_back(candidate);
		else
			chain.push_back(chain.back());
	}

	return std::vector<std::map<NodeType, ValueType>>(&chain[burn_in], &chain[count+burn_in]);
}

template <typename NodeType, typename ValueType, typename DistType>
ValueType BayesNet<NodeType, ValueType, DistType>::expected_value(NodeType node_id, 
	unsigned int count) {
	std::map<ValueType, int> hist;
	std::pair<ValueType, int> max_pair;
	for (int i = 0; i < count; i++) {
		auto s = sample_node(node_id);
		auto it = hist.find(s);
		if (it == hist.end)
			hist[s] = 0;
		else
			hist[s] = it->second + 1;
		if (it->second > max_pair->second) max_pair = it;
	}

	return max_pair->first;
}

template <typename NodeType, typename ValueType, typename DistType>
DistType BayesNet<NodeType, ValueType, DistType>::marginal_dist(
	NodeType node_id,
	unsigned int count) {
	std::map<ValueType, int> hist;
	for (int i = 0; i < count; i++) {
		auto s = sample_node(node_id);
		auto it = hist.find(s);
		if (it == hist.end())
			hist[s] = 0;
		else
			hist[s] = it->second + 1;
	}

	return normalize_dist(hist, count);
}

template <typename NodeType, typename ValueType, typename DistType>
std::map<std::map<NodeType, ValueType>, double> BayesNet<NodeType, ValueType, DistType>::marginal_dist(
	std::map<NodeType, ValueType> q,
	unsigned int count,
	SampleStrategy strat) {
	std::vector<std::map<NodeType, ValueType>> samples;
	if (strat == SampleStrategy::GIBBS)
		samples = gibbs_sample(count, 32);
	else
		samples = metropolis_sample(count, 32);

	unsigned int hits = 0;
	for (std::map<NodeType, ValueType> sample : samples)
		if (std::includes(sample.begin(), sample.end(), q.begin(), q.end()))
			++hits;

	std::map<std::map<NodeType, ValueType>, double> hist;
	hist[q] = hits / (double) count;
	return hist;
}

template <typename NodeType, typename ValueType, typename DistType>
float BayesNet<NodeType, ValueType, DistType>::average_value(NodeType node_id, 
	unsigned int count) {
	assert(std::is_integral<ValueType>::value);
	std::vector<ValueType> samples;
	for (int i = 0; i < count; i++)
		samples.push_back(sample_node(node_id));
	return std::accumulate(samples.begin(), samples.end(), 0) / (float)count;
}

template <typename NodeType, typename ValueType, typename DistType>
DistType BayesNet<NodeType, ValueType, DistType>::normalize_dist(
	const std::map<ValueType, int>& hist, 
	unsigned int count) {
	DistType normalized_dist;
	for (std::pair<ValueType, double> prob : hist)
		normalized_dist[prob.first] = prob.second / (double)count;
	return normalized_dist;
}

template <typename NodeType, typename ValueType, typename DistType>
double BayesNet<NodeType, ValueType, DistType>::sample_probability(
	std::map<NodeType, ValueType> sample) {
	std::vector<ValueType> parent_values;
	double accumulator = 1;
	for (std::pair<NodeType, ValueType> p : sample) {
		for (NodeType parent : parents_[p.first])
			parent_values.push_back(sample[parent]);
		double prob = probabilities_[p.first].
			get_distribution(parent_values).at(p.second);
		accumulator *= prob;
	}
	return accumulator;
}

#endif
