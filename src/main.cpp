#include <string>
#include <iostream>

#include "./tag_reader.cpp"
#include "./tag_writer.cpp"

// Reading
// `tagreader read asd.mp3`
// Updating
// `tagreader update asd.mp3 hej="asd" TITLE="dfhgldf"`

// `tagreader update-cover asd.mp3 hej.jpeg`
// `tagreader read-cover asd.mp3 <cover.jpeg?>`
// std::map<std::string, std::string> parse_params(int argc, char *argv[]) {
//   std::map<std::string, std::string> *params = new std::map<std::string, std::string>();

//   if (argc < 3) {
//     std::cout << "Not Enough arguments" << std::endl;
//   }

//   for (int i = 3; i < argc; ++i) {
//     std::cout << argv[i] << std::endl;
//   }

//   return params;
// }


int main(int argc, char *argv[]) {
  std::string action(argv[1]);
  std::string file_path(argv[2]);

  if (action == "read") {
    read_tags(file_path);
  } else if (action == "update") {
    // std::map<std::string, std::string> params = parse_params(argc, argv);
    std::map<std::string, std::string> params = {};
    for (int i = 3; i < argc; ++i) {
      std::string arg = argv[i];
      std::string param = arg.substr(0, arg.find("="));
      std::string value = arg.substr(arg.find("=") + 1);
      params.insert({param, value});
    }
    write_tags(file_path, params);
    read_tags(file_path);
  } else if (action == "read-cover") {
    std::cout << "We read-cover now" << std::endl;
  } else if (action == "update-cover") {
    std::cout << "We update-cover now" << std::endl;
  } else if (action == "remove") {
    std::cout << "We remove now" << std::endl;
  } else {
    std::cout << "Unrecognized Action" << std::endl;
  }
  return 0;
}
