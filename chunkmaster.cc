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
#include <iostream>
#include <cstring>
#include <iomanip>
#include <cassert>
#include <bitset>
#include <initializer_list>
#include <array>
#include "test.h"
#include "utility.h"
#include "statemaker.h"
#include "chunkmaster.h"

using std::uint32_t;
using std::cout;
using std::endl;
using std::strlen;
using std::setw;
using std::right;
using std::bitset;
using std::hex;
using std::dec;
using std::setfill;
using std::initializer_list;
using std::array;
using std::ios;

ChunkMaster::ChunkMaster(const void* msg, size_t len, bool d) : debug(d), buffer(nullptr), numChunks(0), binary(true), numLocalChunks(1) {
   Initialize(msg, len);
}

// This function creates up to two 64 byte chunks that
// stores padded input data
// The second local chunk is created if the first one doesn't have enough
// space for the "1" bit and the 32 bit data len
// see sha256 spec for details
// KNOWN BUG: assumes data len (in bits) % 8 = 0
void ChunkMaster::Initialize(const void* msg, uint64_t len) {
   static const size_t minNeeded = 9;
   uint64_t bitlen = len * 8;
   numChunks = (len / 64) + 1;
   uint64_t inLastBlock = (len % 64);
   uint64_t leftover = 64 - inLastBlock;

   if (leftover < minNeeded) {
      ++numLocalChunks;
      ++numChunks;
   }
   buffer = static_cast<const char*>(msg);
   for (size_t i=0; i<numLocalChunks; ++i) {
      memset(localChunk[i], 0, 64);
   }
   if (inLastBlock > 0) {
      memcpy(localChunk[0], &buffer[len-inLastBlock], inLastBlock);
   }
   if (numLocalChunks == 1) {
      localChunk[0][inLastBlock] = 0x80;
      memcpy(localChunk[0]+56, &bitlen, 8);
   } else {
      if (leftover > 0) {
	 localChunk[0][inLastBlock] = 0x80;
      } else {
	 localChunk[1][0] = 0x80;
      }
      memcpy(localChunk[1]+56, &bitlen, 8);
   }
}

// gets a pointer to the chunk using the given index
// if the index is the last chunk then the paded chunk is returned
const char* ChunkMaster::GetChunk(size_t index) {
   assert(index<numChunks);
   if (index >= (numChunks - numLocalChunks)) {
      return localChunk[index - (numChunks - numLocalChunks)];
   }
   else {
      return buffer + (index*64);
   }
}

size_t ChunkMaster::NumChunks() {
   return numChunks;
}

// Debugging functions below
void ChunkMaster::Dump() {
   if (!debug) {
      return;
   }
   cout << "Num Chunks: " << numChunks << endl;
   for (size_t i = 0; i<numChunks; ++i) {
      DumpChunk(i);
   }
}

void ChunkMaster::DumpLine(const void* p) {
   uint32_t data[4];
   memcpy(data, p, 16);
   cout.unsetf(ios::showbase);   
   for (size_t j=0; j<4; ++j) {
      cout << hex << setw(8) << setfill('0') << data[j];
      if (j != 3) {
	 cout << " ";
      }
   }
   cout << endl;   
}

void ChunkMaster::DumpChunk(size_t index) {
   const char* p = GetChunk(index);
   for (size_t i=0; i<4; ++i) {
      DumpLine(p + (i*16));
   }      
}

