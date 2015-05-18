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

#ifndef CHUNK_MASTER_H
#define CHUNK_MASTER_H

//
// This class is responsible for converting the sha256 input
// into 64 byte chunks. It is responsible for padding the input
// per sha256 specs
// 

class ChunkMaster {
public:
   ChunkMaster(const void* msg, uint64_t len, bool d);
   size_t NumChunks();
   const char* GetChunk(size_t index);
   void Dump();
   void DumpChunk(size_t index);
private:
   void DumpLine(const void* p);
   void Initialize(const void* msg, uint64_t len);
   bool debug;
   bool binary;
   const char* buffer;
   size_t numChunks;
   size_t numLocalChunks;
   char localChunk[2][64];   
};

#endif
