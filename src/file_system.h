#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <vector>
#include <string>
#include <map>

#define DIR_DELIM '/'

using namespace std;

class File {
public:
  string name;
  bool is_dir;
  map<string, File> files;
};

inline bool operator < (const File &x, const File &y) {
  return x.name < y.name;
}

typedef File LexFile;

class RevLexFile : public File {};
inline bool operator < (const RevLexFile &x, const RevLexFile &y) {
  return y.name < x.name;
}

template<typename F>
class FileSystem {
public:
  virtual vector<F> get_matches(string path) = 0;
};

template<typename F>
class RealFileSystem : public FileSystem<F> {
public:
  vector<F> get_matches(string path) {
    size_t found = path.find_last_of(DIR_DELIM);
    string dir = path.substr(0, found);
    string prefix = path.substr(found + 1);

    vector<F> matches;

    DIR *dp = opendir(dir.c_str());
    if (!dp)
      throw invalid_argument("cannot open the directory");

    struct dirent *fp;
    while ((fp = readdir(dp))) {
      F file;
      file.name = fp->d_name;
      if (file.name[0] == '.' && prefix[0] != '.')
        continue;
      struct stat buf;
      stat(file.name.c_str(), &buf);
      if (S_ISDIR(buf.st_mode))
        file.name += DIR_DELIM;
      if (file.name.substr(0, prefix.size()) == prefix)
        matches.push_back(file);
    }
    closedir(dp);

    return matches;
  }
};

template<typename F>
class VirtualFileSystem : public FileSystem<F> {
public:
  LexFile root;

  VirtualFileSystem() {
    root.is_dir = true;
  }

  vector<F> get_matches(string path) {
    size_t found = path.find_last_of(DIR_DELIM);
    string dir = path.substr(0, found);
    string prefix = path.substr(found + 1);

    vector<F> matches;
    string name;
    LexFile *f = &root;

    for (uint i = 1; i < path.size(); i += (name + DIR_DELIM).size()) {
      size_t found = path.find(DIR_DELIM, i);
      name = path.substr(i, found - i);

      if (i + name.size() == path.size())
        break;

      if (f->files.find(name) == f->files.end())
        throw invalid_argument("cannot open the directory");

      f = &(f->files[name]);
    }

    for (map<string, LexFile>::iterator it = f->files.begin(); it != f->files.end(); it++) {
      F file;
      file.name = it->second.name;
      if (file.name[0] == '.' && prefix[0] != '.')
        continue;
      if (file.name.substr(0, prefix.size()) == prefix)
        matches.push_back(file);
    }

    return matches;
  }

  void create_file(string path) {
    LexFile *f = &root;
    string name;
    for (uint i = 1; i < path.size(); i += (name + DIR_DELIM).size()) {
      size_t found = path.find(DIR_DELIM, i);
      name = path.substr(i, found - i);

      if (f->files.find(name) == f->files.end()) {
        LexFile file;
        file.name = name;
        file.is_dir = (i + name.size() != path.size());
        f->files[name] = file;
      }

      f = &(f->files[name]);
    }
  }
};
