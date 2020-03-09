"""Быстрое сложение

Для сложения чисел используется старый компьютер.
Время, затрачиваемое на нахождение суммы двух чисел равно их сумме.
Таким образом для нахождения суммы чисел 1,2,3 может потребоваться разное
время, в зависимости от порядка вычислений.
((1+2)+3) -> 1+2 + 3+3 = 9
((1+3)+2) -> 1+3 + 4+2 = 10
((2+3)+1) -> 2+3 + 5+1 = 11
Требуется написать программу, которая определяет минимальное время,
достаточное для вычисления суммы заданного набора чисел.
Формат входных данных. Вначале вводится n - количество чисел.
Затем вводится n строк - значения чисел (значение каждого числа
не превосходит 10^9, сумма всех чисел не превосходит 2*10^9).
Формат выходных данных. Натуральное число - минимальное время.
in:
5
5 2 3 4 6
out:
45

in:
5
3 7 6 1 9
out:
56
"""

class PriorityQueue:
    def __init__(self, lst, how='min'):
        comparator_dict = {'min': lambda x, y: x < y,
                           'max': lambda x, y: x > y}
        self.__comp = comparator_dict[how]

        self._heap = lst
        build_heap(self._heap, self.__comp)

    def __len__(self):
        return len(self._heap)

    def pop(self):
        assert self._heap
        # Result is the root of tree
        result = self._heap.pop(0)
        if self._heap:
            # Last element become first
            self._heap.insert(0, self._heap.pop())
            # Sift down first element
            sift_down(self._heap, 0, self.__comp)
        return result

    def top(self):
        return self._heap[0]

    def push(self, x):
        self._heap.append(x)
        sift_up(self._heap, len(self._heap) - 1, self.__comp)


def build_heap(lst, comp):
    """Transform list to heap (inplace) using comparator"""
    for i in range(n // 2 - 1, -1, -1):
        sift_down(lst, i, comp)


def get_parent_index(i):
    """Returns index of parent node"""
    return max((i + 1) // 2 - 1, 0)


def get_children_index(i):
    """Returns indexes of children nodes (left and right)
    Even if child nodes aren't exist"""
    return 2 * i + 1,  2 * i + 2


def sift_up(lst, i, comp):
    """Sifts up i-th node (inplace) using comparator"""
    p = get_parent_index(i)
    # comp(x, y) by default is the same as x < y
    if comp(lst[i], lst[p]):
        # Swap if comparator is True
        lst[i], lst[p] = lst[p], lst[i]
        sift_up(lst, p, comp)


def sift_down(lst, i, comp):
    """Sifts down i-th node (inplace) using comparator"""
    lc, rc = get_children_index(i)
    if lc < len(lst) and rc < len(lst):
        # If both children exist, choose one
        c = lc if comp(lst[lc], lst[rc]) else rc
    elif lc < len(lst):
        # If one child then there's no choice
        c = lc
    else:
        return
    if comp(lst[c], lst[i]):
        # Swap if comparator is True
        lst[c], lst[i] = lst[i], lst[c]
        sift_down(lst, c, comp)


n = int(input())
nums = [int(i) for i in input().split()]

priority_queue = PriorityQueue(nums)

# If got one number, computation time equals zero
computation_time = 0
# If more, than calculate computation_time
if n >= 2:
    for _ in range(n-1):
        # next smallest possible sum
        s = priority_queue.pop() + priority_queue.pop()
        priority_queue.push(s)
        computation_time += s
print(computation_time)
