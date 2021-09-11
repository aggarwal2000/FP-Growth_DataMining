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

namespace{


bool compare_header_table_rows( const header_table_row & T_a , const header_table_row & T_b)
{
	return std::get<1>(T_a) >= std::get<1>(T_b);
}


std::pair< std::vector<header_table_row> , std::vector<std::vector<int>>>  CreateHeaderTableAndReducedDataBase(const std::vector<std::unordered_set<int>> & database)
{
	std::unordered_map<int , int> frequency_table;

	const int num_transactions =  database.size();

	for(int t = 0; t < num_transactions; t++)
	{
		for(auto itr = database[t].begin(); itr != database[t].end(); itr++)
		{	
			int item = *itr;
			
			auto it = frequency_table.find(item);

			if(it == frequency_table.end())
			{
				frequency_table[item] = 1;
			}
			else
			{
				it->second = it->second + 1;
			}

		}
	}


    std::vector<header_table_row> header_table;

    //Now...
    for (auto itr = frequency_table.begin(); itr != frequency_table.end(); itr++)
    {	
    	if(itr->second >= min_support_count)
    	{	
    		header_table.push_back(header_table_row(itr->first, itr->second, nullptr));
    	}
       
    }

   
    std::sort(header_table.begin(), header_table.end(), compare_header_table_rows);

    // for(int i = 0; i < header_table.size();i++)
    // {
    // 	std::cout << std::get<0>(header_table[i]) << "    " << std::get<1>(header_table[i]) << std::endl;
    // }



    std::vector<std::vector<int>> reduced_database;
    for(int trans = 0; trans < database.size(); trans++)
    {	
    	std::vector<int> row_reduced_db;

    	for(int i = 0; i < header_table.size(); i++)
    	{
    		const int item_in_header_table = std::get<0>(header_table[i]);

    		if(database[trans].find(item_in_header_table) != database[trans].end())
    		{
    			row_reduced_db.push_back(item_in_header_table);
    		}
    	}

    	reduced_database.push_back(std::move(row_reduced_db));
    }

    return std::pair< std::vector<header_table_row> , std::vector<std::vector<int>>>(std::move(header_table), std::move(reduced_database));

}



int Get_Suitable_row(const std::vector<std::tuple<int ,int , std::shared_ptr<FPNode> >> & header_table , const int item)
{
	for(int row = 0; row < header_table.size(); row++)
	{
		if(item == std::get<0>(header_table[row]))
		{
			return row;
		}
	}
}




void PrintFPTree(const std::vector<header_table_row> & header_table, const FPTree & fp_tree)
{
	for(int i = 0; i < header_table.size(); i++)
	{
		std::cout <<  std::get<0>(header_table[i]) << "   " << std::get<1>(header_table[i])  << std::endl;

		//std::cout <<  std::get<2>(header_table[i]) << std::endl;

		std::shared_ptr<const FPNode> address = std::get<2>(header_table[i]);

		while(address != nullptr)
		{	

			if(address->is_leaf == true)
			{	
				//std::cout << " Leaf to Root: " << std::endl;

				std::shared_ptr<const FPNode> curr_address = address;

				while(curr_address != nullptr)
				{	
					//std::cout << "Item: " << curr_address->item << "  number: " << curr_address->num << std::endl;

					curr_address = (curr_address->parent_address).lock();
				}

			}
			address = address->side_address;

		}
	}


}




std::pair<bool, int> Check_Single_Path(const std::vector<header_table_row> & header_table, const FPTree & fp_tree)
{	
	int header_row_leaf = -1;

	int num_leaf_nodes = 0;

	for(int i = 0; i < header_table.size(); i++)
	{
		//std::cout <<  std::get<0>(header_table[i]) << "   " << std::get<1>(header_table[i])  << std::endl;

		//std::cout <<  std::get<2>(header_table[i]) << std::endl;

		std::shared_ptr<FPNode> address =  std::get<2>(header_table[i]);

		while(address != nullptr)
		{	

			if(address->is_leaf == true)
			{		
				num_leaf_nodes++;
				header_row_leaf = i;
			}

			address = address->side_address;

		}
	}

	if(num_leaf_nodes == 1)
	{
		return std::pair<bool, int>(true, header_row_leaf);
	}
	else
	{
		return std::pair<bool, int>(false, -1);
	}
}


void FindSubsets(int curr_index, std::vector<  std::pair<std::vector<int>,int> > & subsets,
 const std::vector<std::pair<int, int>> & items_in_a_path)
{
	if(curr_index == items_in_a_path.size())
	{
		return;
	}
	else
	{	
		int curr_item = items_in_a_path[curr_index].first;
		int curr_item_count = items_in_a_path[curr_index].second;

		std::vector<  std::pair<std::vector<int>,int> >  new_subsets;

		for(int i = 0; i < subsets.size(); i++)
		{
			new_subsets.push_back(subsets[i]);

			(new_subsets[i].first).push_back(curr_item);

			if(curr_item_count < new_subsets[i].second)
			{
				new_subsets[i].second = curr_item_count;
			}
			
		}

		std::pair< std::vector<int>, int> new_single_item;

		(new_single_item.first).push_back(curr_item);
		new_single_item.second = curr_item_count;

		new_subsets.push_back(new_single_item);

		for(int i = 0; i < new_subsets.size(); i++)
		{
			subsets.push_back(new_subsets[i]);
		}


		curr_index += 1;

		FindSubsets(curr_index, subsets, items_in_a_path);
	}
}


void EnumerateSubsets(const std::vector<header_table_row> & header_table, const FPTree & fp_tree,
 const int row, const std::vector<int> & conditional_items, std::vector< std::pair<std::vector<int>,int> > & frequent_item_sets)
{
	std::shared_ptr<const FPNode> address =  std::get<2>(header_table[row]);

	std::vector<std::pair<int, int>> items_in_a_path;

	while(address != nullptr && (address->parent_address).lock() != nullptr)
	{
		items_in_a_path.push_back(std::pair<int,int>(address->item , address->num));
		//std::cout << "item in path: " << address->item << "  and count num: " << address->num << std::endl;
		address = (address->parent_address).lock();
	}

	//items_in_a_path.pop_back(); //destroys obj?


	//find all subsets and take support: min of the elems in subsets ; also take union with conditional elems
	//and push back these to frequent item sets.

	std::vector<  std::pair<std::vector<int>,int> > subsets;

	//std::cout << "\n\nprint num items: " << items_in_a_path.size() << std::endl;

	int curr_index = 0;

	FindSubsets(curr_index, subsets, items_in_a_path);

	for(int i = 0; i < subsets.size(); i++)
	{	
		//std::cout << "hi" << std::endl;

		for(int j =0; j < conditional_items.size(); j++)
		{
			(subsets[i].first).push_back(conditional_items[j]);
		}
	
		frequent_item_sets.push_back(subsets[i]);
	}

}


void CreateConditionalPatternBase(const int row, const std::vector<header_table_row> &  header_table,
 const FPTree & FPTree, std::vector<std::unordered_set<int>> & conditional_pattern_base)
{
	std::shared_ptr<const FPNode> address = std::get<2>(header_table[row]);

	while(address != nullptr)
	{

		std::shared_ptr<const FPNode> curr_address = address;

		const int num_times =  curr_address->num;

		std::unordered_set<int> item_set;

		curr_address = (curr_address->parent_address).lock();

		while(curr_address != nullptr && (curr_address->parent_address).lock() != nullptr)
		{
			item_set.insert(curr_address->item);
			curr_address = (curr_address->parent_address).lock();
		}

		for(int i = 0; i < num_times; i++)
		{
			conditional_pattern_base.push_back(item_set);
		}
		
		address = address->side_address;
	}
}



bool compare_lexicographic(int a, int b)
{
	std::string A = std::to_string(a);

	std::string B = std::to_string(b);

	return A < B;
}


} //unnamed namespace




