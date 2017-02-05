#include <string>
#include <iostream>

#include "./tag_reader.cpp"
#include "./tag_writer.cpp"

void print_help() {
  std::cout << "Usage: " << std::endl;
  std::cout << "\t music-tagger read <file>" << std::endl;
  std::cout << "\t music-tagger update <file> TITLE=\"New title\"" << std::endl;
}

std::map<std::string, std::string> read_params(int argc, char *argv[]) {
  std::map<std::string, std::string> params = {};
  
  for (int i = 3; i < argc; ++i) {
    std::string arg = argv[i];
    std::string param = arg.substr(0, arg.find("="));
    std::string value = arg.substr(arg.find("=") + 1);
    params.insert({param, value});
  }
}

int main(int argc, char *argv[]) {
  std::string action(argv[1]);
  if (argc <= 2) {
    std::cout << "Unrecognized Action" << std::endl;
    return 0;
  }

  std::string file_path(argv[2]);

  if (action == "read") {
    read_tags(file_path);
  } else if (action == "update") {
    write_tags(file_path, read_params(argc, argv));
    read_tags(file_path);
  } else {
    print_help();
  }
  return 0;
}
