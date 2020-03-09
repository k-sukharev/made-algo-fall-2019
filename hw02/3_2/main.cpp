/* Сортировка почти упорядоченной последовательности

Дана последовательность целых чисел a1...an и натуральное число k,
такое что для любых i, j: если j >= i + k, то a[i] <= a[j].
Требуется отсортировать последовательность.
Последовательность может быть очень длинной. Время работы O(n * log(k)).
Память O(k). Использовать слияние.
In:
10 4
0 4 3 2 1 8 7 6 5 9

Out:
0 1 2 3 4 5 6 7 8 9
*/
#include <assert.h>
#include <string.h>
#include <iostream>

void Merge(int* first_arr, int firstLen, int* second_arr, int secondLen,
           int *c, bool (*compare)(int first, int second)) {
    int i = 0;
    int j = 0;
    for (int m = 0; m < firstLen + secondLen; m++) {
        // Если второй массив закончился или же в первом массиве есть элементы
        // и при этом элемент из первого массива меньше
        if ((j == secondLen) || (i < firstLen) &&
            (compare(first_arr[i], second_arr[j]))) {
            c[m] = first_arr[i];
            i++;
        } else {
            c[m] = second_arr[j];
            j++;
        }
    }
}

void MergeSort(int* a, int aLen, bool (*compare)(int first, int second)) {
    if (aLen <= 1) {
        return;
    }
    int firstLen = aLen / 2;
    MergeSort(a, firstLen, compare);
    int secondLen = aLen - firstLen;
    MergeSort(a + firstLen, secondLen, compare);
    int* c = new int[aLen];
    Merge(a, firstLen, a + firstLen, secondLen, c, compare);
    memcpy(a, c, sizeof(int) * aLen);
    delete[] c;
}

bool CompareLess(int first, int second) {
  return first < second;
}



int main() {
    int n = 0;
    int k = 0;
    std::cin >> n >> k;
    int* arr = new int[k * 2];
    // Если количество элементов меньше или равно 2k, то считываем, сортируем
    // и выводим все сразу
    if (n <= k * 2) {
        for (int i = 0; i < n; i++) {
            std::cin >> arr[i];
        }
        MergeSort(arr, n, CompareLess);
        for (int i = 0; i < n; i++) {
            std::cout << arr[i] << " ";
        }

    // Если элементов больше 2k, то считываем, сортируем и выводим по частям
    } else {
        // Сначала считываем 2k чисел
        for (int i = 0; i < k * 2; i++) {
            std::cin >> arr[i];
        }
        MergeSort(arr, k * 2, CompareLess);
        // Выводим k отсортированных чисел
        // и запоминаем, что k у нас остается в массиве
        for (int i = 0; i < k; i++) {
            std::cout << arr[i] << " ";
        }
        // В цикле читаем и выводим по k чисел, при этом сортируем 2k чисел
        // Повторяем (n/k)-2 раз, т.к. 2k чисел считали в начале.
        for (int j = 0; j < (n / k) - 2; j++) {
            for (int i = 0; i < k; i++) {
                std::cin >> arr[i];
            }
            MergeSort(arr, k * 2, CompareLess);
            for (int i = 0; i < k; i++) {
                std::cout << arr[i] << " ";
            }
        }
        // Читаем оставшиеся n % k чисел
        for (int i = k - n % k; i < k; i++) {
            // Записываем их вплотную к последним k чисел
            std::cin >> arr[i];
        }
        MergeSort(&arr[k - n % k], n % k + k, CompareLess);
        // Выводим n % k + k (те самые k, которые оставались в массиве)
        for (int i = k - n % k; i < 2 * k; i++) {
            std::cout << arr[i] << " ";
        }
    }
    delete[] arr;
    return 0;
}
