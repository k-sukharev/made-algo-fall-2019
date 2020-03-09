/* MST

Дан неориентированный связный граф.
Требуется найти вес минимального остовного дерева в этом графе.
Вариант 1. С помощью алгоритма Прима.

Формат ввода:
Первая строка содержит два натуральных числа
n и m — количество вершин и ребер графа соответственно
(1 ≤ n ≤ 20000, 0 ≤ m ≤ 100000). 
Следующие m строк содержат описание ребер по одному на строке. 
Ребро номер i описывается тремя натуральными числами
bi, ei и wi — номера концов ребра и его вес соответственно
(1 ≤ bi, ei ≤ n, 0 ≤ wi ≤ 100000).

Формат вывода:
Выведите единственное целое число - вес минимального остовного дерева.

In:
4 4
1 2 1
2 3 2
3 4 5
4 1 4

Out:
7

In:
5 10
4 3 3046
4 5 90110
5 1 57786
3 2 28280
4 3 18010
4 5 61367
4 1 18811
4 2 69898
3 5 72518
3 1 85838

Out:
107923

*/

#include <iostream>
#include <queue>
#include <vector>
#include <limits>

struct Edge {
  // Индекс вершины, к которой ведет ребро
  unsigned int to;
  unsigned int weight;
  bool operator>(const Edge& other) const;
};

bool Edge::operator>(const Edge& other) const {
  return weight > other.weight;
}

struct Vertex {
  std::vector<Edge> edges;
};

struct Graph {
  Graph(size_t initial_size) : vertices(initial_size) {};
  std::vector<Vertex> vertices;
};

unsigned int find_mst_weight(const Graph& graph) {
  std::vector<unsigned int> distance_from_mst_to_vertex(graph.vertices.size(), std::numeric_limits<unsigned int>::max());
  std::vector<unsigned int> used_in_mst(graph.vertices.size());
  unsigned int graph_weight = 0;
   // В очереди хранятся следующие вершины
   // или же ребра к следующим вершинам, что тоже самое
   // (второе позволяет переиспользовать структуру Edge)
  std::priority_queue<Edge, std::deque<Edge>, std::greater<Edge>> edges_to_next_vertices_queue;

  // Находим вес MST, используя алгоритм Прима
  // Само минимальное остовное дерево при этом не сохраняем

  // Добавляем в очередь фиктивное ребро к нулевой вершине
  edges_to_next_vertices_queue.push(Edge{0, 0});
  while (!edges_to_next_vertices_queue.empty()) {
    Edge edge = edges_to_next_vertices_queue.top();
    edges_to_next_vertices_queue.pop(); 
    // Если вес ребра меньше или равен расстоянию от вершины MST до вершины в ребре
    if (edge.weight <= distance_from_mst_to_vertex[edge.to]) {
      used_in_mst[edge.to] = true;
      graph_weight += edge.weight;
      // Проходимся по всем ребрам вершины и добавляем их в очередь
      for (auto next_edge : graph.vertices[edge.to].edges) {
        // Пропускаем ребра, ведущие к вершинам в MST и ребра,
        // вес которых больше сохраненного расстояния
        if (!used_in_mst[next_edge.to] &&
            (next_edge.weight < distance_from_mst_to_vertex[next_edge.to])) {
          distance_from_mst_to_vertex[next_edge.to] = next_edge.weight;
          edges_to_next_vertices_queue.push(next_edge);
        } 
      }
    }
  }
  return graph_weight;
}

int main() {
    unsigned int n, m;
    std::cin >> n >> m;
    Graph graph(n);

    unsigned int vertex1, vertex2, weight;
    for (unsigned int i = 0; i < m; i++) {
      // Отнимаем 1, так как нумерация в векторах с нуля
      std::cin >> vertex1 >> vertex2 >> weight;
      vertex1--;
      vertex2--;
      graph.vertices[vertex1].edges.push_back(Edge{vertex2, weight});
      graph.vertices[vertex2].edges.push_back(Edge{vertex1, weight});
    }

    std::cout << find_mst_weight(graph);
    return 0;
}
