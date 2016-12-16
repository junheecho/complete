#pragma once

#include <time.h>
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
  double frecency;
  long long time;
};

class FileSystem {
public:
  virtual vector<File> get_matches(string path) = 0;
  virtual long long get_time() = 0;
  virtual bool is_virtual() = 0;
};

class RealFileSystem : public FileSystem {
public:
  vector<File> get_matches(string path) {
    size_t found = path.find_last_of(DIR_DELIM);
    string dir = path.substr(0, found);
    string prefix = path.substr(found + 1);

    vector<File> matches;

    DIR *dp = opendir(dir.c_str());
    if (!dp)
      throw invalid_argument("cannot open the directory");

    struct dirent *fp;
    while ((fp = readdir(dp))) {
      File file;
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

  bool is_virtual() {
    return false;
  }
  long long get_time() {
    return time(NULL);
  }
};

class VirtualFileSystem : public FileSystem {
public:
  File root;

  long long curr_time = 0;

  VirtualFileSystem() {
    root.is_dir = true;
  }

  vector<File> get_matches(string path) {
    size_t found = path.find_last_of(DIR_DELIM);
    string dir = path.substr(0, found);
    string prefix = path.substr(found + 1);

    vector<File> matches;
    string name;
    File *f = &root;

    for (uint i = 1; i < path.size(); i += (name + DIR_DELIM).size()) {
      size_t found = path.find(DIR_DELIM, i);
      name = path.substr(i, found - i);

      if (i + name.size() == path.size())
        break;

      if (f->files.find(name) == f->files.end())
        throw invalid_argument("cannot open the directory");

      f = &(f->files[name]);
    }

    for (map<string, File>::iterator it = f->files.begin(); it != f->files.end(); it++) {
      File file;
      file.name = it->second.name;
      if (file.name[0] == '.' && prefix[0] != '.')
        continue;
      if (it->second.is_dir)
        file.name += DIR_DELIM;
      if (file.name.substr(0, prefix.size()) == prefix)
        matches.push_back(file);
    }

    return matches;
  }

  void create_file(string path) {
    File *f = &root;
    string name;
    for (uint i = 1; i < path.size(); i += (name + DIR_DELIM).size()) {
      size_t found = path.find(DIR_DELIM, i);
      name = path.substr(i, found - i);

      if (f->files.find(name) == f->files.end()) {
        File file;
        file.name = name;
        file.is_dir = (i + name.size() != path.size());
        f->files[name] = file;
      }

      f = &(f->files[name]);
    }
  }

  bool is_virtual() {
    return true;
  }
  void set_time(long long time) {
    curr_time = time;
  }
  long long get_time() {
    return curr_time;
  }
};
