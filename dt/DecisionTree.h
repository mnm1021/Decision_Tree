#pragma once
#include <map>
#include <string>
#include <vector>

/*
 * decision tree 를 형성하는 클래스입니다.
 */
class DecisionTree
{
public:
	DecisionTree(std::vector< std::map< std::string, std::string > > train_sets);
	DecisionTree(std::string value);
	~DecisionTree();

	std::vector< std::map< std::string, std::string > > test(std::vector< std::map< std::string, std::string > >& test_sets);
	std::string DecisionTree::getAttributeValue(std::map<std::string, std::string> test_set);

	std::map< std::string, DecisionTree* > child_nodes;/* 다음 노드들을 가리킵니다. */
	std::string attribute_name;/* 이 노드가 구분하는 기준이 되는 attribute 의 이름을 저장합니다. */
	std::string attribute_value;/* leaf node 인 경우, 이 노드에 도달한 set 가 어떤 최종 값을 가질지를 표시합니다. */
};

