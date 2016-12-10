#include <cstdio>
#include <vector>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>

#include "completion.h"
#include "frecency_completion.h"

#define EXIT_FAILURE 1

using namespace std;

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
    Completion<LexFile> comp_lex(path);
    Completion<RevLexFile> comp_revlex(path);
    FrecencyCompletion comp_frec(path);

    vector<File> matches = comp_frec.get_matches();

    for (uint i = 0; i < matches.size(); i++)
      printf("%s\n", matches[i].name.c_str());
  } else if (cmd == "choose") {
    if (argc < 4)
      return EXIT_FAILURE;

    string choice = argv[3];

    FrecencyCompletion comp_frec(path);
    comp_frec.choose(choice);
  }

  return 0;
}
