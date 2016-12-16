#pragma once

#include <cmath>
#include <vector>
#include <string>
#include <map>

#include "file_system.h"
#include "completion.h"

using namespace std;

#define BONUS 10.0

#define FRECENCY_FILE (string("frecency"))

class FrecencyCompletion : public Completion {
protected:
  map<string, double> frecency;
  map<string, long long> time;
  long long last_t;
  long long now_t;

public:
  double HALF_LIFE = 30 * 24 * 60 * 60;

  FrecencyCompletion(FileSystem &_fs, string frecency_file = DATA_DIR + FRECENCY_FILE) : Completion(_fs) {
    if (frecency_file.size()) {
      FILE *f = fopen(frecency_file.c_str(), "r");
      if (f != NULL) {
        char path[1024];
        double frec = 0;
        long long t = 0;
        fscanf(f, "%lld", &last_t);
        while (!feof(f)) {
          fscanf(f, "%s%lf%lld", path, &frec, &t);
          frecency[path] = frec;
          time[path] = t;
        }
        fclose(f);
      }
    }
  }

  void init(string path) {
    Completion::init(path);

    for (uint i = 0; i < matches.size(); i++) {
      matches[i].frecency = frecency[dir + "/" + matches[i].name];
    }
  }

  static bool compare_frecency(const File &x, const File &y) {
    return x.frecency > y.frecency || (x.frecency == y.frecency && x.name < y.name);
  }
  void sort() {
    std::sort(matches.begin(), matches.end(), compare_frecency);
  }

  void choose(string choice) {
    uint i = 0;
    for (uint i = 0; i < matches.size() && choice != matches[i].name; i++);
    if (i == matches.size())
      return;

    now_t = fs.get_time();
    /*
    long long diff_t = now_t - last_t;
    for (map<string, double>::iterator it = frecency.begin(); it != frecency.end(); it++) {
      it->second *= exp2(-((double) diff_t / HALF_LIFE));
    }
    */
    for (uint i = 0; i < matches.size(); i++) {
      frecency[dir + "/" + matches[i].name] *= exp2(-((double) (now_t - time[dir + "/" + matches[i].name]) / HALF_LIFE));
      time[dir + "/" + matches[i].name] = now_t;
    }

    frecency[dir + "/" + choice] += BONUS;

    if (!fs.is_virtual()) {
      save();
    }

    last_t = now_t;
  }

  void save() {
    string frecency_file;
    if (fs.is_virtual())
      frecency_file = DATA_DIR + "test-" + FRECENCY_FILE;
    else
      frecency_file = DATA_DIR + FRECENCY_FILE;

    FILE *f = fopen(frecency_file.c_str(), "w");
    if (f == NULL) {
      // TODO:
      return;
    }
    fprintf(f, "%lld\n", now_t);
    for (map<string, double>::iterator it = frecency.begin(); it != frecency.end(); it++) {
      fprintf(f, "%s %lf %lld\n", it->first.c_str(), it->second, time[it->first]);
    }
    fclose(f);
  }
};
