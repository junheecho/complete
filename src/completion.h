#pragma once

#include <time.h>

#include <cstdlib>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>

#include "file_system.h"

using namespace std;

#define DATA_DIR (string(getenv("HOME")) + DIR_DELIM + ".completion" + DIR_DELIM)

template<typename F>
class Completion {
protected:
  string dir;
  string prefix;
  vector<F> matches;
  FileSystem<F> &fs;

public:
  bool simulation = false;
  long long curr_time;

  Completion(FileSystem<F> &_fs, string path) : fs(_fs) {
    size_t found = path.find_last_of(DIR_DELIM);
    dir = path.substr(0, found);
    prefix = path.substr(found + 1);

    matches = fs.get_matches(path);
  }
  Completion(const Completion &prev) {
    dir = prev.dir;
    prefix = prev.prefix;
    matches = prev.matches;
    fs = prev.fs;
  }

  vector<File> get_matches() {
    sort(matches.begin(), matches.end());
    vector<File> result;
    for (uint i = 0; i < matches.size(); i++)
      result.push_back(matches[i]);
    return result;
  }

  void choose(string choice) {
  }

  void set_time(long long time) {
    this->curr_time = time;
  }

  long long get_time() {
    if (simulation) {
      return this->curr_time;
    } else {
      return time(NULL);
    }
  }
};
