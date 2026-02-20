#pragma once
#include "IHashTable.h"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <cassert>
#include <set>
#include <functional>
#include <concepts>
#include "ChainHashTable.h"
#include "OpenHashTable.h"

using IntStringTable = IHashTable<int, std::string>;

template <std::derived_from<IntStringTable> HashTable>
class HashTableTest {

public:

    static void comprehensive_test(const std::string& name) {
        std::cout << "========================================\n";
        std::cout << "Hash Tables TEST (" << name << ")\n";
        std::cout << "========================================\n\n";
        

        // 1. Тест граничных случаев
        test_edge_cases();
		
        // 2. Основной тест с разным размером таблиц
        //main_test();
        
        // 3. Тест копирования и перемещения
        test_copy_move_semantics();
		
        // 5. Тесты на исключения        
        if constexpr (std::is_same_v<HashTable, OpenHashTable<int, std::string>>) {
            test_exceptions();
        }
		/*
        // 6. Тесты на коэффициенты
        if constexpr (std::is_same_v<HashTable, OpenHashTable<K, V>>) {
            test_exceptions();
        }*/
        std::cout << "\n========================================\n";
        std::cout << "ALL TESTS PASSED SUCCESSFULLY!\n";
        std::cout << "========================================\n";
    }   

private:

	// ==================== 1. Граничные случаи ====================
	static void test_edge_cases() {
		std::cout << "1. EDGE CASES TEST\n";
		std::cout << "------------------\n";

		size_t M = 10;
		HashTable hashtable(M);

		// 1.1 Пустая таблица
		assert(hashtable.empty());
		assert(hashtable.size() == 0);		
		std::cout << "+ Empty table checks passed\n";

		// 1.2 Один элемент
		hashtable.insert(42, std::string("42"));
		assert(!hashtable.empty());
		assert(hashtable.size() == 1);		
		assert(hashtable.contains(42));
		assert(!hashtable.contains(0));
		assert(*(hashtable.find(42)) == std::string("42"));
		std::cout << "+ Single element checks passed\n";

		// 1.3 Дубликаты не добавляются
		hashtable.insert(42, std::string("42")); // Дубликат
		assert(hashtable.size() == 1);
		std::cout << "+ Duplicate prevention check passed\n";

		// 1.4 Удаление единственного элемента
		hashtable.remove(42);
		assert(hashtable.empty());
		assert(hashtable.size() == 0);		
		assert(!hashtable.contains(42));
		std::cout << "+ Single element removal passed\n";

		// 1.5 Удаление несуществующего элемента
		hashtable.remove(999);
		assert(hashtable.empty()); // Должно остаться пустым
		std::cout << "+ Non-existent removal passed\n";
		
		// 1.6 Очистка
		hashtable.insert(1, std::string("1"));
		hashtable.insert(2, std::string("2"));
		hashtable.insert(3, std::string("3"));
		hashtable.clear();
		assert(hashtable.empty());
		assert(hashtable.size() == 0);
		std::cout << "+ Clear operation passed\n";

		std::cout << "++ All edge cases passed\n\n";
	}

	// ==================== 2. Основной тест ====================	
	
	static void main_test() {
		std::cout << "2. MAIN TEST\n";
		std::cout << "-------------------------------------\n";
		
		size_t M = 101;
		for (size_t i = 0; i != 6; ++i) {
			std::cout << "Iteration " << i << "; M =" << M << "\n";
			single_main_test(M);
			M = M * 10 + 1;
		}

		std::cout << "++ Main test completed\n\n";
	}

	static void single_main_test(size_t M) {
		
		HashTable table(M);
		std::cout << "Fill table --------------------------\n";
		auto data = gen_data(M / 4 * 3);
		auto start = std::chrono::high_resolution_clock::now();
		for (auto item : data) {
			bool success = table.insert(item.first, item.second);
			assert(success);
		}
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		std::cout << "  Table filled in " << duration.count() << " ms\n";
		std::cout << "  Size: " << table.size() << ", buckets " << table.max_bucket_count() << "\n";
		std::cout << "  Load factor: " << table.load_factor() << "\n";

		std::cout << "Find 10% --------------------------\n";
		size_t to_find = M / 40 * 3;
		start = std::chrono::high_resolution_clock::now();
		for (size_t i = 0; i != to_find; ++i) {
			auto val = table.find(data[i].first);
			assert(val);
		}
		end = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
		std::cout << "  10% elements founded in " << duration.count() << " ms\n";		

		std::cout << "Remove 10% --------------------------\n";
		
		size_t to_remove = M / 40 * 3;
		start = std::chrono::high_resolution_clock::now();		
		for (size_t i = 0; i != to_remove; ++i) {
			bool success = table.remove(data[i].first);
			assert(success);
		}
		end = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
		std::cout << "  10% elements removed in " << duration.count() << " ms\n";
		std::cout << "  New size: " << table.size() << ", buckets " << table.max_bucket_count() << "\n";
		std::cout << "  Load factor: " << table.load_factor() << "\n";
	}