void PrintDatabase(const std::vector<std::vector<int>> & given_db)
{
	const int num_transactions =  given_db.size();

	for(int t = 0; t < num_transactions; t++)
	{
		std::cout << "\n Transaction: " << t << std::endl;

		for(int i = 0; i < given_db[t].size(); i++)
		{
			std::cout << given_db[t][i] << "   ";
		}
	}
}



void ReadTransactionalDatabaseFile(const std::string & database_file, std::vector<std::unordered_set<int>> & given_db)
{
	std::fstream file;
	file.open(database_file);
	std::string line;

	while(std::getline(file, line))
	{	
		std::unordered_set<int> items_in_a_transaction;
		
		char* line_ptr =   const_cast<char*>(line.c_str());
		//split line (based on white spaces)
		char* token =  strtok(line_ptr, " ");

		while(token != nullptr)
		{	
			int item = atoi(token);
			items_in_a_transaction.insert(item);

			token = strtok(nullptr, " ");
		}

		given_db.push_back(items_in_a_transaction);
			
	}

	file.close();
}



FPTree::FPTree(std::vector<std::tuple<int ,int , std::shared_ptr<FPNode> >> & header_table,const std::vector<std::vector<int>> & database)
	{
		root_node = std::shared_ptr<FPNode>(new FPNode(-1 , -1 , nullptr, nullptr));

		for(int trans = 0; trans < database.size(); trans++)
		{		
			std::shared_ptr<FPNode> desired_parent_address = root_node;

			for(int i = 0; i < database[trans].size(); i++)
			{	
				const int item = database[trans][i];

				const int row = Get_Suitable_row(header_table, item);

				std::shared_ptr<FPNode> address =  std::get<2>(header_table[row]);

				if(address == nullptr)
				{	
					desired_parent_address->is_leaf = false;
					std::shared_ptr<FPNode> new_node_address{new FPNode(item, 1 , desired_parent_address, nullptr)};
					std::get<2>(header_table[row]) = new_node_address;
					desired_parent_address = new_node_address;
				}
				else
				{
					
					std::shared_ptr<FPNode> prev_address;

					while(address != nullptr && (address->parent_address).lock() != desired_parent_address)
					{	
						prev_address = address;
						address = address->side_address;
					}

					if(address != nullptr)
					{
						address->num = address->num + 1;
						desired_parent_address = address;
					}
					else
					{	
						desired_parent_address->is_leaf = false;
						std::shared_ptr<FPNode> new_node_address{new FPNode(item, 1 , desired_parent_address, nullptr)};
						prev_address->side_address = new_node_address;
						desired_parent_address = new_node_address;
					}

				}
			}
		}

	}



