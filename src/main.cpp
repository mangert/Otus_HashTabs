#include <iostream>
#include <string>
#include "ChainHashTable.h"

int main() {
	
	setlocale(LC_ALL, "Russian");
	
	ChainHashTable<int, std::string> table(100);
	int key = 135;
	std::string value = "dog";	
	std::cout << "---------------" << std::endl;
	table.insert(key, value);
	std::cout << "---------------" << std::endl;
	
	*table.find(key) = "cat";
	std::string found = *table.find(key);
	std::cout << found << std::endl;

	table.remove(key);
	std::cout << table.contains(key) << std::endl;

	
	return 0;
}
