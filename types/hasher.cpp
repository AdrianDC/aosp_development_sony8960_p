#include "hasher.h"
#include <iostream>
using std::cout;
using std::endl;

static uint64_t the_hash = 0xdeadbeef;

uint64_t getHash()
{
  return the_hash;
}

static int calls = 0;
void addHash(uint64_t n)
{
  the_hash *= 65;
  the_hash += n;
  //  cout << calls++ << ":U64 " << n << " h " << the_hash << endl;
}
void addHash(int64_t n)
{
  the_hash *= 65;
  the_hash += uint64_t(n);
  //  cout << calls++ << ":S64 " << n << " h " << the_hash << endl;
}
void addHash(uint32_t n)
{
  the_hash *= 65;
  the_hash += n;
  //  cout << calls++ << ":U32 " << n << " h " << the_hash << endl;
}
void addHash(int32_t n)
{
  the_hash *= 65;
  the_hash += uint32_t(n);
  //  cout << calls++ << ":S32 " << n << " h " << the_hash << endl;
}
void addHash(uint16_t n)
{
  the_hash *= 65;
  the_hash += n;
  //  cout << calls++ << ":U64 " << n << " h " << the_hash << endl;
}
void addHash(int16_t n)
{
  the_hash *= 65;
  the_hash += uint16_t(n);
  //  cout << calls++ << ":S64 " << n << " h " << the_hash << endl;
}
void addHash(uint8_t n)
{
  the_hash *= 65;
  the_hash += n;
  //  cout << calls++ << ":U32 " << n << " h " << the_hash << endl;
}
void addHash(int8_t n)
{
  the_hash *= 65;
  the_hash += uint8_t(n);
  //  cout << calls++ << ":S32 " << n << " h " << the_hash << endl;
}
