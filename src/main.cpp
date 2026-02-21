#include <iostream>
#include <string>
#include "ChainHashTable.h"
#include "OpenHashTable.h"
#include "HashTableTest.h"

int main() {
	
	setlocale(LC_ALL, "Russian");
	std::cout << "Тестирование хэш-таблиц\n";
	std::cout << "-------------------------------------------------\n\n";
	HashTableTest<ChainHashTable<int, std::string>>::comprehensive_test("Chain Hash Table (метод цепочек)");
	std::cout << "-------------------------------------------------\n\n";
	HashTableTest<OpenHashTable<int, std::string>>::comprehensive_test("Open Hash Table (открытая адресация)");
	
	return 0;
}
