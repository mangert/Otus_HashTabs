#pragma once
#include "IHashTable.h"
#include <iostream>
#include <vector>
#include <stdexcept>
#include <numeric>

template <typename K, typename V> requires HashableKey<K>
class OpenHashTable : public IHashTable<K, V> {

public:
	//----------- Конструкторы -------------------//
	OpenHashTable() = delete;
	
	explicit OpenHashTable(size_t size, size_t a = 0, size_t b = 1)
		: table(size), M(size), A(a), B(b)  {
		
		if (M == 0) throw std::invalid_argument("Size must be positive");
		
		if (std::gcd(A, M) != 1 || std::gcd(B, M) != 1) {
			throw std::invalid_argument("A and B must be coprime with M");
		}
	}

	OpenHashTable(const OpenHashTable&) = default;
	OpenHashTable(OpenHashTable&&) = default;
	OpenHashTable& operator=(const OpenHashTable&) = default;
	OpenHashTable& operator=(OpenHashTable&&) = default;
	virtual ~OpenHashTable() = default;

	//---------- Основные операции-------------------//
	//Операции вставки
	bool insert(K key, const V& value) override { //не доделано. Дубликаты?

		size_t iter = 0;
		while (iter != M) {
			
			size_t idx = probe(hash(key), iter);
			
			if (table[idx].is_empty()) {
				//table.emplace(table.begin() + idx, Entry(key, value)); а так можно?
				table[idx] = Entry(key, value);
				++element_count;
				return true;
			}
			++iter;
		}
		/*
		 for(int i=0; i<table_size; i++)
    {
      int Hash = getHash(key, i);
      probing++;
      if (isDelete[Hash] == true)
        continue;
      if (hash[Hash] == -1)
      {
        hash[Hash] = key;
        break;
      }
    }

		*/
		
		return true;
	}

	bool insert(K key, V&& value) override {//заглушка
		
		return true;
	}

	//операции удаления
	bool remove(const K& key) override {//заглушка
		
		return false;
	}

	//операции доступа и поиска
	bool contains(const K& key) const override {//заглушка
		
		return false;
	}

	V* find(const K& key) override {//заглушка
		return nullptr;
	}

	const V* find(const K& key) const override {//заглушка
		
		return nullptr;
	}


	V& at(const K& key) override {//заглушка
		
		V a{};
		return a;

	};

	const V& at(const K& key) const override {//заглушка
		V a{};
		return a;
		
	}

	// Только для неконстантных объектов
	V& operator[](const K& key) override {//заглушка
		V a{};
		return a;
	}

	//очистка
	void clear() override {//заглушка

	}

	//---------- Рехэширование -------------------//
	void rehash(size_t new_size) override {//заглушка
	}


	//---------- Характeристики-------------------//

	//максимальное число бакетов
	[[nodiscard]] size_t max_bucket_count() const noexcept override { return M; }

	//фактический размер
	size_t size() const noexcept {
		
		return element_count;
	}

	//проверка на пустоту
	bool empty() const noexcept override { return size() == 0; }

	// Коэффициент заполнения
	double load_factor() const override {
		return static_cast<double>(element_count / M);
	}
private:	
	enum class EntryState { EMPTY, ACTIVE, DELETED }; //виды состояний
	
	struct Entry { //структура для данных таблицы
		K key;
		V value;
		EntryState state = EntryState::EMPTY;

		Entry() = default;
		Entry(const K& k, const V& v) : key(k), value(v), state(EntryState::ACTIVE) {}
		Entry(K k, V&& v) : key(std::move(k)), value(std::move(v)), state(EntryState::ACTIVE) {}

		bool is_active() const { return state == EntryState::ACTIVE; }
		bool is_empty() const { return state == EntryState::EMPTY; }
		bool is_deleted() const { return state == EntryState::DELETED; }
	};

private:
	//вспомогательная функция пробинга
	size_t probe(size_t hash, size_t i) const {		
		
		return (hash + i * A + i * i * B) % M;
	}

	//вспомогательная функция расчета хэш
	size_t hash(const K& key) const {
		return std::hash<K>{}(key) % M;
	}


private:		
	
	std::vector<Entry> table;
	size_t M; //размер таблицы	
	
	//коэффициенты пробинга
	size_t A; //линейный
	size_t B; //квадратичный

	size_t element_count = 0; //количество "живых" элементов
};
