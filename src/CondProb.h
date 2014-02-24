#ifndef COND_PROB_H
#define COND_PROB_H

#include <vector>
#include <set>
#include <map>

template <
	typename NodeType = int,
	typename ValueType = int,
	typename DistType = std::map<ValueType, double>
>
class CondProb
{
public:
	// constructors
	CondProb();
	CondProb(const std::map<std::vector<ValueType>, DistType>& table);
	CondProb(const std::set<NodeType>& scope, 
		const std::map<std::vector<ValueType>, DistType>& table);

	// observers
	const DistType get_distribution(
		const std::vector<ValueType>& parentvalues);

	// size of the conditional probability table
	unsigned int size() const;

	using CondCase = std::pair<std::vector<ValueType>, DistType>;
private:
	// parents in table
	std::set<NodeType> scope_;

	// conditional probability table
	std::map<std::vector<ValueType>, DistType> table_;
};

template <typename NodeType, typename ValueType, typename DistType>
CondProb<NodeType, ValueType, DistType>::CondProb() {}

template <typename NodeType, typename ValueType, typename DistType>
CondProb<NodeType, ValueType, DistType>::CondProb(const std::map<std::vector<ValueType>, DistType>& table) :
	scope_(), table_(table)
{}

template <typename NodeType, typename ValueType, typename DistType>
CondProb<NodeType, ValueType, DistType>::CondProb(const std::set<NodeType>& scope, 
	const std::map<std::vector<ValueType>, DistType>& table) :
	scope_(scope), table_(table)
{}

template <typename NodeType, typename ValueType, typename DistType>
const DistType CondProb<NodeType, ValueType, DistType>::get_distribution(
	const std::vector<ValueType>& parentValues) {
	return table_[parentValues];
}

template <typename NodeType, typename ValueType, typename DistType>
unsigned int CondProb<NodeType, ValueType, DistType>::size() const { return table_.size(); }

#endif
