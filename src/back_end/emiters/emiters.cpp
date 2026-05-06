#include "emiters.h"

#include <assert.h>
#include <cstdint>
#include <stdint.h>
#include <stdlib.h>

const uint8_t MAGIC_NUM_REX = 0b0100'0000;
const uint8_t W_REX = 0b1000;
const uint8_t R_REX = 0b0100;
const uint8_t X_REX = 0b0010;
const uint8_t B_REX = 0b0001;

// ================================= EMITERS ==================================

// size_t
// EmitPush(uint8_t*        buffer,
//          argument_type_e type, 
//          uint64_t        oper)
// {
//     assert(buffer != nullptr);

//     const uint8_t op_code = 0x50;
//     size_t add_rip = 0;

//     if (type == REGISTER)
//     {
//         if (oper & REG_EXTENDED)
//         {
//             *buffer = MAGIC_NUM_REX | B_REX;
//             buffer++;
//             add_rip++;
//         }
        
//         *buffer = op_code | ((uint8_t) oper & (~REG_EXTENDED));
//         buffer++;
//         add_rip++;
//     }
//     else
//     {
//         // sorry not added yet
//         assert(0);
//     }
    
//     return add_rip;
// }


// size_t 
// EmitPop(uint8_t*        buffer,
//         argument_type_e type, 
//         uint64_t        oper)
// {
//     assert(buffer != nullptr);

//     const uint8_t op_code = 0x58;
//     size_t add_rip = 0;

//     if (type == REGISTER)
//     {
//         if (oper & REG_EXTENDED)
//         {
//             *buffer = MAGIC_NUM_REX | B_REX;
//             buffer++;
//             add_rip++;
//         }
        
//         *buffer = op_code | ((uint8_t) oper & (~REG_EXTENDED));
//         buffer++;
//         add_rip++;
//     }
//     else
//     {
//         // sorry not added yet
//         assert(0);
//     }
    
//     return add_rip;
// }

// size_t 
// EmitMov(uint8_t*        buffer,
//         argument_type_e type_l, 
//         uint64_t        oper_l,
//         argument_type_e type_r, 
//         uint64_t        oper_r,
//         int64_t         offset)
// {
//     assert(buffer != nullptr);

//     const uint8_t op_code_r_to_m = 0x89;
//     const uint8_t op_code_m_to_r = 0x8B;
//     size_t add_rip = 0;
    
//     *buffer = MAGIC_NUM_REX | B_REX;
//     buffer++;
//     add_rip++;

//     if (((type_l == REGISTER) && (oper_l & REG_EXTENDED))
//             || ((type_r == REGISTER) && (oper_r & REG_EXTENDED)))
//     {
//         //  sorry not added yet
//         assert(0);
//     }
    
//     if ((type_l == REGISTER) && (type_r == REGISTER))
//     {
//         *buffer = MAGIC_NUM_REX | B_REX;
//         *(buffer + 1) = op_code_r_to_m;
//         *(buffer + 2) = 0;
//         buffer += 3;
//         add_rip += 3;
        
//     }
//     else if (type_l == REGISTER)
//     {
        
//     }
//     else if (type_r == REGISTER)   
//     {
        
//     }
//     else  
//     {
//         // invalid instruction 
//         assert(0); 
//     }
    
//     return add_rip;
// }
