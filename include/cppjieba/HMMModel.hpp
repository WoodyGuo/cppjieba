#ifndef CPPJIEBA_HMMMODEL_H
#define CPPJIEBA_HMMMODEL_H

#include <iostream>
#include <unordered_map>
#include <vector>

#include "utils.h"

#include "Trie.hpp"

namespace cppjieba {

typedef unordered_map<Rune, double> EmitProbMap;

struct HMMModel {
  /*
   * STATUS:
   * 0: HMMModel::B, 1: HMMModel::E, 2: HMMModel::M, 3:HMMModel::S
   * */
  enum { B = 0, E = 1, M = 2, S = 3, STATUS_SUM = 4 };

  HMMModel(const string& modelPath) {
    memset(startProb, 0, sizeof(startProb));
    memset(transProb, 0, sizeof(transProb));
    statMap[0] = 'B';
    statMap[1] = 'E';
    statMap[2] = 'M';
    statMap[3] = 'S';
    emitProbVec.push_back(&emitProbB);
    emitProbVec.push_back(&emitProbE);
    emitProbVec.push_back(&emitProbM);
    emitProbVec.push_back(&emitProbS);
    LoadModel(modelPath);
  }
  ~HMMModel() {}
  void LoadModel(const string& filePath) {
    if (filePath.empty()) return;

    ifstream ifile(filePath.c_str());
    // XCHECK(ifile.is_open()) << "open " << filePath << " failed";
    string line;
    vector<string> tmp2;
    // Load startProb
    /** XCHECK */ (GetLine(ifile, line));
    vector<string> tmp = hola::Split(line, " ");
    // XCHECK(tmp.size() == STATUS_SUM);
    for (size_t j = 0; j < tmp.size(); j++) {
      startProb[j] = atof(tmp[j].c_str());
    }

    // Load transProb
    for (size_t i = 0; i < STATUS_SUM; i++) {
      // XCHECK(GetLine(ifile, line));
      GetLine(ifile, line);
      tmp = hola::Split(line, " ");
      // XCHECK(tmp.size() == STATUS_SUM);
      for (size_t j = 0; j < STATUS_SUM; j++) {
        transProb[i][j] = atof(tmp[j].c_str());
      }
    }

    // Load emitProbB
    /** XCHECK */ (GetLine(ifile, line));
    /** XCHECK */ (LoadEmitProb(line, emitProbB));

    // Load emitProbE
    /** XCHECK */ (GetLine(ifile, line));
    /** XCHECK */ (LoadEmitProb(line, emitProbE));

    // Load emitProbM
    /** XCHECK */ (GetLine(ifile, line));
    /** XCHECK */ (LoadEmitProb(line, emitProbM));

    // Load emitProbS
    /** XCHECK */ (GetLine(ifile, line));
    /** XCHECK */ (LoadEmitProb(line, emitProbS));
  }
  double GetEmitProb(const EmitProbMap* ptMp, Rune key, double defVal) const {
    EmitProbMap::const_iterator cit = ptMp->find(key);
    if (cit == ptMp->end()) {
      return defVal;
    }
    return cit->second;
  }
  bool GetLine(ifstream& ifile, string& line) {
    while (getline(ifile, line)) {
      hola::Trim(&line);
      if (line.empty()) {
        continue;
      }
      if (hola::StartsWith(line, "#")) {
        continue;
      }
      return true;
    }
    return false;
  }
  bool LoadEmitProb(const string& line, EmitProbMap& mp) {
    if (line.empty()) {
      return false;
    }
    vector<string> tmp2;
    Unicode unicode;
    auto tmp = hola::Split(line, ",");
    for (size_t i = 0; i < tmp.size(); i++) {
      tmp2 = hola::Split(tmp[i], ":");
      if (2 != tmp2.size()) {
        std::cerr << "emitProb illegal." << std::endl;
        return false;
      }
      if (!DecodeRunesInString(tmp2[0], unicode) || unicode.size() != 1) {
        std::cerr << "TransCode failed." << std::endl;
        return false;
      }
      mp[unicode[0]] = atof(tmp2[1].c_str());
    }
    return true;
  }

  char statMap[STATUS_SUM];
  double startProb[STATUS_SUM];
  double transProb[STATUS_SUM][STATUS_SUM];
  EmitProbMap emitProbB;
  EmitProbMap emitProbE;
  EmitProbMap emitProbM;
  EmitProbMap emitProbS;
  vector<EmitProbMap*> emitProbVec;
};  // struct HMMModel

}  // namespace cppjieba

#endif
