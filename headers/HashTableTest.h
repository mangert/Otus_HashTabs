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
        main_test();
        
        // 3. Тест копирования и перемещения
        test_copy_move_semantics();
		
        // 4. Тесты на исключения                
        test_exceptions();        
		
        // 5. Тесты на коэффициенты
        if constexpr (std::is_same_v<HashTable, OpenHashTable<int, std::string>>) {
            test_coefficients();
        }
        std::cout << "\n========================================\n";
        std::cout << "ALL TESTS PASSED SUCCESSFULLY!\n";
        std::cout << "========================================\n";
    }   

private:

	// ==================== 1. Граничные случаи ====================
	static void test_edge_cases() {
		std::cout << "\n1. EDGE CASES TEST\n";
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
		std::cout << "\n2. MAIN TEST\n";
		std::cout << "-------------------------------------\n";
		
		size_t M = 101;
		for (size_t i = 0; i != 6; ++i) {
			std::cout << "Iteration " << i << "; M =" << M << "\n";
			single_main_test(M);
			M = M * 10 + 1;
		}
		std::cout << "++ Main test completed\n\n";
	}	

	static void test_copy_move_semantics() {
		std::cout << "\n3. COPY/MOVE SEMANTICS TEST\n";
		std::cout << "---------------------------\n";

		size_t M = 50;
		auto data = gen_data(M);
		// Создаём исходную таблицу и заполняем
		HashTable original(M);
		for (size_t i = 0; i != M; ++i) {
			original.insert(data[i].first, data[i].second);
		}				

		// 4.1 Конструктор копирования
		HashTable copy_constructed(original);
		verify_equality(original, copy_constructed, data, "copy constructor");
		std::cout << "+ Copy constructor\n";

		// 4.2 Оператор присваивания копированием		
		HashTable copy_assigned = original;
		verify_equality(original, copy_assigned, data, "copy assignment");
		std::cout << "+ Copy assignment\n";

		// 4.3 Конструктор перемещения
		HashTable temp_for_move1 = original; // копируем
		HashTable move_constructed(std::move(temp_for_move1));
		verify_equality(original, move_constructed, data, "move constructor");
		assert(temp_for_move1.empty() || temp_for_move1.size() == 0);
		std::cout << "+ Move constructor\n";

		// 4.4 Оператор присваивания перемещением
		HashTable temp_for_move2 = original; // копируем		
		HashTable move_assigned = std::move(temp_for_move2);
		verify_equality(original, move_assigned, data, "move assignment");
		assert(temp_for_move2.empty() || temp_for_move2.size() == 0);
		std::cout << "+ Move assignment\n";

		// 4.5 Self-assignment
		HashTable self_assigned = original;
		self_assigned = self_assigned; // self-assignment
		verify_equality(original, self_assigned, data, "self assigment");
		std::cout << "+ Self-assignment\n";
		std::cout << "++ Copy/move semantics test completed\n\n";
	}	    
    
    // Тест с разными коэффициентами (для OpenHashTable)    
    static void test_coefficients() {
        std::vector<std::tuple<size_t, size_t, size_t>> params = {
            {950041, 0, 1},    // классический
            {950041, 1, 1},    // с линейным членом
            {950041, 2, 3},     // простые числа            
        };

		std::cout << "\n4. COEFFICIENTS TEST\n";
		std::cout << "-------------------------------------\n";		

        for (auto [M, A, B] : params) {
			std::cout << "\n ***** M = " << M << "; A = " << A << "; B = " << B << "\n";
			single_main_test(M, A, B);
        }

		std::cout << "++ Coefficients test completed\n\n";
    }
	
    // Тест на исключения
	static void test_exceptions() {
		std::cout << "\n5. EXCEPTIONS TEST\n";
		std::cout << "------------------\n";

		// Проверяем, что конструктор кидает при M=0
		try {
			HashTable(0);
			std::cout << "FAILED: Expected std::invalid_argument, but no exception thrown";
		}
		catch (const std::invalid_argument&) {
			std::cout << "+ NON-ZERO M test passed\n";
		}
		catch (...) {
			std::cout << "FAILED: Expected std::invalid_argument, but different exception thrown";
		}

		// Проверка взаимной простоты (только для OpenHashTable)
		if constexpr (std::is_same_v<HashTable, OpenHashTable<int, std::string>>) {
			try {
				OpenHashTable<int, std::string> bad_table(100, 2, 2); // A=2, B=2, M=100 (gcd(2,100)=2)
				std::cout << "FAILED: Expected std::invalid_argument about coprime, but no exception thrown";
			}
			catch (const std::invalid_argument&) {
				std::cout << "+ Coprime check passed\n";
			}
			catch (...) {
				std::cout << "FAILED: Expected std::invalid_argument, but different exception thrown";
			}
			// Проверка на нулевый коэффициенты (только для OpenHashTable)
			try {
				OpenHashTable<int, std::string> bad_table(100, 0, 0); // A, B == 0
				std::cout << "FAILED: Expected std::invalid_argument about zero A B, but no exception thrown";
			}
			catch (const std::invalid_argument&) {
				std::cout << "+ NON ZERO A&B check passed\n";
			}
			catch (...) {
				std::cout << "FAILED: Expected std::invalid_argument, but different exception thrown";
			}
		}
	}    
	// ==================== Вспомогательные функции ====================	
	//функция компплексного теста на единичном наборе данных
	static void single_main_test(size_t M, size_t a = 0, size_t b = 1) {

		HashTable table = [&]() {
			if constexpr (std::is_same_v<HashTable, OpenHashTable<int, std::string>>) {
				return HashTable(M, a, b);
			}
			else {
				return HashTable(M);
			}
			}();

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

		std::cout << "Rehash X2 --------------------------\n";
		
		start = std::chrono::high_resolution_clock::now();		
		table.rehash(M * 2 + 1);
		end = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		std::cout << "  Table have been rehashed in " << duration.count() << " ms\n";
		std::cout << "  New size: " << table.size() << ", buckets " << table.max_bucket_count() << "\n";
		std::cout << "  Load factor: " << table.load_factor() << "\n";
	}
	//проверка скопированных / перемещенных таблиц
	static void verify_equality(const HashTable& t1, const HashTable& t2, 
		const std::vector<std::pair<int, std::string>>& test_data, const std::string& test_name) {

		// 1. Проверяем размер
		if (t1.size() != t2.size()) {
			std::cerr << "FAILED " << test_name << ": sizes differ: "
				<< t1.size() << " vs " << t2.size() << std::endl;
			return;
		}

		// 2. Проверяем empty() согласованность
		if (t1.empty() != t2.empty()) {
			std::cerr << "FAILED " << test_name << ": empty() mismatch" << std::endl;
			return;
		}

		// 3. Проверяем load_factor
		if (std::abs(t1.load_factor() - t2.load_factor()) > 1e-10) {
			std::cerr << "FAILED " << test_name << ": load factors differ: "
				<< t1.load_factor() << " vs " << t2.load_factor() << std::endl;
			return;
		}

		// 4. Проверяем max_bucket_count
		
		if (t1.max_bucket_count() != t2.max_bucket_count()) {
			std::cerr << "FAILED " << test_name << ": max_bucket_count differ: "
			<< t1.max_bucket_count() << " vs " << t2.max_bucket_count() << std::endl;
			return;
		}		

		std::cout << "PASSED " << test_name << " (basic checks)" << std::endl;
		
		// Проверяем все ключи из тестовых данных
		for (const auto& [key, expected_value] : test_data) {
			auto* val1 = t1.find(key);
			auto* val2 = t2.find(key);

			// Проверяем наличие
			if ((val1 == nullptr) != (val2 == nullptr)) {
				std::cerr << "FAILED " << test_name << ": key " << key
					<< " exists in one table but not the other" << std::endl;
				return;
			}

			// Если ключ есть в обеих, проверяем значение
			if (val1 != nullptr && *val1 != *val2) {
				std::cerr << "FAILED " << test_name << ": value mismatch for key " << key
					<< ": '" << *val1 << "' vs '" << *val2 << "'" << std::endl;
				return;
			}
		}		
		std::cout << "PASSED " << test_name << " (full content check)" << std::endl;
	}
	
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
