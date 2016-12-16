#include <fstream>
#include <cstdio>
#include <vector>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>

#include "completion.h"
#include "file_system.h"
#include "frecency_completion.h"
#include "test.h"
#include "symbreg.h"

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
    RealFileSystem fs;

    LexCompletion comp_lex(fs);
    RevLexCompletion comp_revlex(fs);
    FrecencyCompletion comp_frec(fs);

    Completion *comp = &comp_frec;

    comp->init(path);
    vector<File> matches = comp->get_matches();

    for (uint i = 0; i < matches.size(); i++)
      printf("%s\n", matches[i].name.c_str());
  } else if (cmd == "choose") {
    if (argc < 4)
      return EXIT_FAILURE;

    string choice = argv[3];

    RealFileSystem fs;

    FrecencyCompletion comp_frec(fs);

    comp_frec.init(path);
    comp_frec.choose(choice);
  } else if (cmd == "test") {
    VirtualFileSystem fs_lex, fs_revlex;

    LexCompletion comp_lex(fs_lex);
    RevLexCompletion comp_revlex(fs_revlex);

    printf("\nLexCompletion\n");
    TestCompletion::test(comp_lex, argv[2]);
    printf("\nRevLexCompletion\n");
    TestCompletion::test(comp_revlex, argv[2]);

    VirtualFileSystem fs_frec;
    FrecencyCompletion comp_frec(fs_frec, "");
    printf("\nFrecencyCompletion\n");
    TestCompletion::test(comp_frec, argv[2]);
  }

  return 0;
}
