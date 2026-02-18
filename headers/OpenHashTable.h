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
	
	explicit OpenHashTable(size_t size, size_t a = 0, size_t b = 1, double mlf = 0.75l)
		: table(size), M(size), A(a), B(b), max_load_factor(mlf){
		
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
	bool insert(K key, const V& value) override {
		
		constexpr double GROWTH_FACTOR = 1.618l;  //золотое сечение
		
		// Гарантируем, что место есть
		if (load_factor() >= max_load_factor) {
			rehash(M * GROWTH_FACTOR);
		}
		// Собственно вставка
		return insert_impl(std::move(key), value);
	}
		
	bool insert(K key, V&& value) override {//заглушка
		
		if (load_factor() >= max_load_factor) {
			rehash(static_cast<size_t>(M * GROWTH_FACTOR));
		}
		return insert_impl(std::move(key), std::move(value));
	}

	//операции удаления
	bool remove(const K& key) override {//заглушка
		
		return false;
	}

	//операции доступа и поиска
	bool contains(const K& key) const override {
		size_t base_hash = std::hash<K>{}(key) % M;
		for (size_t i = 0; i < M; ++i) {
			const auto& entry = table[probe(base_hash, i)];
			if (entry.is_active() && entry.key == key) {
				return true;
			}
			if (entry.is_empty()) {
				return false;  // Дальше искать бессмысленно
			}			
		}
		return false;
	}

	V* find(const K& key) override {
		size_t base_hash = std::hash<K>{}(key) % M;
		for (size_t i = 0; i < M; ++i) {
			auto& entry = table[probe(base_hash, i)];
			if (entry.is_active() && entry.key == key) {
				return &entry.value;
			}
			if (entry.is_empty()) {
				return nullptr;  // Дальше искать бессмысленно
			}
			// DELETED — продолжаем
		}
		return nullptr;
	}

	const V* find(const K& key) const override {
		size_t base_hash = std::hash<K>{}(key) % M;
		for (size_t i = 0; i < M; ++i) {
			const auto& entry = table[probe(base_hash, i)];
			if (entry.is_active() && entry.key == key) {
				return &entry.value;
			}
			if (entry.is_empty()) {
				return nullptr;
			}
		}
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

	//максимальный коэффициент заполнения
	double get_max_load_factor() const { return max_load_factor; }

private:
	//вспомогательная функция пробинга
	size_t probe(size_t hash, size_t i) const {		
		
		return (hash + i * A + i * i * B) % M;
	}

	//внутренняя реализация вставки
	template<typename VFwd>
	bool insert_impl(K key, VFwd&& value) {
		size_t hash = std::hash<K>{}(key) % M;
		int first_deleted = -1;

		for (size_t i = 0; i < M; ++i) {
			size_t index = probe(hash, i);
			Entry& entry = table[index];

			if (entry.is_active()) {
				if (entry.key == key) return false;
			}
			else if (entry.is_deleted()) {
				if (first_deleted == -1) first_deleted = index;
			}
			else { // EMPTY
				int target = (first_deleted != -1) ? first_deleted : index;
				table[target] = Entry(std::move(key), std::forward<VFwd>(value));
				++element_count;
				return true;
			}
		}

		if (first_deleted != -1) {
			table[first_deleted] = Entry(std::move(key), std::forward<VFwd>(value));
			++element_count;
			return true;
		}

		throw std::runtime_error("Hash table invariant violated");
	}

private:
	enum class EntryState { EMPTY, ACTIVE, DELETED }; //виды состояний

	struct Entry { //структура для данных таблицы
		K key;
		V value;
		EntryState state = EntryState::EMPTY;

		Entry() = default;
		Entry(const K& k, const V& v) : key(k), value(v), state(EntryState::ACTIVE) {}
		Entry(K k, const V& v) : key(std::move(k)), value(v), state(EntryState::ACTIVE) {}
		Entry(K k, V&& v) : key(std::move(k)), value(std::move(v)), state(EntryState::ACTIVE) {}

		bool is_active() const { return state == EntryState::ACTIVE; }
		bool is_empty() const { return state == EntryState::EMPTY; }
		bool is_deleted() const { return state == EntryState::DELETED; }
	};

private:		
	
	std::vector<Entry> table;
	size_t M; //размер таблицы	
	
	//коэффициенты пробинга
	size_t A; //линейный
	size_t B; //квадратичный

	const max_load_factor;

	size_t element_count = 0; //количество "живых" элементов
};
