/* Реализуйте стратегию выбора опорного элемента “случайный элемент”

Функцию Partition реализуйте методом прохода
двумя итераторами от конца массива к началу.

Даны неотрицательные целые числа n,k
и массив целых чисел из [0..10^9] размера n.
Требуется найти k-ю порядковую статистику.
т.е. напечатать число, которое бы стояло на позиции
с индексом k (0..n-1) в отсортированном массиве.
Напишите нерекурсивный алгоритм.
Требования к дополнительной памяти: O(n).
Требуемое среднее время работы: O(n).
Функцию Partition следует реализовывать методом
прохода двумя итераторами в одном направлении. 

In:
10 4
1 2 3 4 5 6 7 8 9 10
Out:
5

In:
10 0
3 6 5 7 2 9 8 10 4 1
Out:
1

In:
10 9
0 0 0 0 0 0 0 0 0 1
Out:
1
*/

#include <iostream>
#include <cstdlib>
#include <ctime>

// Возвращает индекс, на который pivot встанет после разделения.
int partition(int* a, int head, int tail) {
    srand(time(NULL));
    // Выбираем случайный индекс от head до tail
    int p = head + rand() % (tail - head + 1);
    int pivot = a[p];
    std::swap(a[p], a[head]);
    // i указывает на начало группы элементов,
    // не больших опорного (считая с конца)
    int i = tail;
    // j указывает на рассматриваемый элемент
    int j = tail;
    while (j > head) {
        if (a[j] > pivot) {
            std::swap(a[j], a[i]);
            i--;
        }
        j--;
    }
    std::swap(a[i], a[head]);
    return i;
}

// Возвращает k-ю порядковую статистику
// (1-я статистика при k = 0, вторая при k = 1 и т.д.)
int find_kth_order_statistic(int* a, int n, int k) {
    int head = 0; 
    int tail = n - 1;
    int i = 0;
    bool found_k_flag = false;
    while (!found_k_flag) {
        i = partition(a, head, tail);
        if (i == k) {
            // Если индекс совпал, то мы нашли статистику
            found_k_flag = true;
        } else if (i > k) {
            // Обновляем индекс конца части массива, в которой ищем
            tail = i - 1;
        } else {
            // Обновляем индекс начала части массива
            head = i + 1;
        }
    }
    // Статистика на своем месте, возвращаем ее
    return a[k];
}

int main() {
    int n = 0;
    int k = 0;
    std::cin >> n >> k;
    int* a = new int[n];

    for (int i = 0; i < n; i++) {
        std::cin >> a[i];
    }

    std::cout << find_kth_order_statistic(a, n, k);
    delete[] a;
    return 0;
}
