#include <fstream>
#include <cstdio>
#include <vector>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>

#include "completion.h"
#include "file_system.h"
#include "frecency_completion.h"

#define EXIT_FAILURE 1

using namespace std;

#define DATE (string("Date:"))

void init() {
  mkdir(DATA_DIR.c_str(), 0777);
}

int main(int argc, char **argv) {
  if (argc < 3)
    return EXIT_FAILURE;

  init();

  string cmd = argv[1];
  string path = argv[2];

  if (cmd == "complete") {
    RealFileSystem<LexFile> fs_lex;
    Completion<LexFile> comp_lex(fs_lex, path);

    RealFileSystem<RevLexFile> fs_revlex;
    Completion<RevLexFile> comp_revlex(fs_revlex, path);

    RealFileSystem<FrecencyFile> fs_frec;
    FrecencyCompletion comp_frec(fs_frec, path);

    vector<File> matches = comp_frec.get_matches();

    for (uint i = 0; i < matches.size(); i++)
      printf("%s\n", matches[i].name.c_str());
  } else if (cmd == "choose") {
    if (argc < 4)
      return EXIT_FAILURE;

    string choice = argv[3];

    RealFileSystem<FrecencyFile> fs_frec;
    FrecencyCompletion comp_frec(fs_frec, path);
    comp_frec.choose(choice);
  } else if (cmd == "test") {
    VirtualFileSystem<LexFile> fs_lex;

    long long time;
    int cnt = 0;

    ifstream f;
    f.open(argv[2]);
    while (!f.eof()) {
      string line;
      getline(f, line);
      if (line.size() == 0) {
        continue;
      } else if (line.find(":") < line.size()) {
        if (line.substr(0, sizeof(DATE)) == DATE) {
          time = stoll(line.substr(DATE.size()));
        }
      } else {
        string name;

        line = "/" + line;
        fs_lex.create_file(line);
        for (uint i = 1; i < line.size(); i += (name + DIR_DELIM).size()) {
          size_t found = line.find(DIR_DELIM, i);
          name = line.substr(i, found - i);

          Completion<LexFile> comp_lex(fs_lex, line.substr(0, i + 1));
          vector<LexFile> matches = comp_lex.get_matches();

          for (uint j = 0; j < matches.size(); j++) {
            if (matches[j].name == name) {
              cnt += j + 1;
              break;
            }
          }
        }
      }
    }
    f.close();

    // TODO:
    time = time;

    printf("%d\n", cnt);
  }

  return 0;
}
