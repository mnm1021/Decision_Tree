#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <sstream>
#include "DecisionTree.h"

using namespace std;

/*
 * I/O ���� �Լ����Դϴ�.
 */
vector< map< string, string > > read_train_set(ifstream& ifs);
vector< map< string, string > > read_test_set(ifstream& ifs);
void print_test_sets(ofstream& ofs, vector< map< string, string > > test_sets);

/*
 * Attribute �� �̸���, ������ ���� �� �ִ� ���� �̸��� ���� ������ �����մϴ�.
 * �� ���� train set �� ������ �ʱ�ȭ�˴ϴ�.
 */
map< int, string > attribute_names;
map< string, set< string > > attribute_values;
string final_attribute_name;

/*
 * argv[1] : train set, argv[2] : test set �� ����� ���ϸ��Դϴ�.
 */
int main(int argc, char** argv)
{
	vector< map< string, string > > train_sets, test_sets;
	DecisionTree* dtRootNode;

	if (argc != 3)
	{
		cout << "usage : dt.exe [train_text_file] [test_text_file]" << endl;
		exit(-1);
	}

	/* train set�� �о���̰�, attribute_names �� attribute_values �� ���� �ʱ�ȭ�մϴ�. */
	ifstream ifs(argv[1]);
	train_sets = read_train_set(ifs);
	ifs.close();

	/* test set�� �о���Դϴ�. */
	ifs.open(argv[2]);
	test_sets = read_test_set(ifs);
	ifs.close();

	/* train set �� ������� decision tree �� �����մϴ�. */
	dtRootNode = new DecisionTree(train_sets);

	/* test set ���� decision tree �� ���� Ž���Ͽ� ������� �����մϴ�. */
	test_sets = dtRootNode->test(test_sets);

	/* test set ������� ����մϴ�. */
	ofstream ofs("dt_result.txt");
	print_test_sets(ofs, test_sets);
	ofs.close();

	delete dtRootNode;

	system("PAUSE");
	return 0;
}

/*
 * train set �� ������ �о�Ȱ� ���ÿ�, �ش� set ���� ������ �����Ͽ� ��ȯ�մϴ�.
 * �� �Լ��� attribute_names �� attribute_values �� �ʱ�ȭ�մϴ�.
 */
vector< map< string, string > > read_train_set(ifstream& ifs)
{
	string line;
	vector< map< string, string > > train_sets;

	/* �� ���� �о�ɴϴ�. */
	getline(ifs, line);
	
	/* �� attribute �̸��� �����մϴ�. */
	istringstream iss(line);
	int index = 0;
	while (iss)
	{
		string substr;
		iss >> substr;

		/* ���� ���ڿ��� �������� �ʽ��ϴ�. */
		if(!substr.empty())
			attribute_names[index++] = substr;
	}

	/* ������ ���� Ž���մϴ�. */
	while (ifs)
	{
		/* �� ���� �о�ɴϴ�. */
		getline(ifs, line);
		if (line.empty())
			break;

		map< string, string > new_set;

		/* �� attribute �̸��� �����մϴ�. */
		istringstream iss(line);
		index = 0;
		while (iss)
		{
			string substr;
			iss >> substr;

			/* ���� ���ڿ��� �������� �ʽ��ϴ�. */
			if (!substr.empty())
			{
				new_set[attribute_names[index]] = substr;
				attribute_values[attribute_names[index++]].insert(substr);
			}
		}

		/* ���� �о�� ������ �����մϴ�. */
		train_sets.push_back(new_set);
	}

	final_attribute_name = attribute_names[attribute_names.size() - 1];

	return train_sets;
}

/*
 * test set �� ������ �о�ɴϴ�.
 */
vector< map< string, string > > read_test_set(ifstream& ifs)
{
	string line;
	vector< map< string, string > > test_sets;

	/* �� ���� �о�ɴϴ�. (�׽�Ʈ �¿��� �� ���� �����ϴ�.) */
	getline(ifs, line);

	/* ��� test set ���� �н��ϴ�. */
	while (ifs)
	{
		/* �� ���� �о�ɴϴ�. */
		getline(ifs, line);
		if (line.empty())
			break;

		map< string, string > new_set;

		/* �� attribute �̸��� �����մϴ�. */
		istringstream iss(line);
		int index = 0;
		while (iss)
		{
			string substr;
			iss >> substr;

			/* ���� ���ڿ��� �������� �ʽ��ϴ�. */
			if (!substr.empty())
				new_set[attribute_names[index++]] = substr;
		}

		/* ������� �ϴ� undefined �� ó���մϴ�. */
		new_set[attribute_names[index]] = "undefined";

		/* ���� �о�� ������ �����մϴ�. */
		test_sets.push_back(new_set);
	}

	return test_sets;
}

/*
 * test sets �� ������� ����մϴ�.
 */
void print_test_sets(ofstream& ofs, vector< map< string, string > > test_sets)
{
	/* ��� attribute ���� ����մϴ�. */
	for (map< int, string >::iterator it = attribute_names.begin(); it != attribute_names.end(); it++)
	{
		ofs << it->second;
		if (it->first != attribute_names.size() - 1)
			ofs << "\t";
	}
	ofs << endl;

	/* ������ set ���� ����մϴ�. */
	for (vector< map< string, string > >::iterator it = test_sets.begin(); it != test_sets.end(); it++)
	{
		int index = 0;
		for (map< int, string >::iterator map_it = attribute_names.begin(); map_it != attribute_names.end(); map_it++)
		{
			ofs << (*it)[map_it->second];
			if (map_it->first != attribute_names.size() - 1)
				ofs << "\t";
		}
		ofs << endl;
	}
}