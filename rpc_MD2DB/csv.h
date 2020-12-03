#pragma once

#include <string>
#include <vector>

class CSVRow {
  std::vector<int> m_data;
  std::string m_line;

 public:
  std::string operator[](std::size_t index) const;
  std::size_t size() const { return m_data.size() - 1; }
  void readNextRow(std::istream& str);
};

std::istream& operator>>(std::istream&, CSVRow&);

class CSVIterator {
  std::istream* m_str = nullptr;
  CSVRow m_row;

 public:
  CSVIterator() = default;
  CSVIterator(std::istream&);

  CSVIterator& operator++();
  CSVIterator operator++(int);
  CSVRow const& operator*() const { return m_row; }
  CSVRow const* operator->() const { return &m_row; }
  bool operator==(CSVIterator const&);
  bool operator!=(CSVIterator const&);
};

class CSVRange {
  std::istream& stream;

 public:
  CSVRange(std::istream& str) : stream(str) {}
  CSVIterator begin() const { return CSVIterator{stream}; }
  CSVIterator end() const { return CSVIterator{}; }
};
