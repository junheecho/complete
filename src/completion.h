#pragma once

#include <cstdlib>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define DIR_DELIM '/'

using namespace std;

string DATA_DIR = string(getenv("HOME")) + DIR_DELIM + ".completion" + DIR_DELIM;

class File {
public:
  string name;
};

class LexFile : public File {};
inline bool operator < (const LexFile &x, const LexFile &y) {
  return x.name < y.name;
}

class RevLexFile : public File {};
inline bool operator < (const RevLexFile &x, const RevLexFile &y) {
  return y.name < x.name;
}

template<typename F>
class Completion {
protected:
  string dir;
  string prefix;
  vector<F> matches;

public:
  Completion(string path) {
    size_t found = path.find_last_of(DIR_DELIM);
    dir = path.substr(0, found);
    prefix = path.substr(found + 1);

    DIR *dp = opendir(dir.c_str());
    if (!dp)
      throw invalid_argument("cannot open the directory");
    struct dirent *fp;
    while ((fp = readdir(dp))) {
      F file;
      file.name = fp->d_name;
      if (file.name[0] == '.')
        continue;
      struct stat buf;
      stat(file.name.c_str(), &buf);
      if (S_ISDIR(buf.st_mode))
        file.name += DIR_DELIM;
      if (file.name.substr(0, prefix.size()) == prefix)
        matches.push_back(file);
    }
    closedir(dp);
  }
  Completion(const Completion &prev) {
    dir = prev.dir;
    prefix = prev.prefix;
    matches = prev.matches;
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
};
