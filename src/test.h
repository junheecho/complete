#pragma once

#include <time.h>
#include <iostream>
#include <fstream>
#include <string>

#include "file_system.h"

using namespace std;

#define DATE string("Date:")

class TestCompletion {
public:
  static int test(Completion &comp, string path) {
    VirtualFileSystem &fs = (VirtualFileSystem&)comp.fs;

    time_t begun_at = time(NULL);

    int n_tries = 0;
    int n_unamb = 0;
    int n_amb = 0;
    int n_tab_for_amb = 0;
    int n_tab = 0;
    bool begun = false;

    ifstream f;
    f.open(path);
    while (!f.eof()) {
      string line;
      getline(f, line);
      if (line.size() == 0) {
        continue;
      } else if (line == "BEGIN_TEST") {
        begun = true;
      } else if (line.find(":") < line.size()) {
        if (line.substr(0, DATE.size()) == DATE) {
          fs.set_time(stoll(line.substr(DATE.size())));
        }
      } else {
        string name;

        line = "/" + line;
        fs.create_file(line);
        for (uint i = 1; i < line.size(); i += (name + DIR_DELIM).size()) {
          size_t found = line.find(DIR_DELIM, i);
          name = line.substr(i, found - i);

          comp.init(line.substr(0, i + 1));
          vector<File> matches = comp.get_matches();
          comp.choose((i + name.size() == line.size() ? name : name + DIR_DELIM));

          if (begun) {
            n_tries++;
            for (uint j = 0; j < matches.size(); j++) {
              if (matches[j].name == (i + name.size() == line.size() ? name : name + DIR_DELIM)) {
                if (matches.size() == 1) {
                  n_unamb++;
                } else if (matches.size() > 1) {
                  n_amb++;
                  n_tab_for_amb += j + 1;
                }
                n_tab += j + 1;
                break;
              }
            }
          }
        }
      }
    }
    f.close();

    printf("# of tries: %d\n", n_tries);
    printf("# of unambiguous completion: %d\n", n_unamb);
    printf("# of ambiguous completion: %d\n", n_amb);
    printf("# of TABs when ambiguous: %d\n", n_tab_for_amb);
    printf("total # of TABs: %d\n", n_unamb + n_tab_for_amb);
    printf("avg. TABs: %lf\n", (double) n_tab / n_tries);
    printf("%lld sec\n", (long long)time(NULL) - begun_at);

    return n_unamb + n_tab_for_amb;
  }
};
