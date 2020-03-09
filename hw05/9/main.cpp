/* Алгоритм сжатия данных Хаффмана

Реализуйте алгоритм построения оптимального префиксного кода Хаффмана.
При помощи алгоритма реализуйте две функции для создания архива из одного файла
и извлечения файла из архива.

*/

#include "Huffman.h"
#include <assert.h>
#include <vector>
#include <unordered_map>
#include <queue>
#include <stack>
#include <string>

// Узел для дерева кодирования
struct TreeNode {
  explicit TreeNode(int _freq, byte _symbol) :  freq(_freq), symbol(_symbol) {}
  // Частота символа (в листе) или сумма частот листьев (для внутреннего узла)
  int freq = 0;
  // Значение исходного символа
  byte symbol = 0x00;
  TreeNode* left = nullptr;
  TreeNode* right = nullptr;
};

// Компаратор для сравнения узлов по частотам
class Compare {
  public:
    bool operator()(TreeNode* left, TreeNode* right) {
      return left->freq > right->freq;
    }
};

// Класс для побитовой записи
class BitsWriter {
 public:
  void WriteBitToBuffer(bool bit);
  void WriteBufferToStream(IOutputStream& compressed);

 private:
  std::vector<byte> buffer_;
  unsigned char accumulator_ = 0;
  int bits_count_ = 0;
};

// Добавление бита в буффер
void BitsWriter::WriteBitToBuffer(bool bit) {
  // Ставим бит в аккумулятор на нужное место
  accumulator_ |= static_cast<unsigned char>(bit) << bits_count_;
  ++bits_count_;
  if (bits_count_ == 8) {
    bits_count_ = 0;
    buffer_.push_back(accumulator_);
    accumulator_ = 0;
  }
}

// Запись буфера в поток
void BitsWriter::WriteBufferToStream(IOutputStream& compressed) {
  if (bits_count_ != 0) {
    // Добавляем в буфер аккумулятор, если в нем что-то есть.
    buffer_.push_back(accumulator_);
  }
  buffer_.push_back(static_cast<byte>(bits_count_));
  for (byte byte : buffer_) {
    compressed.Write(byte);
  }
  buffer_.clear();
}

// Класс для побитового чтения
class BitsReader {
 public:
  bool ReadBit(std::queue<byte>& compressed);
  byte ReadByte(std::queue<byte>& compressed);

 private:
  byte buffer_;
  int bits_count_ = 0;
};

// Чтение одного бита
bool BitsReader::ReadBit(std::queue<byte>& compressed) {
  if (bits_count_ == 0) {
    buffer_ = compressed.front();
    compressed.pop();
    bits_count_ = 8;
    }
  bool bit = buffer_ & 1;
  bits_count_--;
  buffer_ = buffer_ >> 1;
  return bit;
}

// Чтение одного байта
byte BitsReader::ReadByte(std::queue<byte>& compressed) {
  byte value;
  value = compressed.front();
  compressed.pop();
  return value;
}


