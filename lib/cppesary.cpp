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
#include "cppesary.hpp"


namespace esary {

  CPPESary::CPPESary(){
  }

  CPPESary::~CPPESary(){
  }


  void CPPESary::addLine(const char* line){
    //from http://shogo82148.hatenablog.com/entry/20110916/1316172382
    UnicodeString str(line, "UTF-8");
    StringCharacterIterator it(str);
    for(UChar32 uc = it.first32(); uc != it.DONE; uc = it.next32()) {
      T.push_back(uc);
      Term.insert(uc);
    }
    T.push_back(1);//add Delimiter
  }

  int CPPESary::build(){
    int n = T.size();
    if(n==0)return -1;
    nodeNum = 0;
    if(Term.size==0){
      int alphaSize = 0x110000;
    }else{
      int alphaSize = Term.size()+1;  // TermSize
    }
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

  void CPPESary::getResult(const std::vector<uint_32_t>& indexes,std::vector<std::string> & result){
    result.clear();
    for (std::vector<uint_32_t>::iterator it = indexes.begin(); it != indexes.end(); ++it) {
      result.push_back(getLine(*it));
    }
  }


  std::string CPPESary::getLine(uint_32_t index){
    int start_idx;
    int end_idx;
    //search backward
    for(start_idx=index;idx>=0;--start_idx){
      if(T[start_idx]==1){
        --start__idx;
        break;
      }
    }
    //search forward
    for(end_idx=index;end_idx<T.size();++end_idx){
      if(T[end_idx]==1){
        --end_idx;
        break;
      }
    }
    UnicodeString us;
    for(int i =start_idx;i<=end_idx;++i){
      us+=T[i];
    }

    int32_t convertedLength = us.extract(0, us.length(), 0, "UTF-8");
    char* result = new char[convertedLength + 1];
    us.extract(0, us.length(), result, "UTF-8");
    std::string s=result;
    delete[] result;
    return s;
  }

  void CPPESary::search(const char* query, std::vector<uint_32_t>& indexes){
    UnicodeString str(query, "UTF-8");
    StringCharacterIterator it(str);
    std::vector<UChar32> queryVec;
    for(UChar32 uc = it.first32(); uc != it.DONE; uc = it.next32()) {
      queryVec.push_back(uc);
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
    bsearch(query, lbeg, lhalf, lsize, lmatch2, llmatch, lrmatch, 1);

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
  int CPPESary::compare(const uint32_t ind, const vector<UChar32>& query, uint32_t& match) const{
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

  void CPPESary::bsearch(const vector<UChar32>& query,
                      uint32_t& beg, uint32_t& half, uint32_t& size, 
                      uint32_t& match, uint32_t& lmatch, uint32_t& rmatch, 
                      const int state){
    half = size/2;
    for (; size > 0; size = half, half /= 2){
      match = min(lmatch, rmatch);
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

  int CPPESary::save(const char* fileName){
    std::ofstream ofs(fileName);
    if (!ofs){
      what_ << "cannot open " << fileName;
      return -1;
  }
    if (write(T, "T", ofs) == -1) return -1;
    if (write(SA, "SA", ofs) == -1) return -1;
    if (write(L, "L", ofs) == -1) return -1;
    if (write(R, "R", ofs) == -1) return -1;
    if (write(D, "D", ofs) == -1) return -1;
    if (write(nodeNum, "nodeNum", ofs) == -1) return -1;
    return 0;
  }

  int CPPESary::load(const char* fileName){
    ifstream ifs(fileName);
    if (!ifs){
      what_ << "cannot open " << fileName;
      return -1;
    }
    if (read(T, "T", ifs) == -1) return -1;
    if (read(SA, "SA", ifs) == -1) return -1;
    if (read(L, "L", ifs) == -1) return -1;
    if (read(R, "R", ifs) == -1) return -1;
    if (read(D, "D", ifs) == -1) return -1;
    if (read(nodeNum, "nodeNum", ifs) == -1) return -1;
    return 0;
  }


}//sarray