void PrintFrequentItemSets(const std::vector< std::pair<std::vector<int>,int> > &  frequent_item_sets,  const std::string & file_name)
{	

	std::ofstream f_obj(file_name); 
	//f_obj.open();

	for(int i = 0; i < frequent_item_sets.size(); i++)
	{
		std::pair<std::vector<int>,int> itemset_supp_pair =  frequent_item_sets[i];

		std::cout << "\nItemSet: " << std::endl;

		for(int k = 0; k < (itemset_supp_pair.first).size(); k++)
		{
			std::cout << (itemset_supp_pair.first)[k] <<  "  " ;

			f_obj <<  (itemset_supp_pair.first)[k] << " ";
		}

		std::cout << "\nSupport count is: " << itemset_supp_pair.second << std::endl;

		f_obj << ": " << itemset_supp_pair.second  << std::endl;

	}

	f_obj.close();
}





void FPGrowthMineFrequentItemSets(const std::vector<std::unordered_set<int>> & given_db , const std::vector<int> & conditional_items, 
	std::vector< std::pair<std::vector<int>,int> > & frequent_item_sets)
{

	std::pair< std::vector<std::tuple<int ,int , std::shared_ptr<FPNode> >>, std::vector<std::vector<int>> >  header_table_and_reduced_database = CreateHeaderTableAndReducedDataBase(given_db); //use hashmap, where key is item and value is tuple of count and address of FP tree nodes.

	std::vector<std::tuple<int ,int , std::shared_ptr<FPNode> >> header_table =  header_table_and_reduced_database.first;

	std::vector<std::vector<int>> reduced_database = header_table_and_reduced_database.second;


	
 	// std::cout << "\n The database is: " << std::endl;
  //   PrintDatabase(reduced_database);

  //   std::cout << "\n The Header Table is: " << std::endl;

  //   for(int i = 0; i < header_table.size();i++)
  //   {
  //   	std::cout << std::get<0>(header_table[i]) << "    " << std::get<1>(header_table[i]) << std::endl;
  //   }



	FPTree fp_tree(header_table, reduced_database);

	//std::cout << "\n The FP tree is: " << std::endl;
	//PrintFPTree(header_table, fp_tree);

	std::pair<bool, int> check_single =  Check_Single_Path(header_table, fp_tree);



	if(check_single.first == true)
	{	
		//std::cout << "\n Single path found: " << std::endl;
		EnumerateSubsets(header_table, fp_tree, check_single.second, conditional_items, frequent_item_sets);

	} //When the tree is empty, it enters the else part, and basically nothing happens further as the control does not enter the for loop in else part.
	else
	{
		for(int row = header_table.size() - 1; row >= 0 ; row--)
		{	
			std::vector<int> conditional_items_for_row = conditional_items;

			std::vector<std::unordered_set<int>> conditional_pattern_base;

			CreateConditionalPatternBase(row, header_table, fp_tree, conditional_pattern_base);

			conditional_items_for_row.push_back(std::get<0>(header_table[row]));

			const int count = std::get<1>(header_table[row]);

			// conditional_items_for_row with support as count: Add to frequent itemset.
			//DO THIS-->DONE
			std::vector<int> v;
			for(auto itr = conditional_items_for_row.begin(); itr < conditional_items_for_row.end(); itr++)
			{
				v.push_back(*itr);
			}

			frequent_item_sets.push_back(std::pair<std::vector<int>, int>(v, count));

			// std::cout << "\n Pushed pair, support count: " << count << std::endl;

			// for(int i = 0; i < v.size(); i++)
			// {
			// 	std::cout << v[i] << "  ";
			// }

			// std::cout << "\n Done!" << std::endl;

			//std::cout << "\n The row is: " << row << std::endl;

			// std::cout << "\n Conditional items from before: " << std::endl;
			// for(int i =0; i < conditional_items.size(); i++)
			// {
			// 	std::cout << conditional_items[i] << std::endl;
			// }

			// std::cout << "\n Conditional item added now: " << std::endl;

			// std::cout <<   std::get<0>(header_table[row]) << std::endl;			

			FPGrowthMineFrequentItemSets(conditional_pattern_base, conditional_items_for_row, frequent_item_sets);

		}
	}

}




void SortFrequentItemSetsLexicographicOrder(std::vector< std::pair<std::vector<int>,int> > &  frequent_item_sets)
{
	for(int i = 0 ; i < frequent_item_sets.size(); i++)
	{
		std::vector<int> & item_set =  (frequent_item_sets[i]).first;

		//Now sort in lexicographic order
		sort(item_set.begin(), item_set.end(), compare_lexicographic);

	}
}



