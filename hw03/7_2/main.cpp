/* Солдаты
 
В одной военной части решили построить в одну шеренгу по росту.
Т.к. часть была далеко не образцовая, то солдаты часто приходили не вовремя,
а то их и вовсе приходилось выгонять из шеренги за плохо начищенные сапоги.
Однако солдаты в процессе прихода и ухода должны были всегда быть выстроены
по росту – сначала самые высокие, а в конце – самые низкие.
За расстановку солдат отвечал прапорщик,
который заметил интересную особенность – все солдаты в части разного роста.
Ваша задача состоит в том, чтобы помочь прапорщику правильно расставлять солдат,
а именно для каждого приходящего солдата указывать,
перед каким солдатом в строе он должен становится.

Требуемая скорость выполнения команды - O(log n) амортизировано.
Для решения задачи реализуйте сплей дерево.

Формат входных данных.
Первая строка содержит число N – количество команд (1 ≤ N ≤ 30 000).
В каждой следующей строке содержится описание команды:
число 1 и X если солдат приходит в строй (X – рост солдата,
натуральное число до 100 000 включительно) и число 2 и Y если солдата,
стоящим в строе на месте Y надо удалить из строя. Солдаты в строе нумеруются с нуля.

Формат выходных данных.
На каждую команду 1 (добавление в строй) вы должны выводить число K – номер позиции,
на которую должен встать этот солдат (все стоящие за ним двигаются назад).

in:
5
1 100
1 200
1 50
2 1
1 150


out:
0
0
2
1

*/

#include <assert.h>
#include <iostream>
#include <stack>
#include <utility>

// Узел бинарного дерева
struct SplayTreeNode {
  explicit SplayTreeNode(int _value) : value(_value) {}

  int value = 0;
  int count = 1;
  SplayTreeNode* parent = nullptr;
  SplayTreeNode* left = nullptr;
  SplayTreeNode* right = nullptr;
};


// Сплей дерево
class SplayTree {
 public:
  ~SplayTree();

  void Add(int value);
  // Обертка для Add, возвращает место
  int AddReturnPlace(int value);
  // Удаляет солдата с места k
  void RemovePlace(int k);

 private:
  void Splay(SplayTreeNode* node);
  std::pair<SplayTreeNode*, SplayTreeNode*> Split(SplayTreeNode* node);
  SplayTreeNode* Merge(SplayTreeNode* _left, SplayTreeNode* _right);
  void RotateRight(SplayTreeNode* root);
  void RotateLeft(SplayTreeNode* root);

  int GetCount(SplayTreeNode* node);
  void FixCount(SplayTreeNode* node);
  SplayTreeNode* FindPlace(int k);

  SplayTreeNode* root = nullptr;
};


SplayTree::~SplayTree() {
  if (!root) return;
  std::stack<SplayTreeNode*> next_nodes;
  // Добавляем корень на стек
  next_nodes.push(root);
  SplayTreeNode* current_node = nullptr;
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


// Добавление элемента
void SplayTree::Add(int value) {
  SplayTreeNode* current_node = root;
  // Если дерево пусто, новый узел в корне
  if (current_node == nullptr) {
    root = new SplayTreeNode(value);
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
        current_node->left = new SplayTreeNode(value);
        // Проставляем ссылку на родителя
        current_node->left->parent = current_node;
        // Правим количество узлов в поддереве 
        FixCount(current_node);
        // Поднимаем вставленный узел
        Splay(current_node->left);
        return;
      }
    // Иначе, если значение больше или равно значения в текущем узле
    } else {
      // Если есть правый ребенок, то он станет текущим узлом
      if (current_node->right) {
        current_node = current_node->right;
      // Иначе, если правого ребенка нет, создаем новый узел
      } else {
        current_node->right = new SplayTreeNode(value);
        // Проставляем ссылку на родителя
        current_node->right->parent = current_node;
        // Правим количество узлов в поддереве 
        FixCount(current_node);
        // Поднимаем вставленный узел
        Splay(current_node->right);
        return;
      }
    }
  }
}


// Обертка для Add(), возвращает место,
// на которое должен встать солдат
int SplayTree::AddReturnPlace(int value) {
  Add(value);
  // Место - это количество узлов в правом поддереве
  // т.к. солдат выстраивают по убыванию роста
  return GetCount(root->right);
}


// Возвращает количество узлов в поддереве, 
// обрабатывает случай, когда узел = nullptr
int SplayTree::GetCount(SplayTreeNode* node) {
  int node_count = 0;
  if (node) {
        node_count = node->count;
      }
  return node_count;
}


// Разделение дерева по узлу
std::pair<SplayTreeNode*, SplayTreeNode*> SplayTree::Split(SplayTreeNode* node) {
  Splay(node);
  SplayTreeNode* _left = node->left;
  SplayTreeNode* _right = node->right;
  // Убираем ссылки на родителей
  if (_left) {
    _left->parent = nullptr;
  }
  if (_right) {
    _right->parent = nullptr;
  }

  std::pair<SplayTreeNode*, SplayTreeNode*> subtrees(_left, _right);
  return subtrees;
}


// Убирает солдата, стоящего на месте k
void SplayTree::RemovePlace(int k) {
  SplayTreeNode* node_to_del = FindPlace(k);
  std::pair<SplayTreeNode*, SplayTreeNode*> subtrees = Split(node_to_del); 
  root = Merge(subtrees.first, subtrees.second);
  delete node_to_del;
}

