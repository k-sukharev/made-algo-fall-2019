/* Поиск подстроки

Найдите все вхождения шаблона в строку.
Длина шаблона – p, длина строки ­– n.
Время O(n + p), доп. память – O(p).

Вариант 1. С помощью префикс-функции (алгоритм Кнута-Морриса-Пратта).

Формат входного файла:
Шаблон, символ перевода строки, строка.

Формат выходного файла:
Позиции вхождения шаблона в строке.

In:
aba
abacababa

Out:
0 4 6
*/

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// Вычисляет префикс функцию для строки
std::vector<unsigned int> calculate_prefix_function(const std::string& str) {
  std::vector<unsigned int> pi(str.size());
  for (unsigned int i = 1; i < str.size(); i++) {
    unsigned int k = pi[i - 1];
    while ((k > 0) && (str[i] != str[k])) {
      k = pi[k - 1];
    }
    if (str[i] == str[k]) {
      k++;
    }
      pi[i] = k;
  }
  return pi;
}


// Принимает шаблон и ifstream со строкой, в которой будем искать
std::vector<unsigned int> kmp_find(const std::string& pattern, std::ifstream& input_file) {
  std::vector<unsigned int> pattern_pi = calculate_prefix_function(pattern);
  std::vector<unsigned int> occurrences;
  
  char current_symbol = 0;
  unsigned int current_symbol_idx = 0; // Индекс последнего прочитанного символа в строке
  unsigned int pattern_idx = 0;
  // Считываем посимвольно в current_symbol, пока не дойдем до конца
  while (input_file.get(current_symbol)) {
    while (true) {
      if (current_symbol != pattern[pattern_idx]) {
        // Выходим из цикла, если текущий индекс в шаблоне 0
        if (pattern_idx == 0) {
          break;
        }
        pattern_idx = pattern_pi[pattern_idx - 1];
      // Иначе, если  последний прочитанный символ
      // совпадает с лежащим по индексу в шаблоне
      } else {
        pattern_idx++;
        // Если индекс в шаблоне равен размеру шаблона
        if (pattern_idx == pattern.size()) {
          occurrences.push_back(current_symbol_idx + 1 - pattern.size());
          pattern_idx = pattern_pi[pattern_idx - 1];
        }
        break;
      }
    }
    // Увеличиваем счетчик прочитанных символов
    current_symbol_idx++;
  }
  return occurrences;
}


int main() {
  std::ifstream input_file;
  input_file.open("input.txt");

  // Считываем шаблон из файла
  std::string pattern;
  char ch = 0;
  input_file.get(ch);
  while (ch != '\n') {
    pattern += ch;
    input_file.get(ch);
  }
  
  // Находим вхождения шаблона в строку
  std::vector<unsigned int> occurrences = kmp_find(pattern, input_file);
  
  // Выводим результат
  for (auto idx : occurrences) {
    std::cout << idx << " ";
  }

  input_file.close();

  return 0;
}
