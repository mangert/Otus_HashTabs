#pragma once
#include "IHashTable.h"
#include <iostream>
#include <vector>
#include <list>
#include <stdexcept>

template <typename K, typename V> requires HashableKey<K>
class ChainHashTable : public IHashTable<K,V> {	
		
public:
	//----------- Конструкторы -------------------//
	ChainHashTable() = delete;
	explicit ChainHashTable(size_t bucket_count) :	table(bucket_count) {
		
		if (!bucket_count) 
			throw std::invalid_argument("Hash table size must be positive");
	}

	ChainHashTable(const ChainHashTable&) = default;
	ChainHashTable(ChainHashTable&&) = default;
	ChainHashTable& operator=(const ChainHashTable&) = default;
	ChainHashTable& operator=(ChainHashTable&&) = default;
	virtual ~ChainHashTable() = default;

	//---------- Основные операции-------------------//
	//Операции вставки
	bool insert(K key, const V& value) override {
		
		size_t bucket_idx = std::hash<K>{}(key) % table.size(); 
		auto& bucket = table[bucket_idx];

		for (auto& [k, v] : bucket) {
			if (k == key) {
				return false; // Ключ уже есть, вставка не удалась
			}
		}
		
		// Ключа нет - добавляем
		bucket.emplace_back(std::pair<K,V>{ key, value });
		return true;		
	}

	bool insert(K key, V&& value) override {

		size_t bucket_idx = std::hash<K>{}(key) % table.size();
		auto& bucket = table[bucket_idx];

		for (auto& [k, v] : bucket) {
			if (k == key) {
				return false; // Ключ уже есть, вставка не удалась
			}
		}

		// Ключа нет - добавляем
		bucket.emplace_back(std::pair<K, V>{ key, value });
		return true;
	}

	//операции удаления
	bool remove(const K& key) override {
		size_t bucket_idx = std::hash<K>{}(key) % table.size();
		auto& bucket = table[bucket_idx];

		for (auto it = bucket.begin(); it != bucket.end(); ) {
			if (it->first == key) {
				it = bucket.erase(it);
				return true;
			}
			else {
				++it;
			}
		}
		return false;
	}

	//операции доступа и поиска
	bool contains(const K& key) const override {
		size_t bucket_idx = std::hash<K>{}(key) % table.size();
		const std::list<std::pair<K, V>>& bucket = table[bucket_idx];		
		for (auto& [k, v] : bucket) {
			if (k == key) {
				return true;
			}
		}
		return false;
	}

	V* find(const K& key) override {
		
		size_t index = std::hash<K>{}(key) % table.size();		
		for (auto& [k, v] : table[index]) {
			if (k == key) {
				return &v;
			}
		}
		return nullptr;
	}
	
	const V* find(const K& key) const override {
		
		size_t index = std::hash<K>{}(key) % table.size();
		for (auto& [k, v] : table[index]) {
			if (k == key) {
				return &v;
			}
		}
		return nullptr;
	}
	
	
	V& at(const K& key) override {
		
		size_t index = std::hash<K>{}(key) % table.size();
		for (auto& [k, v] : table[index]) {
			if (k == key) {
				return v;
			}
		}
		throw std::out_of_range("Key not found in hash table");
	
	};  
	
	const V& at(const K& key) const override {
		
		size_t index = std::hash<K>{}(key) % table.size();
		for (auto& [k, v] : table[index]) {
			if (k == key) {
				return v;
			}
		}
		throw std::out_of_range("Key not found in hash table");
	}

	// Только для неконстантных объектов
	V& operator[](const K& key) override {
		size_t index = std::hash<K>{}(key) % table.size();
		for (auto& [k, v] : table[index]) {
			if (k == key) {
				return v;
			}
		}		
		auto& new_pair = table[index].emplace_back(key, V{});
		return new_pair.second;
	}

	//очистка
	void clear() override {
		std::vector<std::list<std::pair<K, V>>> new_table(table.size());
		table = std::move(new_table);
	}

	//---------- Рехэширование -------------------//
	void rehash(size_t new_size) override {
		if (new_size < size()) {
			throw std::invalid_argument("rehash: new size too small");
		}
		if (new_size == table.size()) return;

		std::vector<std::list<std::pair<K, V>>> new_table(new_size);

		for (std::list<std::pair<K, V>>& bucket : table) {
			while (!bucket.empty()) {
				K key = std::move(bucket.front().first);
				V value = std::move(bucket.front().second);
				bucket.pop_front();

				size_t index = std::hash<K>{}(key) % new_size;
				new_table[index].emplace_back(std::move(key), std::move(value));
			}
		}

		table = std::move(new_table);
	}

	
	//---------- Характeристики-------------------//
	
	//максимальное число бакетов
	[[nodiscard]] size_t max_bucket_count() const noexcept override { return table.size(); }

	//фактический размер
	size_t size() const noexcept {
		size_t total = 0;
		for (const auto& bucket : table) {
			total += bucket.size();
		}
		return total;
	}

	//проверка на пустоту
	bool empty() const noexcept override { return size() == 0; }

	// Коэффициент заполнения
	double load_factor() const override {
		return static_cast<double>(size()) / table.size();
	}


private:	
	std::vector<std::list<std::pair<K,V>>> table;
};