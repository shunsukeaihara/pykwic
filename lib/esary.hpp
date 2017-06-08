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


#ifndef CPP_ESARY_H_
#define CPP_ESARY_H_

#include "stdint.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace esary{

  class ESary{
  public:
    ESary();
    ~ESary();
    void addLine(std::vector<int>& line);
    int build_sa();
    int build_esa();
    void search(std::vector<int>& query, std::vector<uint32_t>& indexes);
    void getResults(std::vector<uint32_t>& indexes, std::vector<std::pair<std::vector<int>*, std::vector<int>*> > & result);
    void freeResults(std::vector<std::pair<std::vector<int>*, std::vector<int>*> > & results);

    void freeIntVec(std::vector<int>* vec);
    
    int load(const char* fileName,int flag);
    int save(const char* fileName);

    int getNodeNum(){
      return nodeNum;
    }

    std::pair<std::vector<int>*,uint32_t> findMaximalSubstring(uint32_t& pos);

  private:
    std::vector<uint32_t> T;
    std::vector<uint32_t>  SA;     // suffix array
    std::vector<uint32_t>  L;     // left boundaries of internal node
    std::vector<uint32_t>  R;     // right boundaries of internal node
    std::vector<uint32_t>  D;     // depths of internal node
    std::vector<uint32_t> RANK;
    int nodeNum;
    std::ostringstream what_;


    void bsearch(const std::vector<uint32_t>& query,
                 uint32_t& beg, uint32_t& half, uint32_t& size,
                 uint32_t& match, uint32_t& lmatch, uint32_t& rmatch,
                 const int state);
    int compare(const uint32_t ind, const std::vector<uint32_t>& query, uint32_t& match) const;

    void getPrefix(const uint32_t index, std::vector<int>* ret);
    void getSuffix(const uint32_t index, std::vector<int>* ret);

    template<class T> int write(const std::vector<T>& v, const char* vname, std::ofstream& ofs){
      uint32_t size = static_cast<uint32_t>(v.size());
      if (write(size, vname, ofs) == -1) return -1;
      for (size_t i = 0; i < size; ++i){
        if (write(v[i], vname, ofs) == -1){
          return -1;
        }
      }
      return 0;
    }

    template<class T> int write(const T v, const char* vname, std::ofstream& ofs){
      if (!ofs.write((const char*)(&v), sizeof(int32_t))){
        what_ << "write error:" << vname;
        return -1;
      }
      return 0;
    }

    template<class T> int read(std::vector<T>& v, const char* vname, std::ifstream& ifs){
      uint32_t size = 0;
      if (read(size, vname, ifs) == -1) return -1;
      if (size == 0) return 0;
      v.resize(size);
      for (size_t i = 0; i < size; ++i){
        if (read(v[i], vname, ifs) == -1){
          return -1;
        }
      }
      return 0;
    }

    template<class T> int read(T& v, const char* vname, std::ifstream& ifs){
      if (!ifs.read((char*)(&v), sizeof(int32_t))){
        what_ << "read error:" << vname;
        return -1;
      }
      return 0;
    }

  }; //ESary
} //esary


#endif  // CPP_ESARY_H_
