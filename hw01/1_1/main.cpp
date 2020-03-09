/* Реализовать очередь с динамическим зацикленным буфером

Формат входных данных.
В первой строке количество команд n. n ≤ 1000000.
Каждая команда задаётся как 2 целых числа: a b.
a = 1 - push front
a = 2 - pop front
a = 3 - push back
a = 4 - pop back
Для очереди используются команды 2 и 3. Для дека используются все четыре команды.
Если дана команда pop*, то число b - ожидаемое значение.
Если команда pop вызвана для пустой структуры данных, то ожидается “-1”. 
Формат выходных данных.
Требуется напечатать YES - если все ожидаемые значения совпали.
Иначе, если хотя бы одно ожидание не оправдалось, то напечатать NO.

in:
3
3 44
3 50
2 44

out:
YES

in:
2
2 -1
3 10

out:
YES

in:
2
3 44
2 66

out:
NO
*/

#include <assert.h>
#include <iostream>

class Queue {
  public:
    ~Queue();

    // Проверка очереди на пустоту
    bool empty() const;
    // Добавление элемента
    void push(int value);
    // Извлечение
    int pop();

  private:
    // Начальный размер буфера
    int size = 2;
    // Выделяем память под буфер
    int* buffer = new int[size];
    // У пустой очереди head и tail равны -1
    int head = -1;
    int tail = -1;
    // Увеличение буфера
    void increase_buffer();
};

Queue::~Queue() {
  delete[] buffer;
}

bool Queue::empty() const {
  // Оба указателя равны -1 одновреммено
  assert((head == -1) == (tail == -1));

  return head == -1;
}

void Queue::push(int value) {
  if (empty()) {
    head = tail = 0;
  } else {
      // Если новый элемент (tail + 1) попадёт на head
      if ((tail + 1) % size == head) {
        // увеличиваем буфер
        increase_buffer();
      }
      // Обновляем tail
      tail = (tail + 1) % size;
  }
  buffer[tail] = value;
}

int Queue::pop() {
  // Проверка на пустоту
  assert(!empty());
  int result = buffer[head];
  // Обновим head
  if (head == tail) {
    // Если head оставался один в буфере
    head = tail = -1;
  } else if (head == size-1) {
    // head был последним элементом в буфере, то новый head станет нулевым
    head = 0;
  } else {
    // если head был в середине, то новый head станет следующим
    head++;
  }
  return result;
}

void Queue::increase_buffer() {
  // Выделяем память под новый буфер
  int* new_buffer = new int[size * 2];
  // Если head меньше tail просто переносим элементы очереди в буфере
  if (head < tail) {
    for (int i = head; i <= tail; i++) {
      new_buffer[i] = buffer[i];
    }
  } else {
    // Иначе переносим элементы в начале и в конце новый буфер отдельно
    for (int i = 0; i <= tail; i++) {
      new_buffer[i] = buffer[i];
    }
    for (int i = head; i < size; i++) {
      new_buffer[size+i] = buffer[i];
    }
    // Новый head сдвинется на размер старого буфера
    head += size;
  }
  size *= 2;
  // Удалим старый буфер
  delete[] buffer;
  buffer = new_buffer;
}

int main() {
  int commands_count = 0;
  std::cin >> commands_count;
  // Флаг останется равным true, если все ожидаемые значения совпадут
  bool expected_value_flag = true;
  Queue queue;

  for (int i = 0; i < commands_count; ++i) {
    int command = 0;
    int value = 0;
    std::cin >> command >> value;
    if (command == 3) {
      queue.push(value);
    } else if (command == 2) {
      // Проверка на пустоту, чтобы не вызывать pop для пустой очереди
      if (queue.empty()) {
        if (value != -1) {
          // Если очередь пуста и должны вернуть не -1, флаг будет false
          expected_value_flag = false;
          break;
        }
      } else if (queue.pop() != value) {
        // Если очередь не пуста
        // и значение не совпадет с ожидаемым, флаг будет false
        expected_value_flag = false;
        break;
      }
    }
  }
  if (expected_value_flag) {
      std::cout << "YES";
  } else {
      std::cout << "NO";
  }
  return 0;
}