// Поиск солдата на месте k
SplayTreeNode* SplayTree::FindPlace(int k) {
  // Т.к. солдат расставляют по убыванию роста,
  // ищем (n-k)-ую порядковую статистику.
  k = (root->count) - (k);
  // Проверка, что солдат k в строю
  assert((k >= 0) && (k <= root->count));
  int left_count = 0;
  SplayTreeNode* current_node = root;
  while (k > 0) {
    left_count = GetCount(current_node->left);
    // Если в левом поддереве узлов больше, чем k,
    // то меняем текущий узел на левого ребенка.
    if (left_count >= k) {
      current_node = current_node->left;
    // Если в кол-во узлов в левом поддереве + 1 (текущий узел)
    // равно k, то нашли нужного солдата 
    } else if (left_count + 1 == k) {
      break;
    // Иначе уходим в правое поддерево и пересчитываем k
    } else {
      current_node = current_node->right;
      k-=(left_count + 1);
    }
  }
  return current_node;
}


// Слияние двух деревьев 
SplayTreeNode* SplayTree::Merge(SplayTreeNode* _left, SplayTreeNode* _right) {
  // Если есть левое поддерево
  if (_left) {
    // Ищем максимальный узел в левом поддереве
    while (_left->right) {
      _left = _left->right;
    }
    // Поднимаем узел
    Splay(_left);
    // Подсоединяем правое поддерево
    _left->right = _right;
    // Правим ссылку на родителя
    if (_right) {
      _right->parent = _left;
    }
    return _left;
  // Если левого поддерева нет, возвращаем правое
  } else {
    return _right;
  }
}


void SplayTree::RotateRight(SplayTreeNode* _root) {
  // Проверка на существование вращаемых узлов
  assert(_root);
  assert(_root->left);

  SplayTreeNode* new_root = _root->left;
  new_root->parent = _root->parent;
  _root->left = new_root->right;

  if (_root->left) {
    _root->left->parent = _root;
  }
  
  new_root->right = _root;
  _root->parent = new_root;

  // Правим ссылку на родителя поддерева после поворота
  if (new_root->parent) {
    if (new_root->parent->left == _root) {
      new_root->parent->left = new_root;
    } else {
      new_root->parent->right = new_root;
    }
  }
  
  // Пересчитываем количество узлов
  _root->count = GetCount(_root->left) + GetCount(_root->right) + 1;
  new_root->count = GetCount(new_root->left) + GetCount(new_root->right) + 1;
}


void SplayTree::RotateLeft(SplayTreeNode* _root) {
  // Проверка на существование вращаемых узлов
  assert(_root);
  assert(_root->right);
  SplayTreeNode* new_root = _root->right;
  new_root->parent = _root->parent;
  _root->right = new_root->left;
  if (_root->right) {
    _root->right->parent = _root;
  }
   
  new_root->left = _root;
  _root->parent = new_root;

  // Правим ссылку на родителя поддерева после поворота
  if (new_root->parent) {
    if (new_root->parent->left == _root) {
      new_root->parent->left = new_root;
    } else {
      new_root->parent->right = new_root;
    }
  }

  // Пересчитываем количество узлов
  _root->count = GetCount(_root->left) + GetCount(_root->right) + 1;
  new_root->count = GetCount(new_root->left) + GetCount(new_root->right) + 1;
}

void SplayTree::Splay(SplayTreeNode* node) {
  SplayTreeNode* p = nullptr;
  SplayTreeNode* g = nullptr;
  while (node->parent) {
    p = node->parent;
    if (p->parent) {
      g = p->parent;
      if (p->left == node) {
        if (g->left == p) {
          // ZigZig
          RotateRight(g);
          RotateRight(p);
        } else {
          // ZigZag
          RotateRight(p);
          RotateLeft(g);
        }
      } else {
        if (g->right == p) {
          // ZigZig
          RotateLeft(g);
          RotateLeft(p);
        } else {
          // ZigZag
          RotateLeft(p);
          RotateRight(g);
        }
      }
    } else {
      // Zig
      if (p->left == node) {
        RotateRight(p);
      } else if ((p->right == node)) {
        RotateLeft(p);
      }
    }
  }
  // Узел, поднятый наверх, назначаем новым корнем
  root = node;
}

// Функция для итеративной правки количество узлов
// от полученного узла до корня или nullptr
void SplayTree::FixCount(SplayTreeNode* node) {
  int left_count = 0;
  int right_count = 0;
  while (node) {
    left_count = GetCount(node->left);
    right_count = GetCount(node->right);

    node->count = left_count + right_count + 1;
    node = node->parent;
  }
}

int main() {
    int n = 0;
    std::cin >> n;
    SplayTree tree;
    int cmd = 0;
    int inp = 0;
    for (int i = 0; i < n; i++) {
      std::cin >> cmd >> inp;
      // Добавляем солдата по его росту inp, возвращаем его место
      if (cmd == 1) {
        std::cout << tree.AddReturnPlace(inp) << std::endl;
      // Удаляем солдата по его месту inp
      } else if (cmd == 2) {
        tree.RemovePlace(inp);
      }
    }
    return 0;
}
