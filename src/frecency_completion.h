#pragma once

#include <cmath>
#include <vector>
#include <string>
#include <map>

#include "file_system.h"
#include "completion.h"

using namespace std;

#define BONUS 10.0
#define HALF_LIFE 60.0

#define FRECENCY_FILE (string("frecency"))

class FrecencyFile : public LexFile {
public:
  double frecency;
};
inline bool operator < (const FrecencyFile &x, const FrecencyFile &y) {
  return x.frecency > y.frecency || (x.frecency == y.frecency && (LexFile)x < (LexFile)y);
}

class FrecencyCompletion : public Completion<FrecencyFile> {
protected:
  map<string, double> frecency;
  long long last_t;

public:
  FrecencyCompletion(FileSystem<FrecencyFile> &_fs, string path) : Completion<FrecencyFile>(_fs, path) {
    FILE *f = fopen((DATA_DIR + FRECENCY_FILE).c_str(), "r");
    if (f != NULL) {
      char path[1024];
      double frec = 0;
      fscanf(f, "%lld", &last_t);
      while (!feof(f)) {
        fscanf(f, "%s%lf", path, &frec);
        frecency[path] = frec;
      }
      fclose(f);
    }

    for (uint i = 0; i < matches.size(); i++) {
      matches[i].frecency = frecency[dir + "/" + matches[i].name];
    }
  }

  vector<File> get_matches() {
    sort(matches.begin(), matches.end());
    vector<File> result;
    for (uint i = 0; i < matches.size(); i++)
      result.push_back(matches[i]);
    return result;
  }

  void choose(string choice) {
    uint i = 0;
    for (uint i = 0; i < matches.size() && choice != matches[i].name; i++);
    if (i == matches.size())
      return;

    long long now_t = this->get_time();
    long long diff_t = now_t - last_t;
    for (map<string, double>::iterator it = frecency.begin(); it != frecency.end(); it++) {
      it->second *= exp2(-((double) diff_t / HALF_LIFE));
    }
    frecency[dir + "/" + choice] += BONUS;

    FILE *f = fopen((DATA_DIR + FRECENCY_FILE).c_str(), "w");
    if (f == NULL) {
      // TODO:
      return;
    }
    fprintf(f, "%lld\n", now_t);
    for (map<string, double>::iterator it = frecency.begin(); it != frecency.end(); it++) {
      fprintf(f, "%s %lf\n", it->first.c_str(), it->second);
    }
    fclose(f);
  }
};
