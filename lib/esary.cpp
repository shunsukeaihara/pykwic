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


  void ESary::addLine(const char* line){
    //from http://shogo82148.hatenablog.com/entry/20110916/1316172382
    std::vector<uint32_t> ucsvec;
    std::string utf8string(line);
    strutftoucs(utf8string,ucsvec);

    for (std::vector<uint32_t>::iterator it = ucsvec.begin(); it != ucsvec.end(); ++it) {
      T.push_back(*it);
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

  void ESary::getResult(std::vector<uint32_t>& indexes, std::vector<std::pair<std::string,std::string> > & result){
    result.clear();
    for (std::vector<uint32_t>::iterator it = indexes.begin(); it != indexes.end(); ++it) {
      result.push_back(std::pair<std::string,std::string>(getPrefix(*it),getSuffix(*it)));
    }
  }

  std::string ESary::getPrefix(const uint32_t index){
    uint32_t start_idx = index;
    //search backward
    for(;start_idx>0;--start_idx){
      if(T[start_idx]==1){
        ++start_idx;
        break;
      }
    }

    if(start_idx>=index){
      return "";
    }

    std::vector<uint32_t> ucsvec;
    for(uint32_t i =start_idx;i<index;++i){
      ucsvec.push_back(T[i]);
    }
    std::string result;
    strucstoutf(ucsvec,result);
    return result;
  }



  void ESary::getResultSuffix(std::vector<uint32_t>& indexes,std::vector<std::string> & result){
    result.clear();
    for (std::vector<uint32_t>::iterator it = indexes.begin(); it != indexes.end(); ++it) {
      result.push_back(getSuffix(*it));
    }
  }

  std::string ESary::getSuffix(const uint32_t index){
    uint32_t end_idx;
    //search forward
    for(end_idx=index;end_idx<T.size();++end_idx){
      if(T[end_idx]==1){
        --end_idx;
        break;
      }
    }

    std::vector<uint32_t> ucsvec;
    for(uint32_t i =index;i<=end_idx;++i){
      ucsvec.push_back(T[i]);
    }
    std::string result;
    strucstoutf(ucsvec,result);
    return result;
  }


  void ESary::search(const char* query, std::vector<uint32_t>& indexes){
    if (T.size()==0 || T.size()!=SA.size())return;
    std::vector<uint32_t> queryVec;
    std::string utf8string(query);
    strutftoucs(utf8string,queryVec);

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


  std::pair<std::string,uint32_t> ESary::findMaximalSubstring(uint32_t& pos){
    for (; pos < nodeNum; pos++){
      uint32_t c = RANK[R[pos]-1] - RANK[L[pos]];
      if (D[pos] > 0 && c > 0) {
        int flag = 0;

        std::vector<uint32_t> ucsvec;
        for(uint32_t i =SA[L[pos]];i<SA[L[pos]]+D[pos];++i){
          if(T[i]==1){
            flag=1;
            break;
          }
          ucsvec.push_back(T[i]);
        }

        if(flag==1)continue;
        std::string s;
        strucstoutf(ucsvec,s);
        ++pos;
        return std::pair<std::string,int>(s,c+1);
      }
    }
    return std::pair<std::string,int>(std::string(),0);
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

  /**
   * Convert a UTF-8 string into a UCS-4 vector.
   */
  void ESary::strutftoucs(const std::string& src,
                          std::vector<uint32_t>& dest) {
    size_t size = src.size();
    size_t ri = 0;
    while (ri < size) {
      uint32_t c = (unsigned char)src[ri];
      if (c >= 0xc0 && c < 0x80) {
        dest.push_back(c);
      } else if (c < 0xe0) {
        if (ri + 1 < size) {
          dest.push_back(((c & 0x1f) << 6) | (src[ri+1] & 0x3f));
          ri++;
        }
      } else if (c < 0xf0) {
      if (ri + 2 < size) {
        dest.push_back(((c & 0x0f) << 12) | ((src[ri+1] & 0x3f) << 6) |
                        (src[ri+2] & 0x3f));
        ri += 2;
      }
      } else if (c < 0xf8) {
        if (ri + 3 < size) {
          dest.push_back(((c & 0x07) << 18) | ((src[ri+1] & 0x3f) << 12) |
                          ((src[ri+2] & 0x3f) << 6) | (src[ri+3] & 0x3f));
          ri += 3;
        }
      } else if (c < 0xfc) {
        if (ri + 4 < size) {
          dest.push_back(((c & 0x03) << 24) | ((src[ri+1] & 0x3f) << 18) |
                          ((src[ri+2] & 0x3f) << 12) | ((src[ri+3] & 0x3f) << 6) |
                          (src[ri+4] & 0x3f));
        ri += 4;
        }
      } else if (c < 0xfe) {
        if (ri + 5 < size) {
          dest.push_back(((c & 0x01) << 30) | ((src[ri+1] & 0x3f) << 24) |
                          ((src[ri+2] & 0x3f) << 18) | ((src[ri+3] & 0x3f) << 12) |
                          ((src[ri+4] & 0x3f) << 6) | (src[ri+5] & 0x3f));
          ri += 5;
        }
      }
      ri++;
    }
  }

  /**
   * Convert a UCS-4 vector into a UTF-8 string.
   */
  void ESary::strucstoutf(const std::vector<uint32_t>& src,
                          std::string& dest) {
    std::vector<uint32_t>::const_iterator it = src.begin();
    std::vector<uint32_t>::const_iterator itend = src.end();
    while (it != itend) {
      uint32_t c = *it;
      if (c < 0x80) {
        dest.append(1, c);
      } else if (c < 0x800) {
        dest.append(1, 0xc0 | (c >> 6));
        dest.append(1, 0x80 | (c & 0x3f));
      } else if (c < 0x10000) {
        dest.append(1, 0xe0 | (c >> 12));
        dest.append(1, 0x80 | ((c & 0xfff) >> 6));
        dest.append(1, 0x80 | (c & 0x3f));
      } else if (c < 0x200000) {
        dest.append(1, 0xf0 | (c >> 18));
        dest.append(1, 0x80 | ((c & 0x3ffff) >> 12));
        dest.append(1, 0x80 | ((c & 0xfff) >> 6));
        dest.append(1, 0x80 | (c & 0x3f));
      } else if (c < 0x4000000) {
        dest.append(1, 0xf8 | (c >> 24));
        dest.append(1, 0x80 | ((c & 0xffffff) >> 18));
        dest.append(1, 0x80 | ((c & 0x3ffff) >> 12));
        dest.append(1, 0x80 | ((c & 0xfff) >> 6));
        dest.append(1, 0x80 | (c & 0x3f));
      } else if (c < 0x80000000) {
        dest.append(1, 0xfc | (c >> 30));
        dest.append(1, 0x80 | ((c & 0x3fffffff) >> 24));
        dest.append(1, 0x80 | ((c & 0xffffff) >> 18));
        dest.append(1, 0x80 | ((c & 0x3ffff) >> 12));
        dest.append(1, 0x80 | ((c & 0xfff) >> 6));
        dest.append(1, 0x80 | (c & 0x3f));
      }
      ++it;
    }
  }

}//sarray
