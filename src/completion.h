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

class Completion {
protected:
  string dir;
  string prefix;
  vector<File> matches;

public:
  FileSystem &fs;

  Completion(FileSystem &_fs) : fs(_fs) {}

  virtual void sort() = 0;

  virtual void init(string path) {
    size_t found = path.find_last_of(DIR_DELIM);
    dir = path.substr(0, found);
    prefix = path.substr(found + 1);

    matches = fs.get_matches(path);
  }

  vector<File> get_matches() {
    this->sort();

    vector<File> result;
    for (uint i = 0; i < matches.size(); i++)
      result.push_back(matches[i]);
    return result;
  }

  virtual void choose(string choice) {}
};

class LexCompletion : public Completion {
public:
  LexCompletion(FileSystem &_fs) : Completion(_fs) {}

  static bool compare_lex(const File &x, const File &y) {
    return x.name < y.name;
  }
  void sort() {
    std::sort(matches.begin(), matches.end(), compare_lex);
  }
};

class RevLexCompletion : public Completion {
public:
  RevLexCompletion(FileSystem &_fs) : Completion(_fs) {}

  static bool compare_revlex(const File &x, const File &y) {
    return x.name > y.name;
  }
  void sort() {
    std::sort(matches.begin(), matches.end(), compare_revlex);
  }
};
