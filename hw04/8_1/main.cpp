 /* Реализуйте структуру данных типа “множество строк”
на основе динамической хеш-таблицы с открытой адресацией.

Хранимые строки непустые и состоят из строчных латинских букв.
Хеш-функция строки должна быть реализована с помощью
вычисления значения многочлена методом Горнера.
Начальный размер таблицы должен быть равным 8-ми.
Перехеширование выполняйте при добавлении элементов в случае,
когда коэффициент заполнения таблицы достигает 3/4.
Структура данных должна поддерживать операции добавления строки в множество,
удаления строки из множества и проверки принадлежности данной строки множеству.

1_1. Для разрешения коллизий используйте квадратичное пробирование. i-ая проба
g(k, i)=g(k, i-1) + i (mod m). m - степень двойки.

Формат входных данных
Каждая строка входных данных задает одну операцию над множеством.
Запись операции состоит из типа операции и следующей за ним через пробел строки,
над которой проводится операция.
Тип операции  – один из трех символов:
    +  означает добавление данной строки в множество; 
    -  означает удаление  строки из множества;  
    ?  означает проверку принадлежности данной строки множеству. 
При добавлении элемента в множество НЕ ГАРАНТИРУЕТСЯ,
что он отсутствует в этом множестве.
При удалении элемента из множества НЕ ГАРАНТИРУЕТСЯ,
что он присутствует в этом множестве.

Формат выходных данных
Программа должна вывести для каждой операции одну из двух строк OK или FAIL.
Для операции '?': OK, если элемент присутствует во множестве. FAIL иначе.
Для операции '+': FAIL, если добавляемый элемент уже присутствует во множестве
и потому не может быть добавлен. OK иначе.
Для операции '-': OK, если элемент присутствовал во множестве и успешно удален.
FAIL иначе.

In:
+ hello
+ bye
? bye
+ bye
- bye
? bye
? hello

Out:
OK
OK
OK
FAIL
OK
FAIL
OK
*/

#include <assert.h>
#include <iostream>
#include <string>
#include <vector>


class HashTable {
 public:
  explicit HashTable(size_t initial_size);
  // Проверка наличия ключа в таблице
  bool Has(const std::string& key) const;
  // Добавление ключа в таблицу
  bool Add(const std::string& key);
  // Удаление ключа
  bool Remove(const std::string& key);

 private:
  // Хеш-функция
  int Hash(const char* str) const;
  // Перехеширование
  void RehashTable();
  // Количество записей в таблице
  int entries_number = 0;

  // Запись в таблице
  struct HashTableEntry {
    std::string key = "";
    // Будем хранить сырой хеш
    int raw_hash = 0;
    // Флаг для используемых записей,
    // чтобы знать, когда перестать искать повтор ключа
    bool is_used = false;
    // Флаг для удаленных записей
    bool is_deleted = false;
  };

  std::vector<HashTableEntry> hash_table;
};


HashTable::HashTable(size_t initial_size) : hash_table(initial_size) {}

// Проверка наличия ключа
bool HashTable::Has(const std::string& key) const {
  assert(!key.empty());
  // Вычисляем хеш
  int hash = Hash(&key[0]) % hash_table.size();

  for (int i = 0; i < hash_table.size(); i++) {
    // На каждой итерации обновляем пробу
    hash = (hash + i) % hash_table.size();
    // Если не нашли ключ и наткнулись на неиспользованную запись
    if (!hash_table[hash].is_used) {
      return false;
    }
    // Если нашли ключ и он не помечен удаленным
    if ((hash_table[hash].key == key) && (!hash_table[hash].is_deleted)) {
      return true;
    }
  }
  // Если прошли по всем записям и не нашли, возращаем false
  // но такого не будет, т.к. таблица динамическая
  // с максимальным коэффициентом заполнения 0.75
  return false;
}

// Добавление ключа
bool HashTable::Add(const std::string& key) {
  assert(!key.empty());
  // Запоминаем сырой хеш для перехеширования
  int raw_hash = Hash(&key[0]); 
  int hash = raw_hash % hash_table.size();
  bool found_deleted_entry = false;
  int deleted_idx = 0;

  for (int i = 0; i < hash_table.size(); i++) {
    hash = (hash + i) % hash_table.size();
    // Если наткнулись на неиспользованную запись, выходим из цикла
    if (!hash_table[hash].is_used) {
      break;
    }
    
    if ((hash_table[hash].key == key) && (!hash_table[hash].is_deleted)) {
      return false;
    }
    // Запоминаем первую удаленную запись, чтобы переиспользовать,
    // но из цикла не выходим, чтобы дойти до конца и проверить
    // наличие одинаковых ключей
    if ((hash_table[hash].is_deleted) && (!found_deleted_entry)) {
      found_deleted_entry = true;
      deleted_idx = hash;
    }
  }
  
  // Если встретили удаленную запись - переиспользуем ее
  if (found_deleted_entry) {
    hash = deleted_idx;
    hash_table[hash].is_deleted = false;
  // Иначе записываем в новую
  } else {
    hash_table[hash].is_used = true;
  }
  // Записываем ключ
  hash_table[hash].key = key; 
  // Сохраняем сырой хеш
  hash_table[hash].raw_hash = raw_hash;
  // Увеличиваем счетчик записей в таблице
  entries_number++;
  // Если коэффициент заполнения больше 0.75, перехешируем таблицу
  if (entries_number > hash_table.size() * 0.75) {
    RehashTable();
  }
  return true;
}

// Удаление ключа
bool HashTable::Remove(const std::string& key) {
  assert(!key.empty());
  
  int hash = Hash(&key[0]) % hash_table.size();
  
  for (int i = 0; i < hash_table.size(); i++) {
    hash = (hash + i) % hash_table.size();
  
    if (!hash_table[hash].is_used) {
      break;
    }
    // Если нашли ключ и он не удален, помечаем удаленным,
    // возвращаем true
    if ((hash_table[hash].key == key) && (!hash_table[hash].is_deleted)) {
      hash_table[hash].is_deleted=true;
      // Уменьшаем счетчик записей в таблице
      entries_number--;
      return true;
    }
  }
  // Если не нашли ключ
  return false;
}

// Хеш-функция
int HashTable::Hash(const char* str) const {
  int hash = 0;
  for ( ; *str != 0; ++str ) {
    hash = (hash * 233 + *str) % 262144;
  }
  return hash;
}

// Перехеширование таблицы
void HashTable::RehashTable() {
  // Создаем новый вектор
  std::vector<HashTableEntry> new_table(2 * hash_table.size());
  int hash = 0;
  // Переносим в цикле все записи без флага is_deleted
  for (HashTableEntry entry : hash_table) {

    if (entry.is_used) {
      if (!entry.is_deleted) {
        hash = entry.raw_hash % new_table.size();

        for (int i = 0; i < new_table.size(); i++) {
          hash = (hash + i) % new_table.size();

          if (!new_table[hash].is_used) {
            break;
          }
        }

        new_table[hash] = entry;
      }
    }
  }

  // Обновляем хеш-таблицу
  hash_table = new_table;
}


int main() {
  HashTable hash_table(8);
  char command = ' ';
  std::string value;
  while (std::cin >> command >> value) {
    switch (command) {
      case '?':
        std::cout << (hash_table.Has(value) ? "OK" : "FAIL") << std::endl;
        break;
      case '+':
        std::cout << (hash_table.Add(value) ? "OK" : "FAIL") << std::endl;
        break;
      case '-':
        std::cout << (hash_table.Remove(value) ? "OK" : "FAIL") << std::endl;
        break;
    }
  }
  return 0;
}
