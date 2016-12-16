#pragma once

#include <fstream>
#include <cmath>
#include <vector>
#include <string>
#include <map>

#include "symbreg.h"
#include "file_system.h"
#include "completion.h"

using namespace std;

#define BONUS 1

#define FRECENCY_FILE (string("frecency"))
#define FRECENCY_SETTING (string("frecency.conf"))

class FrecencySettings {
public:
  static Node *symbreg;
  static void init() {
    ifstream f;
    string rpn;

    f.open(DATA_DIR + FRECENCY_SETTING);
    if (!f.is_open()) {
      ofstream ff;

      ff.open(DATA_DIR + FRECENCY_SETTING);
      ff << "x1" << endl;
      ff.close();

      f.open(DATA_DIR + FRECENCY_SETTING);
    }
    getline(f, rpn);
    symbreg = Node::parse(rpn);
    f.close();
  }
};

class FrecencyCompletion : public Completion {
protected:
  map<string, double> frecency;
  map<string, long long> time;
  long long last_t;
  long long now_t;

public:
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
    if (FrecencySettings::symbreg == NULL) {
      FrecencySettings::init();
    }
    map<string, double> mapping;
    for (uint i = 0; i < matches.size(); i++) {
      mapping["x1"] = frecency[dir + "/" + matches[i].name];
      if (time.find(dir + "/" + matches[i].name) == time.end())
        mapping["x2"] = 0;
      else
        mapping["x2"] = (double) (now_t - time[dir + "/" + matches[i].name]) / 86400;

      frecency[dir + "/" + matches[i].name] = FrecencySettings::symbreg->eval(mapping);
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
