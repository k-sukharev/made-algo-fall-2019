/* Построение выпуклой оболочки

Дано множество точек на плоскости (x, y).
Постройте выпуклую оболочку этого множества и вычислите ее периметр.

Вариант 2. С помощью алгоритма Джарвиса.

Обратите внимание: три и более точки из множества могут лежать на одной прямой.

Формат входного файла:
Количество точек, далее на каждой строке координаты точек x, y.
Общее количество точек не превосходит 100000.
Координаты точек - числа с плавающей точкой в диапазоне [-1000, 1000].

Формат выходного файла:
Выведите периметр выпуклой оболочки.


In:
3
1. 1.
2. 1.
0. 0.

Out:
4.65028154
*/

#include <iostream>
#include <vector>
#include <cmath>

// Константа для учета погрешностей вычислений при сравнениях
double EPS=1e-6;

// Точка
struct Point {
  double x;
  double y;
};

// Структура для вектора между двумя точками
struct Vector {
  Vector(const Point& p, const Point& p0);

  double x;
  double y;
  double length;
};

Vector::Vector(const Point& p, const Point& p0) {
    x = p.x - p0.x;
    y = p.y - p0.y;
    length = sqrt(pow(x, 2) + pow(y, 2));
  }

// Строит выпуклую оболочку для набора точек с помощью алгоритма Джарвиса
std::vector<Point> construct_convex_hull(std::vector<Point>& points, double eps) {
  std::vector<Point> convex_hull;
  std::vector<bool> used(points.size());
  // Найдем индекс точки с минимальной координатой по оси y,
  // (и максимальной по оси x, если минимальная координата y
  //  совпадет у нескольких точек)
  unsigned int min_y_point_idx = 0;
  for (unsigned int i = 0; i < points.size(); i++) {
    double diff_y = points[min_y_point_idx].y - points[i].y;
    if (diff_y > eps) {
      min_y_point_idx = i;
    // Если координата по оси y примерно равна минимальной,
    // запоминаем индекс точки с большей координатой x
    } else if (diff_y < eps && diff_y > -eps) {
      double diff_x = points[i].x - points[min_y_point_idx].x;
      if (diff_x > 0) {
        min_y_point_idx = i;
      }
    }
  }

  unsigned int next_hull_point_idx = min_y_point_idx;
  do {
    // Добавляем найденную точку с предыдущей итерации
    convex_hull.push_back(points[next_hull_point_idx]);
    used[next_hull_point_idx] = true;

    for (unsigned int i = 0; i < points.size(); i++) {
      // Если точка уже используется в выпуклой оболочке, то пропускаем ее 
      // (исключение - начальная точка, чтобы выйти из цикла)
      if (used[i] && (i != min_y_point_idx)) {
        continue;
      }
      Vector v1(points[i], convex_hull.back());
      Vector v2(points[next_hull_point_idx], convex_hull.back());
      // Считаем векторное произведение
      double cross_product = v1.x * v2.y - v2.x * v1.y;
      // Если векторное произведение больше нуля,
      // сохраняем индекс точки, чтобы добавить в оболочку 
      if (cross_product > eps) {
        next_hull_point_idx = i;
      // Если векторное произведение примерно равно нулю и длина нового вектора больше,
      // тоже сохраняем индекс точки
      } else if (cross_product < eps && cross_product > -eps && v1.length > v2.length) {
        next_hull_point_idx = i;
      }
    }
  }
  // Пока не дойдем до точки с которой начали
  while (next_hull_point_idx != min_y_point_idx);

  return convex_hull;
}

// Принимает vector с точками многоугольника в порядке их соединения, возвращает периметр
double calculate_perimeter(std::vector<Point>& polygon) {
  double perimeter = 0;
  Point previous_point = polygon[0];

  for (unsigned int i = 1; i < polygon.size(); i++) {
     Point current_point = polygon[i];
    perimeter += Vector(current_point, previous_point).length;
    previous_point = current_point;
  }

  perimeter += Vector(polygon.back(), polygon.front()).length;
  return perimeter;
}

int main() {
  unsigned int n;
  std::cin >> n;

  std::vector<Point> points;
  double x, y;
  for (unsigned int i = 0; i < n; i++) {
    std::cin >> x >> y;
    points.push_back(Point{x, y});
  }

  std::vector<Point> convex_hull = construct_convex_hull(points, EPS);
  std::cout.precision(9);
  std::cout << calculate_perimeter(convex_hull);

  return 0;
}
