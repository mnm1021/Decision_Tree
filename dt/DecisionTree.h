#pragma once
#include <map>
#include <string>
#include <vector>

/*
 * decision tree �� �����ϴ� Ŭ�����Դϴ�.
 */
class DecisionTree
{
public:
	DecisionTree(std::vector< std::map< std::string, std::string > > train_sets);
	DecisionTree(std::string value);
	~DecisionTree();

	std::vector< std::map< std::string, std::string > > test(std::vector< std::map< std::string, std::string > >& test_sets);
	std::string DecisionTree::getAttributeValue(std::map<std::string, std::string> test_set);

	std::map< std::string, DecisionTree* > child_nodes;/* ���� ������ ����ŵ�ϴ�. */
	std::string attribute_name;/* �� ��尡 �����ϴ� ������ �Ǵ� attribute �� �̸��� �����մϴ�. */
	std::string attribute_value;/* leaf node �� ���, �� ��忡 ������ set �� � ���� ���� �������� ǥ���մϴ�. */
};

