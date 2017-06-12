/*
  The original version of read(), write(), bsearch(), compare()
  and search() functions are:

  Copyright (c) 2009 Daisuke Okanohara All Rights Reserved.

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation
  files (the "Software"), to deal in the Software without
  restriction, including without limitation the rights to use, copy,
  modify, merge, publish, distribute, sublicense, and/or sell copies
  of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.

  https://code.google.com/p/mini-se/
*/

#include <algorithm>
#include "sais.hxx"
#include "esa.hxx"
#include "esary.hpp"

namespace esary {

  ESary::ESary(){
  }

  ESary::~ESary(){
  }


  void ESary::addLine(std::vector<int>& line){
    for (std::vector<int>::iterator it = line.begin(); it != line.end(); ++it) {
      T.push_back((uint32_t)*it);
    }
    T.push_back(1);//add Delimiter
  }

  int ESary::build_esa(){
    int n = T.size();
    if(n==0)return -1;
    nodeNum = 0;
    int alphaSize = 0x110000;

    SA.clear();
    SA.resize(n);
    L.clear();
    L.resize(n);
    R.clear();
    R.resize(n);
    D.clear();
    D.resize(n);
    if (esaxx(T.begin(), SA.begin(),
              L.begin(), R.begin(),  D.begin(),
              n, alphaSize, nodeNum) == -1){
      return -1;
    }

    RANK.clear();
    RANK.resize(n);
    int r = 0;
    for(int i=0;i<n;i++) {
      if(i==0 || T[(SA[i]+n-1)%n] != T[(SA[i-1]+n-1)%n]) r++;
      RANK[i] = r;
   }
    return 0;
  }

  int ESary::build_sa(){
    int n = T.size();
    if(n==0)return -1;
    nodeNum = 0;
    int alphaSize = 0x110000;

    SA.clear();
    SA.resize(n);
    L.clear();
    R.clear();
    D.clear();
    if (saisxx(T.begin(), SA.begin(),
              n, alphaSize) == -1){
      return -1;
    }
    return 0;
  }

  void ESary::getResults(std::vector<uint32_t>& indexes, std::vector<std::pair<std::vector<int>*, std::vector<int>*> > & result){
    result.clear();
    for (std::vector<uint32_t>::iterator it = indexes.begin(); it != indexes.end(); ++it) {
      std::vector<int>* first = new std::vector<int>();
      std::vector<int>* second = new std::vector<int>();
      getPrefix(*it, first);
      getSuffix(*it, second);
      result.push_back(std::pair<std::vector<int>*, std::vector<int>*>(first, second));
    }
  }
  
  void ESary::freeResults(std::vector<std::pair<std::vector<int>*, std::vector<int>*> > & results){
    for (std::vector<std::pair<std::vector<int>*, std::vector<int>*> >::iterator it = results.begin(); it != results.end(); ++it) {
      delete it->first;
      delete it->second;
    }
  }

  void ESary::freeIntVec(std::vector<int>* vec){
    delete vec;
  }
  

  void ESary::getPrefix(const uint32_t index, std::vector<int>* ret){
    uint32_t start_idx = index;
    //search backward
    for(;start_idx>0;--start_idx){
      if(T[start_idx]==1){
        ++start_idx;
        break;
      }
    }

    if(start_idx>=index){
      return;
    }

    for(uint32_t i=start_idx;i<index;++i){
      ret->push_back((int)T[i]);
    }
    return;
  }

  void ESary::getSuffix(const uint32_t index, std::vector<int>* ret){
    uint32_t end_idx;
    //search forward
    for(end_idx=index;end_idx<T.size();++end_idx){
      if(T[end_idx]==1){
        --end_idx;
        break;
      }
    }

    for(uint32_t i =index;i<=end_idx;++i){
      ret->push_back((int)T[i]);
    }
    return;
  }


