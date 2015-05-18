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
#include <cstring>
#include "chunkmaster.h"
#include "utility.h"
#include "statemaker.h"

extern "C" {
void SaveState(uint32_t* state);
void LoadState(uint32_t* state);
void ProcMsg(char* chunk);
}

void Sha256(void* buf, size_t len, uint32_t* state) {   
   memcpy(state, getInitialHash(), sizeof(uint32_t) *8);
   ChunkMaster cm(buf, len, false);
   size_t numChunks = cm.NumChunks();
   LoadState(state);
   for (size_t i=0; i<numChunks; ++i) {
      ProcMsg(const_cast<char*>(cm.GetChunk(i)));
   }
   SaveState(state);
}

void Sha_d256(void* buf, size_t len, uint32_t* state) {
   StateMaker tState;
   Sha256(buf, len, tState.GetState());
   ChunkMaster cm(reinterpret_cast<char*>(tState.GetState()), 32, false);
   size_t numChunks = cm.NumChunks();
   LoadState(state);
   for (size_t i=0; i<numChunks; ++i) {
      ProcMsg(const_cast<char*>(cm.GetChunk(i)));
   }
   SaveState(state);
}
