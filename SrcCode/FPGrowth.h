#pragma once

class FPNode{

public:

	const int item;
	int num;
	const std::weak_ptr<const FPNode> parent_address;
	std::shared_ptr<FPNode> side_address;
	bool is_leaf;

	
	FPNode(const int Item, const int Num, const std::shared_ptr<const FPNode> & Parent_address,
	 const std::shared_ptr<FPNode> & Side_address): item{Item} , num{Num}, parent_address{Parent_address} ,
	  side_address{Side_address} 
	{
		is_leaf = true;
	}

	~FPNode()
	{
		//std::cout << "\n destroyed node: " << item << std::endl;
	}
};


class FPTree{

public:

	std::shared_ptr<FPNode> root_node;
 	

	FPTree(std::vector<std::tuple<int ,int , std::shared_ptr<FPNode> >> & header_table,const std::vector<std::vector<int>> & database);
	

	~FPTree()
	{

	}

};

using header_table_row = std::tuple<int, int, std::shared_ptr<FPNode>>;

void PrintDatabase(const std::vector<std::vector<int>> & given_db);

void ReadTransactionalDatabaseFile(const std::string & database_file, std::vector<std::unordered_set<int>> & given_db);

void PrintFrequentItemSets(const std::vector< std::pair<std::vector<int>,int> > &  frequent_item_sets,  const std::string & file_name);

void FPGrowthMineFrequentItemSets(const std::vector<std::unordered_set<int>> & given_db , const std::vector<int> & conditional_items, 
	std::vector< std::pair<std::vector<int>,int> > & frequent_item_sets);

void SortFrequentItemSetsLexicographicOrder(std::vector< std::pair<std::vector<int>,int> > &  frequent_item_sets);

extern int min_support_count;