void Encode(IInputStream& original, IOutputStream& compressed) {
  std::vector<byte> original_message;
  byte symbol;
  std::unordered_map<byte,int> freq_count;
  // Проходимся по оригинальному сообщению, сохраняя его
  // и считая частоты символов
  while (original.Read(symbol)) {
    original_message.push_back(symbol);
    auto it = freq_count.find(symbol);
    if (it != freq_count.end()) {
      it->second++;
    } else {
      freq_count[symbol] = 1;
    }
  }

  std::priority_queue<TreeNode*, std::vector<TreeNode*>, Compare> nodes_priority_queue;
  TreeNode* new_node = nullptr;
   // Создаем листья дерева
  for (auto i = freq_count.begin(); i != freq_count.end(); i++) { 
    new_node = new TreeNode(i->second, i->first);
    nodes_priority_queue.push(new_node);
  }

  TreeNode* left = nullptr;
  TreeNode* right = nullptr;
  int alphabet_size = nodes_priority_queue.size();
    // Строим дерево кодирования
  for (int i = 1; i < alphabet_size;  i++){
    left = nodes_priority_queue.top();
    nodes_priority_queue.pop();
    right = nodes_priority_queue.top();
    nodes_priority_queue.pop();
    new_node = new TreeNode(left->freq + right->freq, 0x00);
    new_node->left = left;
    new_node->right = right;
    nodes_priority_queue.push(new_node);
  }

  TreeNode* root = nodes_priority_queue.top();
  nodes_priority_queue.pop();

  // Создаем стек для пар узел-код
  std::stack<std::pair<TreeNode*, std::string>> node_n_code_pairs;
  // Добавляем корень и его код на стек
  if ((root->left == nullptr) && (root->right == nullptr)) {
    // Обрабатываем случай с одним символом в алфавите 
    node_n_code_pairs.push(std::make_pair(root, "0"));
  } else {
    node_n_code_pairs.push(std::make_pair(root, ""));
  }

  // Создаем map из символов в коды для всех узлов
  std::unordered_map<byte, std::string> symbol_to_code;
  std::pair<TreeNode*, std::string> current_node;
  // Обходим дерево, составляем и запоминаем коды листьев 
  while (!node_n_code_pairs.empty()) {
    current_node = node_n_code_pairs.top();
    node_n_code_pairs.pop();
    if  (current_node.first->right != nullptr) {
      node_n_code_pairs.push(std::make_pair(current_node.first->right, current_node.second+"1"));
    } else {
      if (current_node.first->left == nullptr) {
        symbol_to_code[current_node.first->symbol] = current_node.second;
      }
    }
    if  (current_node.first->left != nullptr) {
      node_n_code_pairs.push(std::make_pair(current_node.first->left, current_node.second+"0"));
    } else {
      if (current_node.first->right == nullptr) {
        symbol_to_code[current_node.first->symbol] = current_node.second;
      }
    }
  }

  // Запишем длину алфавита минус 1, чтобы учесть случай длины алфавита 256
  compressed.Write(static_cast<byte>(alphabet_size - 1));

  unsigned int max_code_length = 0;
  // Найдем максимальную длину кода
  for (auto it = symbol_to_code.begin(); it != symbol_to_code.end(); it++) {
    if (max_code_length < it->second.size()) {
      max_code_length = it->second.size();
    }
  }

  std::vector<unsigned int> code_length_counts;
  // Посчитаем количество кодов разной длины и сохраним алфавит в порядке возрастания
  std::vector<byte> alphabet;
  for (unsigned int i = 1; i <= max_code_length; i++) {
    unsigned int count = 0;
    for (auto it = symbol_to_code.begin(); it != symbol_to_code.end(); it++) {
      if (it->second.size()==i) {
        count++;
        alphabet.push_back(it->first);
      }
    }
    code_length_counts.push_back(count);
  }

  // Пишем исходные незакодированные символы
  for (byte symbol : alphabet) {
    compressed.Write(symbol);
  }

  // Пишем максимальную длину кода
  compressed.Write(static_cast<byte>(max_code_length));

  // Пишем количество кодов для каждой длины кода меньше максимальной
  for (unsigned int count : code_length_counts) {
    compressed.Write(static_cast<byte>(count));
  }

  BitsWriter writer = BitsWriter();
  
  // Пишем коды для символов в буффер writer'а
  for (byte symbol : alphabet) {
    for (char bit : symbol_to_code[symbol]) {
      switch (bit)
      {
      case '1':
        writer.WriteBitToBuffer(true);
        break;
      case '0':
        writer.WriteBitToBuffer(false);
        break;
      }
      
    }
  }

  // Пишем закодированное сообщение в буфер writer'а
  for (byte symbol : original_message) {
    for (char bit : symbol_to_code[symbol]) {
      switch (bit)
      {
      case '1':
        writer.WriteBitToBuffer(true);
        break;
      case '0':
        writer.WriteBitToBuffer(false);
        break;
      }
    }
  }

  // Записываем буфер writer'а в поток
  writer.WriteBufferToStream(compressed);


  std::stack<TreeNode*> nodes_to_delete;
  nodes_to_delete.push(root);
  TreeNode* node = nullptr;
  // Освобождаем память от узлов дерева
  while (!nodes_to_delete.empty()) {
    node = nodes_to_delete.top();
    nodes_to_delete.pop();
    if  (node->right != nullptr) {
      nodes_to_delete.push(node->right);
    }
    if  (node->left != nullptr) {
      nodes_to_delete.push(node->left);
    }
    delete node;
  }
}


void Decode(IInputStream& compressed, IOutputStream& original) {
  std::queue<byte> encoded_message;
  byte encoded_byte;
  // Читаем закодированное сообщение
  while (compressed.Read(encoded_byte)) {
    encoded_message.push(encoded_byte);
  }

  BitsReader reader = BitsReader();
  // Считаем длину алфавита, прибавляем 1, т.к. отнимали, когда записывали
  unsigned int alphabet_size = static_cast<unsigned int>(reader.ReadByte(encoded_message)) + 1;

  std::vector<byte> symbols;
   // Читаем символы исходного алфавита
  for (unsigned int i = 0; i < alphabet_size; i++) {
    symbols.push_back(reader.ReadByte(encoded_message));
  }

  // Считываем максимальную длину кода
  unsigned int max_code_length = static_cast<unsigned int>(reader.ReadByte(encoded_message));
  std::vector<unsigned int> code_length_counts;

  // Считываем количество кодов разной длины
  for (unsigned int i = 0; i < max_code_length; i++) {
    code_length_counts.push_back(static_cast<unsigned int>(reader.ReadByte(encoded_message)));
  }

  std::string code = "";
  bool bit = false;
  std::vector<std::string> codes;
  // Читаем коды символов по одному биту
  for (unsigned int i=0; i < code_length_counts.size(); i++) {
    for (unsigned int n=0; n < code_length_counts[i]; n++) {
      code = "";
      for (unsigned int j=0; j < i + 1; j++) {
        bit = reader.ReadBit(encoded_message);
        switch (bit)
        {
        case true:
          code += "1";
          break;
        case false:
          code += "0";
          break;
        }
      }
      codes.push_back(code);
    }
    
  }

  // Создаем map из кодов в символы
  std::unordered_map<std::string, byte> code_to_symbol;
  for (unsigned int i=0; i < alphabet_size; i++){
    code_to_symbol[codes[i]] = symbols[i];
  }
  // Считаем количество битов в предпоследнем байте из последнего байта
  unsigned int last_bits = static_cast<unsigned int>(encoded_message.back());

  // Декодируем сообщение
  while (encoded_message.size() > 0) {
    code = "";
    // Если прочитали последние биты выходим из цикла
    if ((last_bits == 0) && (encoded_message.size()==1)) {
      break;
    }
    // Читаем код по одному символу
    for (unsigned int i = 0; i < max_code_length; i++) {
      bit = reader.ReadBit(encoded_message);
      if (encoded_message.size()==1) {
        last_bits--;
      }
      switch (bit)
        {
        case true:
          code += "1";
          break;
        case false:
          code += "0";
          break;
        }
      // Если код символа есть в map, декодируем символ 
      auto search = code_to_symbol.find(code);
      if (search != code_to_symbol.end()) {
          original.Write(code_to_symbol[code]);
          break;
      }
    }
  }
}
