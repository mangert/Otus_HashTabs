#include <iostream>
#include <string>
#include "ChainHashTable.h"
#include "OpenHashTable.h"

int main() {
	
	setlocale(LC_ALL, "Russian");
	
	OpenHashTable<int, std::string> table(101);
	int key = 135;
	std::string value = "dog";	
	std::cout << "---------------" << std::endl;
	table.insert(key, value);
	std::cout << "---------------" << std::endl;
	
	*table.find(key) = "cat";
	std::string found = *table.find(key);
	std::cout << found << std::endl;	

	const OpenHashTable<int, std::string> table2 = table;
	std::string found2 = *(table2.find(key));
	std::cout << found2 << std::endl;
	std::string foundAt = table.at(key);

	std::cout << foundAt << std::endl;
	try {
		table.at(2);
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	std::string found3 = table2.at(key);
	std::cout << found3 << std::endl;

	std::string found4 = table[key];
	std::cout << found4 << std::endl;
	
	table[key] = "dog";
	std::cout << table[key] << std::endl;	

	std::cout << table.size() << std::endl;
	std::cout << table.max_bucket_count() << std::endl;

	table.rehash(200);
	std::cout << table.size() << std::endl;
	std::cout << table.max_bucket_count() << std::endl;
	std::cout << table[key] << std::endl;
	std::cout << "---" << table[14] << std::endl;
	table[14] = "fox";
	std::cout << table[14] << std::endl;
	
	return 0;
}
