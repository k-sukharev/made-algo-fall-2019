/* Выведите элементы в порядке pre-order (сверху вниз)

Дано число N < 10^6 и последовательность целых чисел из [-2^31..2^31] длиной N.
Требуется построить бинарное дерево, заданное наивным порядком вставки.
Т.е., при добавлении очередного числа K в дерево с корнем root, если root→Key ≤ K,
то узел K добавляется в правое поддерево root; иначе в левое поддерево root.
Рекурсия запрещена.
in:
3
2 1 3

out: 
2 1 3

in:
3
1 2 3

out: 
1 2 3

in:
3
3 1 2

out: 
3 1 2

in:
4
3 1 4 2

out: 
3 1 4 2
*/

#include <iostream>
#include <stack>
#include <vector>


// Узел бинарного дерева
struct TreeNode {
  explicit TreeNode(int _value) : value(_value) {}

  int value = 0;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;
};


class Tree {
 public:
  ~Tree();

  std::vector<TreeNode*> PreOrderTraverse() const;

  void Add(int value);

 private:
  TreeNode* root = nullptr;
};


Tree::~Tree() {
  if (!root) return;
  std::stack<TreeNode*> next_nodes;
  // Добавляем корень на стек
  next_nodes.push(root);
  TreeNode* current_node = nullptr;
  // Удаляем узлы, пока стек не опустеет
  while (!next_nodes.empty()) {
    current_node = next_nodes.top();
    next_nodes.pop();
    // Если есть дети, добавляем в стек
    if  (current_node->right != nullptr) {
      next_nodes.push(current_node->right);
    }
    if  (current_node->left != nullptr) {
      next_nodes.push(current_node->left);
    }
    delete current_node;
  }
}


// Возвращает вектор с указателями на узлы дерева в порядке pre-order обхода
std::vector<TreeNode*> Tree::PreOrderTraverse() const {
  std::stack<TreeNode*> next_nodes;
  std::vector<TreeNode*> traversal;
  // Добавляем корень на стек
  next_nodes.push(root);
  // Пока стек не опустеет,
  // забираем указатели на элементы со стека в вектор
  while (!next_nodes.empty()) {
    traversal.push_back(next_nodes.top());
    next_nodes.pop();
    // Если есть дети, добавляем в стек
    if  (traversal.back()->right != nullptr) {
      next_nodes.push(traversal.back()->right);
    }
    if  (traversal.back()->left != nullptr) {
      next_nodes.push(traversal.back()->left);
    }
  }
  return traversal;
}

// Добавление элемента в дерево
void Tree::Add(int value) {
  TreeNode* current_node = root;
  // Если дерево пусто, новый узел в корне
  if (current_node == nullptr) {
    root = new TreeNode(value);
    return;
  }
  // Выполняем цикл, пока не найдем подходящее место для узла
  while (true) {
    // Если значение меньше, чем в текущем узле
    if (value < current_node->value) {
      // Если есть левый ребенок, то он станет текущим узлом
      if (current_node->left) {
        current_node = current_node->left;
      // Иначе, если левого ребенка нет, создаем новый узел
      } else {
        current_node->left = new TreeNode(value);
        return;
      }
    // Иначе, если значение больше или равно значения в текущем узле
    } else {
      // Если есть правый ребенок, то он станет текущим узлом
      if (current_node->right) {
        current_node = current_node->right;
      // Иначе, если правого ребенка нет, создаем новый узел
      } else {
        current_node->right = new TreeNode(value);
        return;
      }
    }
  }
}


int main() {
  int n = 0;
  std::cin >> n;
  Tree tree;
  int inp = 0;
  for (int i = 0; i < n; i++) {
    std::cin >> inp;
    tree.Add(inp);
  }

  for (auto node : tree.PreOrderTraverse()) {
    std::cout << node->value << " ";
  }

  return 0;
}