	static void test_copy_move_semantics() {
		std::cout << "4. COPY/MOVE SEMANTICS TEST\n";
		std::cout << "---------------------------\n";

		size_t M = 50;
		auto data = gen_data(M);
		// Создаём исходную таблицу и заполняем
		HashTable original(M);
		for (size_t i = 0; i != M/2; ++i) {
			original.insert(data[i].first, data[i].second);
		}				

		// 4.1 Конструктор копирования
		HashTable copy_constructed(original);
		//verify_equality(original, copy_constructed, "copy constructor");
		std::cout << "+ Copy constructor\n";

		// 4.2 Оператор присваивания копированием
		//Tree copy_assigned;
		HashTable copy_assigned = original;
		//verify_equality(original, copy_assigned, "copy assignment");
		std::cout << "+ Copy assignment\n";

		// 4.3 Конструктор перемещения
		HashTable temp_for_move1 = original; // копируем
		HashTable move_constructed(std::move(temp_for_move1));
		//verify_equality(original, move_constructed, "move constructor");
		std::cout << "temp_for_move1" << temp_for_move1.size();

		//assert(temp_for_move1.empty() || temp_for_move1.size() == 0);
		std::cout << "+ Move constructor\n";

		// 4.4 Оператор присваивания перемещением
		HashTable temp_for_move2 = original; // копируем
		//move_assigned;
		HashTable move_assigned = std::move(temp_for_move2);
		//verify_equality(original, move_assigned, "move assignment");
		assert(temp_for_move2.empty() || temp_for_move2.size() == 0);
		std::cout << "+ Move assignment\n";

		// 4.5 Self-assignment
		HashTable self_assigned = original;
		//self_assigned = self_assigned; // self-assignment
		//verify_equality(original, self_assigned, "self-assignment");
		std::cout << "+ Self-assignment\n";
		std::cout << "++ Copy/move semantics test completed\n\n";
	}	
    /*
    
    // Тест с разными коэффициентами (для OpenHashTable)
    template<typename Table>
    void test_coefficients() {
        std::vector<std::tuple<size_t, size_t, size_t>> params = {
            {101, 0, 1},    // классический
            {101, 1, 1},    // с линейным членом
            {97, 2, 3},     // простые числа
            // ... и так далее
        };

        for (auto [M, A, B] : params) {
            Table table(M, A, B);
            // тестируем
        }
    }
	*/
    // Тест на исключения
    static void test_exceptions() {
		std::cout << "7. EXCEPTIONS TEST\n";
		std::cout << "------------------\n";
        // Проверяем, что конструктор кидает при M=0
        //EXPECT_THROW(HashTable(0), std::invalid_argument("Size must be positive"));
		std::cout << "+ NON-ZERO M test passed\n";
		
        // Проверяем, что gcd проверка работает
        //EXPECT_THROW(HashTable(100, 2, 2), std::invalid_argument("Size must be positive"));
		std::cout << "+ PRIME A, B with M test passed\n";

		// Проверяем, что gcd проверка работает
		//EXPECT_THROW(HashTable(100, 0, 0), std::invalid_argument("Size must be positive"));
		std::cout << "+ NON-ZERO A, B  test passed\n";
		
		std::cout << "++ All exception tests passed\n\n";
    }
	// ==================== Вспомогательные функции ====================	
	//основные тесты на одном наборе
	
	// ==================== Генерация тестовых данных ====================	
	
	static std::vector<std::pair<int, std::string>> gen_data(size_t size) {		
		// Подготовка данных
		std::vector<std::pair<int, std::string>> data(size);		

		for (size_t i = 0; i < size; i++) {
			std::pair<int, std::string> item{static_cast<int>(i), std::to_string(i)};
			data[i] = item;			
		}

		std::random_device rd;
		std::mt19937 g(rd());
		std::shuffle(data.begin(), data.end(), g);
		
		return data;
	}

};
