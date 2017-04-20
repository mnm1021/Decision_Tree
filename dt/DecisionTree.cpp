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
 * �־��� sets �� ������� Decision Tree�� �б����� �����մϴ�.
 */
DecisionTree::DecisionTree(vector< map< string, string > > train_sets)
{
	map<string, int> majority_vote;

	/* �б����� ������ �� attribute �� ���մϴ�. */
	attribute_name = get_appropriate_attribute(train_sets);
	attribute_value = "undefined";

	/* majority vote �� �ʱ�ȭ�մϴ�. */
	for (set<string>::iterator it = attribute_values[final_attribute_name].begin();
	                           it != attribute_values[final_attribute_name].end(); it++)
		majority_vote[*it] = 0;

	/* �ڽ� ������ �����մϴ�. */
	for (set<string>::iterator it = attribute_values[attribute_name].begin(); it != attribute_values[attribute_name].end(); it++)
	{
		string currentValue = *it;
		string previousResult = "";
		vector< map<string, string> > train_subsets;

		/* train set ��ü�� ��ȸ�ϸ�, ���� Ž���ϰ��� �ϴ� attribute value �� ��� ���Ϳ� �߰��մϴ�. */
		for (vector< map<string, string> >::iterator vec_it = train_sets.begin(); vec_it != train_sets.end(); vec_it++)
		{
			if ((*vec_it)[attribute_name] == currentValue)
			{
				/* �� subset �� �ش��ϴ� ��� ������� �������� Ȯ���մϴ�. */
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
			/* �������� ���� ���, ���ο� �б����� �����մϴ�.*/
			child_nodes[currentValue] = new DecisionTree(train_subsets);
		}
		else
		{
			/* ��� ��尡 ���� ������� ���� ���, leaf node ���� Ȯ���մϴ�. 
			   �� �κ��� �ƹ� ��尡 ���� ���(���� ""���� ó��) ���� �����մϴ�. */
			child_nodes[currentValue] = new DecisionTree(previousResult);
		}
	}

	/*
	 * majority vote �� �����մϴ�.
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
 * �ش� ���� ������ left node �� �����մϴ�.
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
 * test set ���� Ʈ���� �����Ͽ� ������� �����մϴ�.
 */
vector< map< string, string > > DecisionTree::test(vector< map< string, string > >& test_sets)
{
	for (vector< map< string, string > >::iterator it = test_sets.begin(); it != test_sets.end(); it++)
		(*it)[final_attribute_name] = getAttributeValue(*it);

	return test_sets;
}

/*
 * �� ���� test set �� Ʈ������ leaf node ���� Ž���Ͽ� ������� ����ϴ�.
 */
string DecisionTree::getAttributeValue(map<string, string> test_set)
{
	if (attribute_value == "undefined")
		return child_nodes[test_set[attribute_name]]->getAttributeValue(test_set);
	else
		return attribute_value;
}

/*
 * �־��� train sets �߿��� ���� information gain �� ū attribute �� ã���ϴ�.
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
 * �־��� n���� Ȯ������ ��Ʈ���Ǹ� ����մϴ�.
 */
double get_entropy(vector<double> probabilities)
{
	double entropy = 0;

	for (vector<double>::iterator it = probabilities.begin(); it != probabilities.end(); it++)
		entropy -= (*it) * log2(*it);

	return entropy;
}

/*
 * �־��� attribute �� information gain �� ���մϴ�.
 */
double get_information_gain(vector< map< string, string > > train_sets, string attribute)
{
	double remainder = 0, num_sets = 0, information_gain, entropies = 0;
	map< string, map< string, int > > count_sets;
	map< string, int > count_results_total, count_values_total;

	vector<double> probabilities;

	/* map �ʱ�ȭ */
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

	/* ��� set���� ���� �߻��մϴ�. */
	for (vector< map< string, string > >::iterator it = train_sets.begin(); it != train_sets.end(); it++)
	{
		count_sets[(*it)[attribute]][(*it)[final_attribute_name]] += 1;
		count_results_total[ (*it)[final_attribute_name] ] += 1;
		count_values_total[ (*it)[attribute] ] += 1;
		num_sets += 1;
	}

	if (num_sets == 0)
		return -1;

	/* ��ü�� ��Ʈ���Ǹ� information gain �� �����մϴ�. */
	for (map<string, int>::iterator it = count_results_total.begin(); it != count_results_total.end(); it++)
		if (it->second != 0)
			probabilities.push_back(it->second / num_sets);
	
	information_gain = get_entropy(probabilities);

	/* ������ ������ ��Ʈ������ ������ ���մϴ�. */
	for (map< string, map<string, int> >::iterator it = count_sets.begin(); it != count_sets.end(); it++)
	{
		probabilities.clear();
		for (map<string, int>::iterator map_it = it->second.begin(); map_it != it->second.end(); map_it++)
			if (count_values_total[it->first] != 0 && map_it->second != 0)
				probabilities.push_back((double)map_it->second / (double)count_values_total[it->first]);
		
		entropies += ( (double)count_values_total[it->first] / (double)num_sets ) * get_entropy(probabilities);
	}

	/* ��Ʈ������ �ѷ���ŭ �����մϴ�. */
	information_gain -= fabs(entropies);

	return information_gain;
}