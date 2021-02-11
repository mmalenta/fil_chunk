#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <type_traits>

class Filterbank
{
public:
  Filterbank(std::string input_file);
  ~Filterbank();

  template <typename HeaderType>
  HeaderType get_header_value(std::string key);

  std::unordered_map<std::string, int> int_header(void) { return _int_header_values;};
  std::unordered_map<std::string, double> double_header(void) { return _double_header_values;};

  void read_data(int read_samples);
  void read_data(float skip, float read);
  void save_data(std::string output);

  void read_header(std::vector<std::pair<std::string, int>> keys = {{"tstart", 8}, {"fch1", 8}, {"foff", 8}, {"nbits", 4}, {"nchans", 4}, {"tsamp", 8}});

  void print_header(void);

  unsigned char* get_data(void) { return _data;};

protected:

private:

  template <typename HeaderType>
  HeaderType read_header_value(std::string);

  const std::string _input_file;
  std::unordered_map<std::string, double> _double_header_values;
  std::unordered_map<std::string, int> _int_header_values;

  char *_header;
  unsigned char *_data;

  int _header_length;

};

template <typename HeaderType>
HeaderType Filterbank::get_header_value(std::string key) {

  if (std::is_same<HeaderType, int>::value) {
    return _int_header_values[key];
  } else if (std::is_same<HeaderType, double>::value) {
    return _double_header_values[key];
  }

}

template <typename HeaderType>
HeaderType Filterbank::read_header_value(std::string key) {


  HeaderType value;

  int key_length = key.length();
  char *header_ptr = _header; // skip "HEADER_START"
  char *read_val = new char[sizeof(HeaderType)];

  while (true) {

    if (std::string(header_ptr, header_ptr + key_length) == key) {
      std::copy(header_ptr + key_length, header_ptr + key_length + sizeof(HeaderType), read_val);
      value = *(reinterpret_cast<HeaderType*>(read_val));
      break;
    }
    header_ptr++; // read 1 character at a time
  }

  return value;

}