/* Пятнашки

Написать алгоритм для решения игры в “пятнашки”.
Решением задачи является приведение к виду:
[ 1 2 3 4 ] [ 5 6 7 8 ] [ 9 10 11 12] [ 13 14 15 0 ], где 0 задает пустую ячейку.
Достаточно найти хотя бы какое-то решение. Число перемещений костяшек не обязано быть минимальным.

Формат ввода
Начальная расстановка.

Формат вывода
Если вам удалось найти решение,
то в первой строке файла выведите число перемещений,
которое требуется сделать в вашем решении.
А во второй строке выведите соответствующую последовательность ходов:
L означает, что в результате перемещения костяшка сдвинулась влево,
R – вправо, U – вверх, D – вниз.
Если же выигрышная конфигурация недостижима, то выведите в выходной файл одно число −1.

In:
1 2 3 4
5 6 7 8
9 10 11 0
13 14 15 12

Out:
1
U

*/
#include <assert.h>
#include <algorithm>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>

const size_t HashParameter = 37;

struct position {
  std::vector<char> chips;
  char zero_place;
  // Оценка пройденного расстояния с учетом эвристики
  unsigned int distance = 0;
  // Пройденное количество ребер до текущей позиции
  unsigned int edges_passed = 0;
  
 

  bool operator>(const position& other) const;
  bool operator==(const position& other) const;
  bool operator!=(const position& other) const { return !operator==(other); }
  bool IsFinish() const;
  std::vector<position> Siblings() const;
};

const position FinishPosition{{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0}, 15};

bool position::operator>(const position& other) const {
  return distance > other.distance;
}

bool position::operator==(const position& other) const {
  for (size_t i = 0; i < chips.size(); ++i ) {
    if (chips[i] != other.chips[i]) return false;
  }
  return true;
}

bool position::IsFinish() const {
  return operator==(FinishPosition);
}

std::vector<position> position::Siblings() const {
  std::vector<position> result;
  if (zero_place < 12) {
    position sibling = *this;
    std::swap(sibling.chips[zero_place], sibling.chips[zero_place + 4]);
    sibling.zero_place += 4;
    result.emplace_back(sibling);
  }
  if (zero_place >= 4) {
    position sibling = *this;
    std::swap(sibling.chips[zero_place], sibling.chips[zero_place - 4]);
    sibling.zero_place -= 4;
    result.emplace_back(sibling);
  }
  if (zero_place % 4 != 0) {
    position sibling = *this;
    std::swap(sibling.chips[zero_place], sibling.chips[zero_place - 1]);
    sibling.zero_place -= 1;
    result.emplace_back(sibling);
  }
  if (zero_place % 4 != 3) {
    position sibling = *this;
    std::swap(sibling.chips[zero_place], sibling.chips[zero_place + 1]);
    sibling.zero_place += 1;
    result.emplace_back(sibling);
  }
  return result;
}

namespace std {
  template <>
  struct hash<position> {
    size_t operator()(const position& key) const {
      size_t value = 0;
      for (auto chip : key.chips) {
        value = HashParameter * value + static_cast<size_t>(chip);
      }
      return value;
    }
  };
}

unsigned int CalculateHeuristic (const position& pos) {
  unsigned int acc = 0; 
  for (unsigned int i = 0; i < 16; i++){
    if (pos.chips[i] !=FinishPosition.chips[i]) {
      acc++;
    }
  }
  return 10 * acc;
}

char GetMoveSymbol(const position& from, const position& to) {
  char zero_diff = to.zero_place - from.zero_place;
  switch (zero_diff) {
    case 1:
      return 'L'; // Ноль вправо -> фишка влево
    case -1:
      return 'R';
    case 4:
      return 'U';
    case -4:
      return 'D';
    default:
      assert(false);
  }
  return 0;
}

bool AStar(const position& start, std::unordered_map<position, position>& parents) {
  std::priority_queue<position, std::vector<position>, std::greater<position>> positions_queue;
  positions_queue.push(start);
  while (!positions_queue.empty()) {
    position current = positions_queue.top();
    positions_queue.pop();
    for (auto sibling : current.Siblings()) {
      if (parents.count(sibling)) {
        continue;
      }
      parents[sibling] = current;
      if (sibling.IsFinish()) {
        return true;
      }

      // Обновляем пройденное количество ребер
      sibling.edges_passed = current.edges_passed + 1;
      // Обновляем пройденное расстояние с учетом эвристики
      sibling.distance = sibling.edges_passed + CalculateHeuristic(sibling);

      positions_queue.push(sibling);
    }
  }
  return false;
}

std::vector<char> GetPath(const position& start,
    const std::unordered_map<position, position>& parents) {
  std::vector<char> result;

  position current = FinishPosition;
  while (current != start) {
    const position parent = parents.at(current);
    result.push_back(GetMoveSymbol(parent, current));
    current = parent;
  }
  // Развернем result, так как собирали его с конца.
  std::reverse(result.begin(), result.end());
  return result;
}

// Проверяет расстановку пятнашек на решаемость
bool CheckSolvability(const position& start) {
  unsigned int inversions = 0;
  for (unsigned int i = 0; i < 16; ++i) {
    for (unsigned int j = 0; j < i; ++j) {
      if ((start.chips[j] > start.chips[i]) && (start.chips[i] != 0))
        ++inversions;
    }
  }
  if ((inversions + start.zero_place / 4 + 1) % 2 == 0) {
    return true;
  }
  return false;
}


std::pair<bool, std::vector<char>> SolvePuzzle15(const position& start) {
  if (!CheckSolvability(start)) {
    return std::make_pair(false, std::vector<char>());
  }
  // Запустим алгоритм А*, запоминая предков всех пройденных позиций.
  std::unordered_map<position, position> parents;
  AStar(start, parents);
  return std::make_pair(true, GetPath(start, parents));
}



int main() {
  // Считываем расстановку пятнашек
  std::vector<char> chips;
  for (unsigned i = 0; i < 16; i++) {
    unsigned int chip;
    std::cin >> chip;
    chips.push_back(chip);
  }
  char zero_place = std::find (chips.begin(), chips.end(), 0) - chips.begin();
  position start{chips, zero_place};

  const auto result = SolvePuzzle15(start);

  // Если пятнашки решаемы, выводим результат
  if (result.first) {
    std::cout << result.second.size()<< std::endl;
    for (const auto move : result.second) {
      std::cout << move;
    }
  // Иначе выводим -1
  } else {
    std::cout << -1;
  }

  return 0;
}
