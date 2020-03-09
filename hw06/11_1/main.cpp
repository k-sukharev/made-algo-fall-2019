/* Цикл минимальной длины

Дан невзвешенный неориентированный граф.
Найдите цикл минимальной длины.
Ввод: v: кол-во вершин (макс. 50000),
      n: кол-во ребер (макс. 200000),
      n пар реберных вершин.
Вывод: одно целое число равное длине минимального цикла.
Если цикла нет, то вывести -1.

in:
6
6
0 1
0 2
2 3
2 4
3 5
4 5

out:
4
*/

#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>

class Graph {
  public:
    explicit Graph(size_t vertices_number);
    void add_edge(std::pair<unsigned int, unsigned int>& edge);
    int find_min_loop_length() const;

  private:
    struct Vertex {
      std::vector<unsigned int>  adjacent_vertices;
    };
    std::vector<Vertex> vertices;
    unsigned int find_loop_length(unsigned int loop_vertex) const;
};

Graph::Graph(size_t vertices_number) : vertices(vertices_number) {}

void Graph::add_edge(std::pair<unsigned int, unsigned int>& edge) {
  vertices[edge.first].adjacent_vertices.push_back(edge.second);
  vertices[edge.second].adjacent_vertices.push_back(edge.first);
}

// Находит длину цикла по вершине в этом цикле
unsigned int Graph::find_loop_length(unsigned int loop_vertex) const {
  std::vector<bool> visited(vertices.size());
  std::vector<unsigned int> distance(vertices.size() + 1);
  
  // Храним индексы вершин, откуда пришли
  std::unordered_map<unsigned int, unsigned int>  current_to_previous_index;

  // Обходим в ширину, т.к. начали с узла в составе цикла, то можем найти его длину
  std::queue<unsigned int> next_vertices_idxs;
  next_vertices_idxs.push(loop_vertex);
  while (!next_vertices_idxs.empty()) {
    unsigned int current_idx = next_vertices_idxs.front();
    next_vertices_idxs.pop();
    // Пройденное расстояние из начальной вершины равно нулю
    if (current_idx == loop_vertex) {
      distance[current_idx] = 0;
    // Для остальных вершин расстояние равно расстоянию предыдущей + 1
    } else {
      distance[current_idx] = distance[current_to_previous_index[current_idx]] + 1;
    }
    visited[current_idx] = true;
    // Добавляем прилежащие вершины для текущей в очередь
    for (auto adjacent_vertex_idx : vertices[current_idx].adjacent_vertices) {
      if (visited[adjacent_vertex_idx]) {
        // Пропускае ту вершину, откуда пришли
        if (adjacent_vertex_idx == current_to_previous_index[current_idx]) {
          continue;
        // Нашли еще одну посещенную вершину, возращаем сумму расстояний до них
        // + 1 ребро между ними 
        } else {
          return distance[current_idx] + distance[adjacent_vertex_idx] + 1;
        }
      } else {
          if (!current_to_previous_index.count(adjacent_vertex_idx)) {
            next_vertices_idxs.push(adjacent_vertex_idx);
            current_to_previous_index[adjacent_vertex_idx] = current_idx;
          }
      }
    }
  }
}

// Находит минимальный цикл в графе
int Graph::find_min_loop_length() const {
  std::vector<bool> visited(vertices.size());
  int min_loop_length = -1;

  // Будем запоминать вершины в составе циклов в loop_vertices
  std::vector<unsigned int> loop_vertices;
  std::unordered_map<unsigned int, unsigned int>  current_to_previous_index;
  
  // Обход в ширину, ищем вершины в составе циклов
  std::queue<unsigned int> next_vertices_idxs;
  for (unsigned int next_idx = 0; next_idx < vertices.size(); next_idx++) {
    if (!visited[next_idx]) {
      next_vertices_idxs.push(next_idx);
    }

    while (!next_vertices_idxs.empty()) {
      unsigned int current_idx = next_vertices_idxs.front();
      next_vertices_idxs.pop();

      visited[current_idx] = true;
      for (auto adjacent_vertex_idx : vertices[current_idx].adjacent_vertices) {
        if (visited[adjacent_vertex_idx]) {
          if (adjacent_vertex_idx == current_to_previous_index[current_idx]) {
            continue;
          } else {
            // Нашли вершину в цикле
            loop_vertices.push_back(adjacent_vertex_idx);
          }
        } else {
          if (!current_to_previous_index.count(adjacent_vertex_idx)) {
            next_vertices_idxs.push(adjacent_vertex_idx);
            current_to_previous_index[adjacent_vertex_idx] = current_idx;
          }
        }
      }
    }
  }

  // Перебираем найденные вершины в циклах, ищем минимальный
  for (auto loop_vertex : loop_vertices){
    int loop_length = find_loop_length(loop_vertex);
    // Если нашли цикл, равный 3, то возвращаем. Меньше быть не может.
    if (loop_length == 3) {
      return loop_length;
    }
    // Обновляем минимальную длину цикла в графе
    if ((min_loop_length == -1) || (min_loop_length > loop_length)) {
      min_loop_length = loop_length;
    }
  }

  return min_loop_length;
}


int main() {
  // Читаем количество вершин и рёбер
  unsigned int v, n;
  std::cin >> v >> n;

  Graph graph(v);

  // Читаем пары реберных вершин, добавлем в граф
  std::pair<unsigned int, unsigned int> edge;
  for (unsigned int i = 0; i < n; i++) {
    std::cin >> edge.first >> edge.second;
    graph.add_edge(edge);
  }

  std::cout << graph.find_min_loop_length();

  return 0;
}
