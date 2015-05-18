# sha256lib

The purpose of this library is to wrap around the SPARC sha256 op code and allow the user to rapidly generate sha256 hashes.

The sha256 operation uses the data (state and message) stored in floating point registers to generate the hash. The process is as follows:

* Load the 32 byte initial state (hardcoded) into f0, f1, f2 and f3
* Load the 64 byte message into f4, f5, f6, f7, f8, f9, f10, f11, f12
* Call the sha256 op code (f0, f1, f2, f3) contains the new state
* Load more 64 byte chunks as needed, call sha256
* The result is stored in f0, f1, f2, f3, copy it back into memory

The library exposes the following functions:

```
void Sha256(void* buf, std::size_t len, uint32_t* state);
void Sha_d256(void* buf, std::size_t len, uint32_t* state);
int test()
```

Sha256 takes a pointer to the data to be hashed, the length of the data in bytes and a pointer to an array of 8 uint32_t integers that contains the resulting hash.

Sha_d256 takes the same parameters as above, but also hashes the result returned from the first hash (it is essentially `sha256(sha256(date, len, res))`

test runs a series of tests and returns 0 if all of the tests are successful.

## Known issues
This library doesn't deal with data length that doesn't completely fill in exact bytes. For example it cannot deal with 10 bits of data.

## Sample code

### Running tests

The following code calls the test function of the library:

```
#include <cstring>
#include <cstdint>
#include "utility.h"

int main(int argc, char* argv[]) {
   return test();
}
```

The client code is compiled as follows:

`CC -m64 -std=c++11 -I../sha256lib -L../sha256lib -R../sha256lib -lsha256 test.cc`

And run like this:

`./a.out; echo $?`

### Basic hash

This sample code generates and prints out the hash of abc.

```
#include <cstring>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include "utility.h"

using namespace std;

void Dump(const char* msg, uint32_t* state, size_t num) {
   cout << msg << endl;
   cout << "{";
   for (size_t i = 0; i<num; ++i) {
      cout.setf(ios::showbase);
      cout << hex << setw(8) << setfill('0') << state[i];
      if (i!=7) {
	 cout << ",";
      }
   }
   cout << "}" << endl;
}

void DumpState(uint32_t* state) {
   Dump("STATE DUMP", state, 8);
}

const static uint32_t sha256_initial_hash_value[8] = {
   0x6a09e667UL, 0xbb67ae85UL, 0x3c6ef372UL, 0xa54ff53aUL,
   0x510e527fUL, 0x9b05688cUL, 0x1f83d9abUL, 0x5be0cd19UL
};

int main(int argc, char* argv[]) {
   uint32_t state[8]; 
   memcpy(state, sha256_initial_hash_value, sizeof(uint32_t) *8);
   static const char msg[] = "abc";
   static const size_t len = strlen(msg);

   Sha256(const_cast<char*>(msg), len, state);
   DumpState(state);

   return 0;
}
```

### Bitcoin mining

You can mine for bitcoins using the following code, provided as an example on how the code can be run in a multithreaded environment:

```
#include <iostream>
#include <sstream>
#include <cstring>
#include <iomanip>
#include <omp.h>
#include "utility.h"

using namespace std;

uint32_t ConvertNonce(uint32_t num) {
   uint32_t b0,b1,b2,b3;
   uint32_t res;
   b0 = (num & 0x000000ff) << 24u;
   b1 = (num & 0x0000ff00) << 8u;
   b2 = (num & 0x00ff0000) >> 8u;
   b3 = (num & 0xff000000) >> 24u;
   res = b0 | b1 | b2 | b3;
   return res;
}

void Dump(const char* msg, uint32_t* state, size_t num) {
   cout << msg << endl;
   cout << "{";
   for (size_t i = 0; i<num; ++i) {
      cout.setf(ios::showbase);
      cout << hex << setw(8) << setfill('0') << state[i];
      if (i!=7) { cout << ",";}
   }
   cout << "}" << endl;
}

void DumpState(uint32_t* state) { Dump("STATE DUMP", state, 8); }
void DumpHeader(uint32_t* state) { Dump("HEADER DUMP", state, 20); }

const static uint32_t sha256_initial_hash_value[8] = {
   0x6a09e667UL, 0xbb67ae85UL, 0x3c6ef372UL, 0xa54ff53aUL,
   0x510e527fUL, 0x9b05688cUL, 0x1f83d9abUL, 0x5be0cd19UL
};

int main(int argc, char* argv[]) {
   // block 356584
   // http://blockexplorer.com/block/0000000000000000013bfbbfc1c6094d6732cfc9c3c083a81fe039cf38ab3b9d
   static const uint32_t header[20] = 
      {0x01000000, 0x81cd02ab, 0x7e569e8b, 0xcd9317e2, 0xfe99f2de, 
       0x44d49ab2, 0xb8851ba4, 0xa3080000, 0x00000000, 0xe320b6c2, 
       0xfffc8d75, 0x0423db8b, 0x1eb942ae, 0x710e951e, 0xd797f7af, 
       0xfc8892b0, 0xf1fc122b, 0xc7f5d74d, 0xf2b9441a, 0x42a14695};
   uint32_t _nonce_f = 2504433986; // 2.5 billion ... 4,294 billion max
   uint32_t max = 10000000;
   if (argc == 2) {
      istringstream is(argv[1]);
      is >> max;
   }
   double percent = max / 4294967295.0;
   cout << fixed << setw(2) << percent * 100.0 << endl;
   uint32_t half = _nonce_f - (max / 2);

#pragma omp parallel for
   for (uint32_t i = 0; i<max; ++i) {
      uint32_t i_updated = i + half;
      uint32_t loc_header[20];
      for (unsigned j=0; j<19; ++j) {
	 loc_header[j] = header[j];
      }
      loc_header[19] = ConvertNonce(i_updated);

      uint32_t state[8]; 
      memcpy(state, sha256_initial_hash_value, sizeof(uint32_t) *8);
      Sha_d256(loc_header, 80, state);
      if (state[7] == 0) {
	 DumpState(state);
      } 
   }

   cout << "DONE" << endl;
   return 0;
}
```

Compiled using:

`CC -xopenmp=parallel -fast -m64 -std=c++11 -I../sha256lib -L../sha256lib -R../sha256lib -lsha256 bitcoin.cc`


