#include "DecisionTree.h"
#include <set>
#include <iostream>
#include <cmath>

using namespace std;
extern map< int, string > attribute_names;
extern map< string, set< string > > attribute_values;
extern string final_attribute_name;

string get_appropriate_attribute(vector< map< string, string > > train_sets);
double get_entropy(vector<double> probabilities);
double get_information_gain(vector< map< string, string > > train_sets, string attribute);

/*
 * 주어진 sets 를 기반으로 Decision Tree의 분기점을 생성합니다.
 */
DecisionTree::DecisionTree(vector< map< string, string > > train_sets)
{
	map<string, int> majority_vote;

	/* 분기점의 기준이 될 attribute 를 구합니다. */
	attribute_name = get_appropriate_attribute(train_sets);
	attribute_value = "undefined";

	/* majority vote 를 초기화합니다. */
	for (set<string>::iterator it = attribute_values[final_attribute_name].begin();
	                           it != attribute_values[final_attribute_name].end(); it++)
		majority_vote[*it] = 0;

	/* 자식 노드들을 생성합니다. */
	for (set<string>::iterator it = attribute_values[attribute_name].begin(); it != attribute_values[attribute_name].end(); it++)
	{
		string currentValue = *it;
		string previousResult = "";
		vector< map<string, string> > train_subsets;

		/* train set 전체를 순회하며, 현재 탐색하고자 하는 attribute value 인 경우 벡터에 추가합니다. */
		for (vector< map<string, string> >::iterator vec_it = train_sets.begin(); vec_it != train_sets.end(); vec_it++)
		{
			if ((*vec_it)[attribute_name] == currentValue)
			{
				/* 새 subset 에 해당하는 모든 결과값이 동일한지 확인합니다. */
				if (previousResult == "")
					previousResult = (*vec_it)[final_attribute_name];
				else if (previousResult != (*vec_it)[final_attribute_name])
					previousResult = "undefined";

				train_subsets.push_back(*vec_it);
				majority_vote[(*vec_it)[final_attribute_name]] += 1;
			}
		}
		
		if (previousResult == "undefined")
		{
			/* 지정되지 않은 경우, 새로운 분기점을 생성합니다.*/
			child_nodes[currentValue] = new DecisionTree(train_subsets);
		}
		else
		{
			/* 모든 노드가 같은 결과값을 갖는 경우, leaf node 값을 확정합니다. 
			   이 부분은 아무 노드가 없는 경우(값은 ""으로 처리) 또한 포함합니다. */
			child_nodes[currentValue] = new DecisionTree(previousResult);
		}
	}

	/*
	 * majority vote 를 시행합니다.
	 */
	string majority;
	int majorValue = -1;
	for (map<string, int>::iterator it = majority_vote.begin(); it != majority_vote.end(); it++)
	{
		if (majorValue < it->second)
		{
			majorValue = it->second;
			majority = it->first;
		}
	}

	for (map<string, DecisionTree*>::iterator it = child_nodes.begin(); it != child_nodes.end(); it++)
	{
		if (it->second->attribute_value == "")
			it->second->attribute_value = majority;
	}
}

/*
 * 해당 값을 가지는 left node 를 생성합니다.
 */
DecisionTree::DecisionTree(string value)
{
	attribute_name = "";
	attribute_value = value;
}

DecisionTree::~DecisionTree()
{
	for (map< string, DecisionTree* >::iterator it = child_nodes.begin(); it != child_nodes.end(); it++)
		delete it->second;
}

/*
 * test set 들을 트리에 적용하여 결과값을 도출합니다.
 */
vector< map< string, string > > DecisionTree::test(vector< map< string, string > >& test_sets)
{
	for (vector< map< string, string > >::iterator it = test_sets.begin(); it != test_sets.end(); it++)
		(*it)[final_attribute_name] = getAttributeValue(*it);

	return test_sets;
}

/*
 * 한 개의 test set 을 트리에서 leaf node 까지 탐색하여 결과값을 얻습니다.
 */
string DecisionTree::getAttributeValue(map<string, string> test_set)
{
	if (attribute_value == "undefined")
		return child_nodes[test_set[attribute_name]]->getAttributeValue(test_set);
	else
		return attribute_value;
}

/*
 * 주어진 train sets 중에서 가장 information gain 이 큰 attribute 를 찾습니다.
 */
string get_appropriate_attribute(vector< map< string, string > > train_sets)
{
	double max_information_gain = -1;
	string ret_attribute = "";

	for (int index = 0; (unsigned)index < attribute_names.size() - 1; index++)
	{
		double information_gain = get_information_gain(train_sets, attribute_names[index]);

		if (max_information_gain < information_gain)
		{
			ret_attribute = attribute_names[index];
			max_information_gain = information_gain;
		}
	}

	return ret_attribute;
}

/*
 * 주어진 n개의 확률들의 엔트로피를 계산합니다.
 */
double get_entropy(vector<double> probabilities)
{
	double entropy = 0;

	for (vector<double>::iterator it = probabilities.begin(); it != probabilities.end(); it++)
		entropy -= (*it) * log2(*it);

	return entropy;
}

/*
 * 주어진 attribute 의 information gain 을 구합니다.
 */
double get_information_gain(vector< map< string, string > > train_sets, string attribute)
{
	double remainder = 0, num_sets = 0, information_gain, entropies = 0;
	map< string, map< string, int > > count_sets;
	map< string, int > count_results_total, count_values_total;

	vector<double> probabilities;

	/* map 초기화 */
	for (set< string >::iterator it = attribute_values[attribute].begin(); it != attribute_values[attribute].end(); it++)
	{
		for (set< string >::iterator set_it = attribute_values[final_attribute_name].begin(); 
		     set_it != attribute_values[final_attribute_name].end();
		     set_it++)
		{
			count_sets[*it][*set_it] = 0;
			count_results_total[*set_it] = 0;
		}
		count_values_total[*it] = 0;
	}

	/* 모든 set들의 값을 추산합니다. */
	for (vector< map< string, string > >::iterator it = train_sets.begin(); it != train_sets.end(); it++)
	{
		count_sets[(*it)[attribute]][(*it)[final_attribute_name]] += 1;
		count_results_total[ (*it)[final_attribute_name] ] += 1;
		count_values_total[ (*it)[attribute] ] += 1;
		num_sets += 1;
	}

	if (num_sets == 0)
		return -1;

	/* 전체의 엔트로피를 information gain 에 대입합니다. */
	for (map<string, int>::iterator it = count_results_total.begin(); it != count_results_total.end(); it++)
		if (it->second != 0)
			probabilities.push_back(it->second / num_sets);
	
	information_gain = get_entropy(probabilities);

	/* 각각의 값들의 엔트로피의 총합을 구합니다. */
	for (map< string, map<string, int> >::iterator it = count_sets.begin(); it != count_sets.end(); it++)
	{
		probabilities.clear();
		for (map<string, int>::iterator map_it = it->second.begin(); map_it != it->second.end(); map_it++)
			if (count_values_total[it->first] != 0 && map_it->second != 0)
				probabilities.push_back((double)map_it->second / (double)count_values_total[it->first]);
		
		entropies += ( (double)count_values_total[it->first] / (double)num_sets ) * get_entropy(probabilities);
	}

	/* 엔트로피의 총량만큼 감소합니다. */
	information_gain -= fabs(entropies);

	return information_gain;
}