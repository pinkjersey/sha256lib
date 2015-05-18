/*
The MIT License (MIT)

Copyright (c) 2015 pinkjersey

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include <cstdint>
#include <array>
#include <iostream>
#include <cstring>
#include <iomanip>
#include <bitset>
#include "statemaker.h"
#include "utility.h"

using std::array;
using std::cout;
using std::endl;
using std::strlen;
using std::uint32_t;
using std::bitset;
using std::ios;
using std::setw;
using std::hex;
using std::dec;
using std::setfill;

const static uint32_t sha256_initial_hash_value[8] = {
   0x6a09e667UL,0xbb67ae85UL,0x3c6ef372UL, 0xa54ff53aUL,
   0x510e527fUL,0x9b05688cUL,0x1f83d9abUL, 0x5be0cd19UL
};

const uint32_t* getInitialHash() { return sha256_initial_hash_value; }

StateMaker::StateMaker() : _state(nullptr)
{
   _state = _stack;
   memcpy(_state, sha256_initial_hash_value, sizeof(uint32_t) *8);
}

StateMaker::StateMaker(uint32_t* instate) : _state(instate) {
   memcpy(_state, sha256_initial_hash_value, sizeof(uint32_t) *8);
}

StateMaker::~StateMaker() {
   _state = nullptr;
}

uint32_t* StateMaker::GetState() { return _state; }

void StateMaker::DumpState(size_t index) {
   char* p = reinterpret_cast<char*>(&_state[index]);
   cout << "    ";
   for (size_t i=0; i<4; ++i) {
      bitset<8> bits(*p);
      cout << bits << " ";      
      ++p;
   }
   cout << endl;
}
   

void StateMaker::Dump() {
   cout << "STATE DUMP" << endl;
   cout << "{";
   for (size_t i = 0; i<8; ++i) {
      cout.setf(ios::showbase);
      cout << hex << setw(8) << setfill('0') << _state[i];
      if (i!=7) {
	 cout << ",";
      }
   }
   cout << "}" << endl;
}

uint32_t StateMaker::get(size_t index) { return _state[index]; }
