#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "filterbank.hpp"

Filterbank::Filterbank(std::string input_file)
        : _input_file(input_file) {

}

Filterbank::~Filterbank() {
  delete [] _header;
  delete [] _data;
}

void Filterbank::read_data(int read_samples) {

  std::cout << "Reading the data...\n";

  std::ifstream fil_file(_input_file, std::ios::binary);
  fil_file.seekg(_header_length, fil_file.beg);

  size_t read_size = read_samples * _int_header_values["nchans"];
  _data = new unsigned char[read_size];
  fil_file.read(reinterpret_cast<char*>(_data), read_size * sizeof(char));

  fil_file.close();

}

void Filterbank::read_data(float skip, float read) {

  std::cout << "Reading the data...\n";

  std::ifstream fil_file(_input_file, std::ios::binary);
  fil_file.seekg(_header_length, fil_file.beg);

  size_t time_samples_skip = floor(skip / _double_header_values["tsamp"]);
  size_t time_samples_read = ceil(read / _double_header_values["tsamp"]);

  size_t full_skip = time_samples_skip * _int_header_values["nchans"] * _int_header_values["nbits"] / 8;
  size_t full_read = time_samples_read * _int_header_values["nchans"] * _int_header_values["nbits"] / 8;

  fil_file.seekg(full_skip, fil_file.cur);

  _data = new unsigned char[full_read];
  fil_file.read(reinterpret_cast<char*>(_data), full_read * sizeof(char));

  _double_header_values["tstart"] = _double_header_values["tstart"] + skip / 86400.0;
  _int_header_values["nsamps"] = time_samples_read;

}

void Filterbank::save_data(std::string output) {

  std::ofstream fil_file(output, std::ios::binary);

  int length = 12;

  fil_file.write((char*)&length, sizeof(int));
  fil_file.write("HEADER_START", length * sizeof(char));

  length = 6;
  fil_file.write((char*)&length, sizeof(int));
  fil_file.write("tstart", length * sizeof(char));
  fil_file.write((char*)&_double_header_values["tstart"], sizeof(double));

  fil_file.write((char*)&length, sizeof(int));
  fil_file.write("nchans", length * sizeof(char));
  fil_file.write((char*)&_int_header_values["nchans"], sizeof(int));

  fil_file.write((char*)&length, sizeof(int));
  fil_file.write("nsamps", length * sizeof(char));
  fil_file.write((char*)&_int_header_values["nsamps"], sizeof(int));

  length = 5;
  fil_file.write((char*)&length, sizeof(int));
  fil_file.write("tsamp", length * sizeof(char));
  fil_file.write((char*)&_double_header_values["tsamp"], sizeof(double));

  fil_file.write((char*)&length, sizeof(int));
  fil_file.write("nbits", length * sizeof(char));
  fil_file.write((char*)&_int_header_values["nbits"], sizeof(int));

  length = 4;
  fil_file.write((char*)&length, sizeof(int));
  fil_file.write("fch1", length * sizeof(char));
  fil_file.write((char*)&_double_header_values["fch1"], sizeof(double));

  fil_file.write((char*)&length, sizeof(int));
  fil_file.write("foff", length * sizeof(char));
  fil_file.write((char*)&_double_header_values["foff"], sizeof(double));

  length = 10;
  fil_file.write((char*)&length, sizeof(int));
  fil_file.write("HEADER_END", length * sizeof(char));

  size_t full_write = static_cast<size_t>(_int_header_values["nsamps"]) * _int_header_values["nchans"] * _int_header_values["nbits"] / 8;

  std::cout <<  _int_header_values["nsamps"] << " " << _int_header_values["nchans"] << " " << _int_header_values["nbits"] << "\n";
  std::cout << "Full write: " << full_write << "B\n";

  std::cout << "CHANGING start MJD to " << _double_header_values["tstart"] << "\n";
  std::cout << "CHANGING number of samples to " << _int_header_values["nsamps"] << "\n";

  fil_file.write(reinterpret_cast<char*>(_data), full_write * sizeof(char));

  fil_file.close();

}

void Filterbank::read_header(std::vector<std::pair<std::string, int>> keys) {

  std::cout << "Reading the header...\n";

  std::ifstream fil_file(_input_file, std::ios::binary);
  std::string header_end_str = "HEADER_END";
  int str_length = header_end_str.length();
  int rewind = -1 * (str_length - 1); // progress one character at a time
  int header_length = 0;
  char *char_read = new char[str_length];

  while(true) {
    fil_file.read(char_read, str_length);
    if (std::string(char_read, str_length) == header_end_str) {
      header_length = fil_file.tellg();
      break;
    }
    fil_file.seekg(rewind, fil_file.cur);
  }

  _header_length = header_length;

  std::cout << "Header length: " << _header_length << "\n";
  fil_file.seekg(0, fil_file.beg);

  _header = new char[_header_length];
  fil_file.read(_header, _header_length);

  for (auto key: keys) {

    switch(key.second) {
      case 4:
        _int_header_values[key.first] = read_header_value<int>(key.first);
        break;
      case 8:
        _double_header_values[key.first] = read_header_value<double>(key.first);
        break;
      default:
        std::cout << "Unsupported number of bits: " << key.second << "\n";
    }  

  }

  // Technically not a header value if it is not there
  // (it is specified in the SIGPROC standard though)
  // Still necessary for workign with the file
  fil_file.seekg(0, fil_file.beg);
  fil_file.seekg(_header_length, fil_file.beg);
  size_t data_beg = fil_file.tellg();
  fil_file.seekg(0, fil_file.end);
  size_t data_end = fil_file.tellg();

  size_t data_size = data_end - data_beg;
  int nsamples = static_cast<int>(data_size / (_int_header_values["nchans"])
                              / ((_int_header_values["nbits"]) / 8));
  _int_header_values["nsamps"] = nsamples;

  fil_file.close();

}

void Filterbank::print_header(void) {

  for (auto &header_value: _int_header_values) {
      std::cout << header_value.first << ": " << get_header_value<int>(header_value.first) << "\n"; 
  }

  for (auto &header_value: _double_header_values) {
      std::cout << header_value.first << ": " << get_header_value<double>(header_value.first) << "\n"; 
  }

}