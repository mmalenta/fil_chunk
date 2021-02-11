#include <iostream>
#include <sstream>
#include <string>

#include "filterbank.hpp"

int main(int argc, char *argv[]) {

  if (argc < 2) {
    std::cerr << "Not enough arguments provided!\n";
    return 1;
  }

  float skip_seconds = 0.0;
  float read_seconds = 5.0;
  std::string input_file;
  std::string output_file;

  for (int iarg = 1; iarg < argc; iarg++) {

    if(std::string(argv[iarg]) == "-s") {
      iarg++;
      skip_seconds = std::atof(argv[iarg]);
    } else if (std::string(argv[iarg]) == "-r") {
      iarg++;
      read_seconds = std::atof(argv[iarg]);
    } else if (std::string(argv[iarg]) == "-f") {
      iarg++;
      input_file = std::string(argv[iarg]);
      
      std::ostringstream oss;
      oss << input_file << ".skip_" << skip_seconds << "_read_" << read_seconds;
      output_file = oss.str();
      oss.str("");
      oss.clear();

    }

  }

  std::cout << "Selected configuration:\n"
            << "\tInput file: " << input_file << "\n"
            << "\tSeconds to skip: " << skip_seconds << "\n"
            << "\tSeconds to read: " << read_seconds << "\n"
            << "\tOutput file: " << output_file << "\n\n";

  std::cout << "Reading the header...\n";

  Filterbank filterbank(input_file);
  filterbank.read_header();
  filterbank.print_header();
  
  filterbank.read_data(skip_seconds, read_seconds);
  filterbank.save_data(output_file);

  return 0;
}