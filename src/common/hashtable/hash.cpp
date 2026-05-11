#include "hash.h"

#include <assert.h>
#include <cstdint>
#include <stdint.h>

#include "my_string.h"

// ================================ HASH_ALGOS ================================

uint32_t 
HashCRC32(string_s string)
{
    uint32_t crc = ~0u;
    const uint32_t filter = 0xEDB88320;

    for (size_t i = 0; i < string.size ; i++) 
    {
        crc ^= (uint32_t) string.string[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1) 
            {
                crc = (crc >> 1) ^ filter;
            }
            else 
            {
                crc >>= 1;
            }
        }
    }

    return ~crc;
}



