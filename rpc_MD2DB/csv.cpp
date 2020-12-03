#include "csv.h"

#include <fstream>

std::string CSVRow::operator[](std::size_t index) const {
  return std::string(&m_line[m_data[index] + 1],
                     m_data[index + 1] - (m_data[index] + 1));
}

void CSVRow::readNextRow(std::istream& str) {
  std::getline(str, m_line);

  m_data.clear();
  m_data.emplace_back(-1);
  std::string::size_type pos = 0;
  while ((pos = m_line.find(',', pos)) != std::string::npos) {
    m_data.emplace_back(pos);
    ++pos;
  }
  // This checks for a trailing comma with no data after it.
  pos = m_line.size();
  m_data.emplace_back(pos);
}

std::istream& operator>>(std::istream& str, CSVRow& data) {
  data.readNextRow(str);
  return str;
}

CSVIterator::CSVIterator(std::istream& stream)
    : m_str(stream.good() ? &stream : nullptr) {
  ++(*this);
}

// Pre Increment
CSVIterator& CSVIterator::operator++() {
  if (m_str) {
    if (!((*m_str) >> m_row)) {
      m_str = nullptr;
    }
  }

  return *this;
}

CSVIterator CSVIterator::operator++(int) {
  CSVIterator tmp(*this);
  ++(*this);
  return tmp;
}

bool CSVIterator::operator==(CSVIterator const& rhs) {
  return ((this == &rhs) ||
          ((this->m_str == nullptr) && (rhs.m_str == nullptr)));
}

bool CSVIterator::operator!=(CSVIterator const& rhs) {
  return !((*this) == rhs);
}
