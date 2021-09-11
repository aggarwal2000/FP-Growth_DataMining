#include<iostream>
#include<string>
#include<cstring>
#include<vector>
#include<sstream>
#include<fstream>
#include<math.h>
#include <unordered_map>
#include<unordered_set>
#include<map>
#include<tuple>
#include<utility>
#include <bits/stdc++.h>
#include<memory>
#include"FPGrowth.h"


int min_support_count;


int main(int argc, char *argv[])
{	
	
	if(argc != 3)
	{
		std::cout << "\n The format is: a.exe [database file name] [min support percentage] " << std::endl;
		return 0;
	}
	const std::string database_file =  argv[1];
	const float supp = atof(argv[2]);

	std::vector<std::unordered_set<int>> given_db; // first I am going for vector of vectors ;can also use a vector of unorderd sets here

	ReadTransactionalDatabaseFile(database_file, given_db);
	//PrintDatabase(given_db);

	min_support_count =  ceil((supp * given_db.size())/100);

	std::cout << "\nMin support count is: " << min_support_count << std::endl;

	std::vector< std::pair<std::vector<int>,int> > frequent_item_sets;

	std::vector<int> conditional_items;

	FPGrowthMineFrequentItemSets(given_db, conditional_items, frequent_item_sets);


	std::cout << "\n Num of frequent item sets: " << frequent_item_sets.size() << std::endl;

	SortFrequentItemSetsLexicographicOrder(frequent_item_sets);

	PrintFrequentItemSets(frequent_item_sets, "output.txt");

	
}