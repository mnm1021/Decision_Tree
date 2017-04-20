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
 * I/O 관련 함수들입니다.
 */
vector< map< string, string > > read_train_set(ifstream& ifs);
vector< map< string, string > > read_test_set(ifstream& ifs);
void print_test_sets(ofstream& ofs, vector< map< string, string > > test_sets);

/*
 * Attribute 의 이름과, 각각이 가질 수 있는 값의 이름을 전역 변수로 저장합니다.
 * 이 값은 train set 을 읽으며 초기화됩니다.
 */
map< int, string > attribute_names;
map< string, set< string > > attribute_values;
string final_attribute_name;

/*
 * argv[1] : train set, argv[2] : test set 이 저장된 파일명입니다.
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

	/* train set을 읽어들이고, attribute_names 및 attribute_values 의 값을 초기화합니다. */
	ifstream ifs(argv[1]);
	train_sets = read_train_set(ifs);
	ifs.close();

	/* test set을 읽어들입니다. */
	ifs.open(argv[2]);
	test_sets = read_test_set(ifs);
	ifs.close();

	/* train set 를 기반으로 decision tree 를 생성합니다. */
	dtRootNode = new DecisionTree(train_sets);

	/* test set 들을 decision tree 를 통해 탐색하여 결과값을 도출합니다. */
	test_sets = dtRootNode->test(test_sets);

	/* test set 결과물을 출력합니다. */
	ofstream ofs("dt_result.txt");
	print_test_sets(ofs, test_sets);
	ofs.close();

	delete dtRootNode;

	system("PAUSE");
	return 0;
}

/*
 * train set 의 정보를 읽어옴과 동시에, 해당 set 들의 값들을 저장하여 반환합니다.
 * 이 함수는 attribute_names 와 attribute_values 를 초기화합니다.
 */
vector< map< string, string > > read_train_set(ifstream& ifs)
{
	string line;
	vector< map< string, string > > train_sets;

	/* 한 줄을 읽어옵니다. */
	getline(ifs, line);
	
	/* 각 attribute 이름을 저장합니다. */
	istringstream iss(line);
	int index = 0;
	while (iss)
	{
		string substr;
		iss >> substr;

		/* 공백 문자열은 포함하지 않습니다. */
		if(!substr.empty())
			attribute_names[index++] = substr;
	}

	/* 파일을 마저 탐색합니다. */
	while (ifs)
	{
		/* 한 줄을 읽어옵니다. */
		getline(ifs, line);
		if (line.empty())
			break;

		map< string, string > new_set;

		/* 각 attribute 이름을 저장합니다. */
		istringstream iss(line);
		index = 0;
		while (iss)
		{
			string substr;
			iss >> substr;

			/* 공백 문자열은 포함하지 않습니다. */
			if (!substr.empty())
			{
				new_set[attribute_names[index]] = substr;
				attribute_values[attribute_names[index++]].insert(substr);
			}
		}

		/* 새로 읽어온 정보를 삽입합니다. */
		train_sets.push_back(new_set);
	}

	final_attribute_name = attribute_names[attribute_names.size() - 1];

	return train_sets;
}

/*
 * test set 의 정보를 읽어옵니다.
 */
vector< map< string, string > > read_test_set(ifstream& ifs)
{
	string line;
	vector< map< string, string > > test_sets;

	/* 한 줄을 읽어옵니다. (테스트 셋에서 한 줄은 버립니다.) */
	getline(ifs, line);

	/* 모든 test set 들을 읽습니다. */
	while (ifs)
	{
		/* 한 줄을 읽어옵니다. */
		getline(ifs, line);
		if (line.empty())
			break;

		map< string, string > new_set;

		/* 각 attribute 이름을 저장합니다. */
		istringstream iss(line);
		int index = 0;
		while (iss)
		{
			string substr;
			iss >> substr;

			/* 공백 문자열은 포함하지 않습니다. */
			if (!substr.empty())
				new_set[attribute_names[index++]] = substr;
		}

		/* 결과값은 일단 undefined 로 처리합니다. */
		new_set[attribute_names[index]] = "undefined";

		/* 새로 읽어온 정보를 삽입합니다. */
		test_sets.push_back(new_set);
	}

	return test_sets;
}

/*
 * test sets 의 결과물을 출력합니다.
 */
void print_test_sets(ofstream& ofs, vector< map< string, string > > test_sets)
{
	/* 모든 attribute 들을 출력합니다. */
	for (map< int, string >::iterator it = attribute_names.begin(); it != attribute_names.end(); it++)
	{
		ofs << it->second;
		if (it->first != attribute_names.size() - 1)
			ofs << "\t";
	}
	ofs << endl;

	/* 각각의 set 들을 출력합니다. */
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