  void ESary::search(std::vector<int>& query, std::vector<uint32_t>& indexes){
    if (T.size()==0 || T.size()!=SA.size())return;
    std::vector<uint32_t> queryVec;
    for (std::vector<int>::iterator it = query.begin(); it != query.end(); ++it) {
      queryVec.push_back((uint32_t)*it);
    }
    
    indexes.clear();
    // Binary Search of the SA position containing a query as a prefix
    uint32_t beg    = 0;
    uint32_t size   = static_cast<uint32_t>(SA.size());
    uint32_t half   = size/2;
    uint32_t match  = 0;
    uint32_t lmatch = 0;
    uint32_t rmatch = 0;
    bsearch(queryVec, beg, half, size, match, lmatch, rmatch, 0);

    // Lower Bound
    uint32_t lbeg    = beg;
    uint32_t lsize   = half;
    uint32_t lhalf   = half / 2;
    uint32_t llmatch = lmatch;
    uint32_t lrmatch = match;
    uint32_t lmatch2 = 0;
    bsearch(queryVec, lbeg, lhalf, lsize, lmatch2, llmatch, lrmatch, 1);

    if(size==0)return;//if not found

  // Upper Bound
    uint32_t rbeg    = beg + half + 1;
    uint32_t rsize   = size - half - 1;
    uint32_t rhalf   = rsize / 2;
    uint32_t rlmatch = match;
    uint32_t rrmatch = rmatch;
    uint32_t rmatch2 = 0;
    bsearch(queryVec, rbeg, rhalf, rsize, rmatch2, rlmatch, rrmatch, 2);

    // SA[lbeg...rbeg) are matching positions;
    for (uint32_t i = lbeg; i < rbeg; ++i){
      indexes.push_back(SA[i]);
    }
    std::sort(indexes.begin(), indexes.end());
  }

  /// Return -1 if text[ind...] < query, or return 1 otherwise
  int ESary::compare(const uint32_t ind, const std::vector<uint32_t>& query, uint32_t& match) const{
    while (match < query.size() && match + ind < T.size()){
      if (T[ind+match] != query[match]){
        return (int)T[ind+match] - query[match];
      }
      ++match;
    }
    if (match + ind == T.size()){
      return -1;
    } else { // match == query.size()
      return 0;
    }
  }

  void ESary::bsearch(const std::vector<uint32_t>& query,
                      uint32_t& beg, uint32_t& half, uint32_t& size, 
                      uint32_t& match, uint32_t& lmatch, uint32_t& rmatch, 
                      const int state){
    half = size/2;
    for (; size > 0; size = half, half /= 2){
      match = std::min(lmatch, rmatch);
      int r = compare(SA[beg + half], query, match);
      if (r < 0 || (r == 0 && state==2)){
        beg += half + 1;
        half -= (1 - (size & 1));
        lmatch = match;
      } else if (r > 0 || state > 0){
        rmatch = match;
      } else {
        // T[SA[beg+half]...]'s prefix equals to query
        break;
      }
    }
  }


  std::pair<std::vector<int>*, uint32_t> ESary::findMaximalSubstring(uint32_t& pos){
    for (; pos < nodeNum; pos++){
      uint32_t c = RANK[R[pos]-1] - RANK[L[pos]];
      int len = D[pos];
      if (len > 0 && c > 0) {
        int flag = 0;
        int start = SA[L[pos]];
        std::vector<int>* ret = new std::vector<int>();
        for(uint32_t i=start;i<start+len;++i){
          if(T[i]==1 && (i != start || i != start+len-1)){
            flag=1;
            break;
          }
          if (T[i]!=1){
            ret->push_back((int)T[i]);
          }
        }

        if(flag==1 or ret->size() == 0){
          delete ret;
          continue;
        }
        pos++;
        return std::pair<std::vector<int>*, int>(ret,c+1);
      }
    }
    return std::pair<std::vector<int>*, int>(new std::vector<int>(), 0);
  }

  int ESary::save(const char* fileName){
    std::ofstream ofs(fileName);
    if (!ofs){
      what_ << "cannot open " << fileName;
      return -1;
  }
    if (write(nodeNum, "nodeNum", ofs) == -1) return -1;
    if (write(T, "T", ofs) == -1) return -1;
    if (write(SA, "SA", ofs) == -1) return -1;
    if (write(L, "L", ofs) == -1) return -1;
    if (write(R, "R", ofs) == -1) return -1;
    if (write(D, "D", ofs) == -1) return -1;
    ofs.close();
    return 0;
  }

  int ESary::load(const char* fileName,int flag){
    std::ifstream ifs(fileName);
    if (!ifs){
      what_ << "cannot open " << fileName;
      return -1;
    }

    T.clear();
    SA.clear();
    L.clear();
    R.clear();
    D.clear();
    if (read(nodeNum, "nodeNum", ifs) == -1) return -1;
    if(flag==1&&nodeNum==0){
      what_ << "This file is not for Enhanced Suffix Array" << std::endl;
      return -1;
    }
    if (read(T, "T", ifs) == -1) return -1;
    if (read(SA, "SA", ifs) == -1) return -1;
    if (read(L, "L", ifs) == -1) return -1;
    if (read(R, "R", ifs) == -1) return -1;
    if (read(D, "D", ifs) == -1) return -1;
    return 0;
  }

}//sarray
