#pragma once
#include <iostream>
#include <vector>
#include <list>
#include <optional>

template<typename T>
concept HashableKey = requires(T a, T b) {
	{ std::hash<T>{}(a) } -> std::same_as<size_t>;
	{ a == b } -> std::same_as<bool>;
};

template <typename K, typename V> requires HashableKey<K>
class ChainHashTable {	
		
public:
	//----------- Конструкторы -------------------//
	ChainHashTable() = delete;
	explicit ChainHashTable(size_t bucket_count) :
		table(bucket_count) {
	}

	ChainHashTable(const ChainHashTable&) = default;
	ChainHashTable(ChainHashTable&&) = default;
	ChainHashTable& operator=(const ChainHashTable&) = default;
	ChainHashTable& operator=(ChainHashTable&&) = default;
	virtual ~ChainHashTable() = default;

	//---------- Основные операции-------------------//
	//Операции вставки
	bool insert(K key, const V& value) {
		
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

	bool insert(K key, V&& value) {

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
	bool remove(const K& key) {
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
	bool contains(const K& key) const {
		size_t bucket_idx = std::hash<K>{}(key) % table.size();
		const std::list<std::pair<K, V>>& bucket = table[bucket_idx];		
		for (auto& [k, v] : bucket) {
			if (k == key) {
				return true;
			}
		}
		return false;
	}

	V* find(const K& key) {
		
		size_t index = std::hash<K>{}(key) % table.size();		
		for (auto& [k, v] : table[index]) {
			if (k == key) {
				return &v;
			}
		}
		return nullptr;
	}
	
	const V* find(const K& key) const {
		
		std::cout << "const find" << std::endl;
		size_t index = std::hash<K>{}(key) % table.size();
		for (auto& [k, v] : table[index]) {
			if (k == key) {
				return &v;
			}
		}
		return nullptr;
	}
	
	/*
	V& at(const K& key);  // throws если нет
	const V& at(const K& key) const;

	V& operator[](const K& key);  // создает если нет*/
	
	//---------- Характeристики-------------------//
	
	[[nodiscard]] size_t max_bucket_count() const noexcept { return table.size(); }


private:	
	std::vector<std::list<std::pair<K,V>>> table;
};

