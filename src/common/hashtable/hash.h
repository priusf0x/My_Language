#ifndef HASH_H
#define HASH_H

#include <stdint.h>

#include "my_string.h"

/*1*/ uint32_t HashAlwaysZero(string_s string);
/*2*/ uint32_t HashFirstAscii(string_s string);
/*3*/ uint32_t HashWordLength(string_s string);
/*4*/ uint32_t HashElemSum(string_s string);
/*5*/ uint32_t HashRol(string_s string);
/*6*/ uint32_t HashRor(string_s string);
/*7*/ uint32_t HashELF(string_s string);
/*8*/ uint32_t HashCRC32(string_s string);

#endif // HASH_H
