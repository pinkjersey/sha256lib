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
#include "test.h"
#include "utility.h"
#include "statemaker.h"

using std::array;
using std::cout;
using std::endl;
using std::strlen;

int checkResults(const array<uint32_t, 8>& expectedResults, StateMaker& sm) {
   bool match = true;
   size_t c = 0;
   for (auto i=expectedResults.begin(); i!=expectedResults.end(); ++i) {
      if (*i != sm.get(c)) {
	 match = false;
	 break;
      }
      ++c;
   }
   if (!match) {
      cout << "Test failed" << endl;
      sm.Dump();
      return 1;
   }
   return 0;
}

int testcasebinary(void* msg, size_t len, const array<uint32_t, 8>& expectedResults, bool sha=true) {
   StateMaker sm;
   if (sha == true) {
      Sha256(msg, len, sm.GetState());
   } else {
      Sha_d256(msg, len, sm.GetState());
   }

   return checkResults(expectedResults, sm);   
}

int testcase(char* msg, const array<uint32_t, 8>& expectedResults, bool sha=true) {         
   StateMaker sm;
   if (sha == true) {
      Sha256(msg, strlen(msg), sm.GetState());
   } else {
      Sha_d256(msg, strlen(msg), sm.GetState());
   }

   return checkResults(expectedResults, sm);   
}

int test() {
   int ret = 0;

   if (ret == 0) {
      ret = testcase("abc", 
		     {0xba7816bf,0x8f01cfea,0x414140de,0x5dae2223,0xb00361a3,0x96177a9c,0xb410ff61,0xf20015ad});
      if (ret != 0) {
	 cout << "Test 1 failed" << endl;
      }
   }

   if (ret == 0) {
      ret = testcase("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 
		     {0x248d6a61,0xd20638b8,0xe5c02693,0xc3e6039,0xa33ce459,0x64ff2167,0xf6ecedd4,0x19db06c1});
      if (ret != 0) {
	 cout << "Test 2 failed" << endl;
      }
   }

   if (ret == 0) {
      ret = testcase("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq12345678", 
		     {0xe07f00c1,0xd434c9dc,0x274407d,0xd3f2665,0x1e64145e,0x2ffb0782,0x4601582e,0x38fed622});
      if (ret != 0) {
	 cout << "Test 3 failed" << endl;
      }
   }

   if (ret == 0) {
      char buf[1000001];
      buf[1000000] = 0;
      for (size_t i=0; i<1000000; ++i) {
	 buf[i] = 'a';
      }
      ret = testcase(buf, {0xcdc76e5c,0x9914fb92,0x81a1c7e2,0x84d73e67,0xf1809a48,0xa497200e,0x046d39cc,0xc7112cd0});
      if (ret != 0) {
	 cout << "Test 4 failed" << endl;
      }
   }

   if (ret == 0) {
      ret = testcase("abc", 
		     {0x4f8b42c2,0x2dd3729b,0x519ba6f6,0x8d2da7cc,0x5b2d606d,0x5daed5a,0xd5128cc0,0x3e6c6358}, false);
      if (ret != 0) {
	 cout << "Test 5 failed" << endl;
      }
   }   


   if (ret == 0) {
      uint32_t header[20] = {0x01000000, 0x81cd02ab, 0x7e569e8b, 0xcd9317e2, 0xfe99f2de, 0x44d49ab2, 0xb8851ba4, 0xa3080000, 0x00000000, 0xe320b6c2, 0xfffc8d75, 0x0423db8b, 0x1eb942ae, 0x710e951e, 0xd797f7af, 0xfc8892b0, 0xf1fc122b, 0xc7f5d74d, 0xf2b9441a, 0x42a14695};
      ret = testcasebinary(header, 80,
			   {0x1dbd981f,0xe6985776,0xb644b173,0xa4d0385d,0xdc1aa2a8,0x29688d1e,00000000,00000000}, false);

      if (ret != 0) {
	 cout << "Test 6 failed" << endl;
      }
   }

   return ret;
}
