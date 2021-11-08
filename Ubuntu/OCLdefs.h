#pragma once

#ifndef OCLDEFS_H
#define OCLDEFS_H




#ifdef WIN32
typedef long long          int64_t;
#else
#ifndef __OPENCL_VERSION__
#include <CL/cl.h>
#endif
#endif
typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
//typedef unsigned int       cl_uint;
//typedef unsigned long long cl_ulong;

#if (defined (_WIN32) && defined(_MSC_VER))
typedef unsigned __int64        cl_ulong;
//typedef unsigned __int32        cl_uint;
typedef unsigned __int32        cl_uint;
#endif




#define MAX_MINER 100
#define MAX_POOL_RES 16

////////////////////////////////////////////////////////////////////////////////
//  PARAMETERS: Autolykos algorithm
////////////////////////////////////////////////////////////////////////////////
// constant message size
#define CONST_MES_SIZE_8   8192 // 2^10

// prehash continue position
#define CONTINUE_POS       36

// k: number of indices
#define K_LEN              32

// N: number of precalculated hashes
#define INIT_N_LEN 0x4000000
#define MAX_N_LEN  0x7FC9FF98
#define IncreaseStart (600*1024)
#define IncreaseEnd (4198400)
#define IncreasePeriodForN (50*1024)



#define Sol_Index 0x3381BF + 10
////////////////////////////////////////////////////////////////////////////////
//  PARAMETERS: Heuristic prehash kernel parameters
////////////////////////////////////////////////////////////////////////////////
// number of nonces per thread
#define NONCES_PER_THREAD  1   //allllllllwways

// total number of nonces per iteration
// #define NONCES_PER_ITER    0x200000 // 2^22
#define NONCES_PER_ITER    (0x400000) //
//
// kernel block size
// #define BLOCK_DIM          64
#define BLOCK_DIM        64// 
////////////////////////////////////////////////////////////////////////////////
// Memory compatibility checks
// should probably be now more correctly set
#define MIN_FREE_MEMORY    2200000000
#define MIN_FREE_MEMORY_PREHASH 7300000000


////////////////////////////////////////////////////////////////////////////////
//  CONSTANTS: Autolykos algorithm
////////////////////////////////////////////////////////////////////////////////
// secret key and hash size
#define NUM_SIZE_8         32

// public key size
#define PK_SIZE_8          33

// nonce size
#define NONCE_SIZE_8       8

// height size
#define HEIGHT_SIZE       4

//#define EXTRA_BASE_SIZE       4

// index size
#define INDEX_SIZE_8       4

// BLAKE2b-256 hash buffer size
#define BUF_SIZE_8         128

////////////////////////////////////////////////////////////////////////////////
//  CONSTANTS: Q definition 32-bits and 64-bits words
////////////////////////////////////////////////////////////////////////////////
// Q definition for CUDA ptx pseudo-assembler commands
// 32 bits
//#define qhi_s              "0xFFFFFFFF"
//#define q4_s               "0xFFFFFFFE"
//#define q3_s               "0xBAAEDCE6"
//#define q2_s               "0xAF48A03B"
//#define q1_s               "0xBFD25E8C"
//#define q0_s               "0xD0364141"

#define qhi_s              0xFFFFFFFF
#define q4_s               0xFFFFFFFE
#define q3_s               0xBAAEDCE6
#define q2_s               0xAF48A03B
#define q1_s               0xBFD25E8C
#define q0_s               0xD0364141

// Valid range: Q itself is multiplier-of-Q floor of 2^256
// 64 bits
#define Q3                 0xFFFFFFFFFFFFFFFF
#define Q2                 0xFFFFFFFFFFFFFFFE
#define Q1                 0xBAAEDCE6AF48A03B
#define Q0                 0xBFD25E8CD0364141

////////////////////////////////////////////////////////////////////////////////
//  CONSTANTS: CURL http & JSMN specifiers
////////////////////////////////////////////////////////////////////////////////
// CURL number of retries to POST solution if failed
#define MAX_POST_RETRIES   5

// URL max size
#define MAX_URL_SIZE       1024

//============================================================================//
//  CURL requests
//============================================================================//
// default request capacity
#define JSON_CAPACITY      (256 *3)

// maximal request capacity
#define MAX_JSON_CAPACITY  8192

// total JSON objects count
#define REQ_LEN         13 //  9 (alg version) + H
// JSON position of message
#define MES_POS            2

// JSON position of bound
#define BOUND_POS          4

// JSON position of public key
#define PK_POS             6


//============================================================================//
//  Configuration file
//============================================================================//
// max JSON objects count for config file,
// increased, to have more options if we need them
#define CONF_LEN           21

// config JSON position of secret key
#define SEED_POS           2

// config JSON position of latest block adress
#define NODE_POS           4

// config JSON position of keep prehash option
#define KEEP_POS           6

////////////////////////////////////////////////////////////////////////////////
//  Error messages
////////////////////////////////////////////////////////////////////////////////
#define ERROR_STAT         "stat"
#define ERROR_ALLOC        "Host memory allocation"
#define ERROR_IO           "I/O"
#define ERROR_CURL         "Curl"
#define ERROR_OPENSSL      "OpenSSL"

////////////////////////////////////////////////////////////////////////////////
//  Derived parameters
////////////////////////////////////////////////////////////////////////////////
// secret key and hash size
#define NUM_SIZE_4         (NUM_SIZE_8 << 1)
#define NUM_SIZE_32        (NUM_SIZE_8 >> 2)
#define NUM_SIZE_64        (NUM_SIZE_8 >> 3)
#define NUM_SIZE_32_BLOCK  (1 + (NUM_SIZE_32 - 1) / BLOCK_DIM)
#define NUM_SIZE_8_BLOCK   (NUM_SIZE_32_BLOCK << 2)
#define ROUND_NUM_SIZE_32  (NUM_SIZE_32_BLOCK * BLOCK_DIM)

// public key sizes
#define PK_SIZE_4          (PK_SIZE_8 << 1)
#define PK_SIZE_32_BLOCK   (1 + NUM_SIZE_32 / BLOCK_DIM)
#define PK_SIZE_8_BLOCK    (PK_SIZE_32_BLOCK << 2)
#define ROUND_PK_SIZE_32   (PK_SIZE_32_BLOCK * BLOCK_DIM)
#define COUPLED_PK_SIZE_32 (((PK_SIZE_8 << 1) + 3) >> 2)

// nonce sizes
#define NONCE_SIZE_4       (NONCE_SIZE_8 << 1)
#define NONCE_SIZE_32      (NONCE_SIZE_8 >> 2)

//============================================================================//
//  Puzzle state
//============================================================================//
struct ctx_t;

// puzzle data size
#define DATA_SIZE_8                                                            \
(                                                                              \
    (1 + (2 * PK_SIZE_8 + 2 + 3 * NUM_SIZE_8 + sizeof(ctx_t) - 1) / BLOCK_DIM) \
    * BLOCK_DIM                                                                \
)

//============================================================================//
//  GPU shared memory
//============================================================================//
// (mes || w) sizes
#define NP_SIZE_32_BLOCK   (1 + (NUM_SIZE_32 << 1) / BLOCK_DIM)
#define NP_SIZE_8_BLOCK    (NP_SIZE_32_BLOCK << 2)
#define ROUND_NP_SIZE_32   (NP_SIZE_32_BLOCK * BLOCK_DIM)

// (pk || mes || w) sizes
#define PNP_SIZE_32_BLOCK                                                      \
(1 + (COUPLED_PK_SIZE_32 + NUM_SIZE_32 - 1) / BLOCK_DIM)

#define PNP_SIZE_8_BLOCK   (PNP_SIZE_32_BLOCK << 2)
#define ROUND_PNP_SIZE_32  (PNP_SIZE_32_BLOCK * BLOCK_DIM)

// (x || ctx) sizes
#define NC_SIZE_32_BLOCK                                                       \
(1 + (NUM_SIZE_32 + sizeof(ctx_t) - 1) / BLOCK_DIM)

#define CTX_SIZE sizeof(ctx_t)

#define NC_SIZE_8_BLOCK    (NC_SIZE_32_BLOCK << 2)
#define ROUND_NC_SIZE_32   (NC_SIZE_32_BLOCK * BLOCK_DIM)

//============================================================================//
//============================================================================//
// number of threads per iteration
#define THREADS_PER_ITER   (NONCES_PER_ITER / NONCES_PER_THREAD)

////////////////////////////////////////////////////////////////////////////////
//  Structs
////////////////////////////////////////////////////////////////////////////////
typedef unsigned int uint_t;

// autolukos puzzle state
typedef enum
{
	STATE_CONTINUE = 0,
	STATE_KEYGEN = 1,
	STATE_REHASH = 2,
	STATE_INTERRUPT = 3
}
state_t;
// BLAKE2b-256 hash state context
typedef struct ctx_t
{
	// input buffer
	uint8_t b[BUF_SIZE_8];
	// chained state
	cl_ulong h[8];
	// total number of bytes
	cl_ulong t[2];
	// counter for b
	cl_uint c;
}ctx_t;

// BLAKE2b-256 packed uncomplete hash state context
typedef struct uctx_t
{
	// chained state
	cl_ulong h[8];
	// total number of bytes
	cl_ulong t[2];
}uctx_t;

////////////////////////////////////////////////////////////////////////////////
//  BLAKE2b-256 hashing procedures macros
////////////////////////////////////////////////////////////////////////////////
// initialization vector
#define B2B_IV(v)                                                              \
do                                                                             \
{                                                                              \
    ((cl_ulong *)(v))[0] = 0x6A09E667F3BCC908;                                 \
    ((cl_ulong *)(v))[1] = 0xBB67AE8584CAA73B;                                 \
    ((cl_ulong *)(v))[2] = 0x3C6EF372FE94F82B;                                 \
    ((cl_ulong *)(v))[3] = 0xA54FF53A5F1D36F1;                                 \
    ((cl_ulong *)(v))[4] = 0x510E527FADE682D1;                                 \
    ((cl_ulong *)(v))[5] = 0x9B05688C2B3E6C1F;                                 \
    ((cl_ulong *)(v))[6] = 0x1F83D9ABFB41BD6B;                                 \
    ((cl_ulong *)(v))[7] = 0x5BE0CD19137E2179;                                 \
}                                                                              \
while (0)

// cyclic right rotation
#define ROTR64(x, y) (((x) >> (y)) ^ ((x) << (64 - (y))))

// G mixing function
#define B2B_G(v, a, b, c, d, x, y)                                             \
do                                                                             \
{                                                                              \
    ((cl_ulong *)(v))[a] += ((cl_ulong *)(v))[b] + x;                          \
    ((cl_ulong *)(v))[d]                                                       \
        = ROTR64(((cl_ulong *)(v))[d] ^ ((cl_ulong *)(v))[a], 32);             \
    ((cl_ulong *)(v))[c] += ((cl_ulong *)(v))[d];                              \
    ((cl_ulong *)(v))[b]                                                       \
        = ROTR64(((cl_ulong *)(v))[b] ^ ((cl_ulong *)(v))[c], 24);             \
    ((cl_ulong *)(v))[a] += ((cl_ulong *)(v))[b] + y;                          \
    ((cl_ulong *)(v))[d]                                                       \
        = ROTR64(((cl_ulong *)(v))[d] ^ ((cl_ulong *)(v))[a], 16);             \
    ((cl_ulong *)(v))[c] += ((cl_ulong *)(v))[d];                              \
    ((cl_ulong *)(v))[b]                                                       \
        = ROTR64(((cl_ulong *)(v))[b] ^ ((cl_ulong *)(v))[c], 63);             \
}                                                                              \
while (0)

// mixing rounds
#define B2B_MIX(v, m)                                                          \
do                                                                             \
{                                                                              \
    B2B_G(v, 0, 4,  8, 12, ((cl_ulong *)(m))[ 0], ((cl_ulong *)(m))[ 1]);      \
    B2B_G(v, 1, 5,  9, 13, ((cl_ulong *)(m))[ 2], ((cl_ulong *)(m))[ 3]);      \
    B2B_G(v, 2, 6, 10, 14, ((cl_ulong *)(m))[ 4], ((cl_ulong *)(m))[ 5]);      \
    B2B_G(v, 3, 7, 11, 15, ((cl_ulong *)(m))[ 6], ((cl_ulong *)(m))[ 7]);      \
    B2B_G(v, 0, 5, 10, 15, ((cl_ulong *)(m))[ 8], ((cl_ulong *)(m))[ 9]);      \
    B2B_G(v, 1, 6, 11, 12, ((cl_ulong *)(m))[10], ((cl_ulong *)(m))[11]);      \
    B2B_G(v, 2, 7,  8, 13, ((cl_ulong *)(m))[12], ((cl_ulong *)(m))[13]);      \
    B2B_G(v, 3, 4,  9, 14, ((cl_ulong *)(m))[14], ((cl_ulong *)(m))[15]);      \
                                                                               \
    B2B_G(v, 0, 4,  8, 12, ((cl_ulong *)(m))[14], ((cl_ulong *)(m))[10]);      \
    B2B_G(v, 1, 5,  9, 13, ((cl_ulong *)(m))[ 4], ((cl_ulong *)(m))[ 8]);      \
    B2B_G(v, 2, 6, 10, 14, ((cl_ulong *)(m))[ 9], ((cl_ulong *)(m))[15]);      \
    B2B_G(v, 3, 7, 11, 15, ((cl_ulong *)(m))[13], ((cl_ulong *)(m))[ 6]);      \
    B2B_G(v, 0, 5, 10, 15, ((cl_ulong *)(m))[ 1], ((cl_ulong *)(m))[12]);      \
    B2B_G(v, 1, 6, 11, 12, ((cl_ulong *)(m))[ 0], ((cl_ulong *)(m))[ 2]);      \
    B2B_G(v, 2, 7,  8, 13, ((cl_ulong *)(m))[11], ((cl_ulong *)(m))[ 7]);      \
    B2B_G(v, 3, 4,  9, 14, ((cl_ulong *)(m))[ 5], ((cl_ulong *)(m))[ 3]);      \
                                                                               \
    B2B_G(v, 0, 4,  8, 12, ((cl_ulong *)(m))[11], ((cl_ulong *)(m))[ 8]);      \
    B2B_G(v, 1, 5,  9, 13, ((cl_ulong *)(m))[12], ((cl_ulong *)(m))[ 0]);      \
    B2B_G(v, 2, 6, 10, 14, ((cl_ulong *)(m))[ 5], ((cl_ulong *)(m))[ 2]);      \
    B2B_G(v, 3, 7, 11, 15, ((cl_ulong *)(m))[15], ((cl_ulong *)(m))[13]);      \
    B2B_G(v, 0, 5, 10, 15, ((cl_ulong *)(m))[10], ((cl_ulong *)(m))[14]);      \
    B2B_G(v, 1, 6, 11, 12, ((cl_ulong *)(m))[ 3], ((cl_ulong *)(m))[ 6]);      \
    B2B_G(v, 2, 7,  8, 13, ((cl_ulong *)(m))[ 7], ((cl_ulong *)(m))[ 1]);      \
    B2B_G(v, 3, 4,  9, 14, ((cl_ulong *)(m))[ 9], ((cl_ulong *)(m))[ 4]);      \
                                                                               \
    B2B_G(v, 0, 4,  8, 12, ((cl_ulong *)(m))[ 7], ((cl_ulong *)(m))[ 9]);      \
    B2B_G(v, 1, 5,  9, 13, ((cl_ulong *)(m))[ 3], ((cl_ulong *)(m))[ 1]);      \
    B2B_G(v, 2, 6, 10, 14, ((cl_ulong *)(m))[13], ((cl_ulong *)(m))[12]);      \
    B2B_G(v, 3, 7, 11, 15, ((cl_ulong *)(m))[11], ((cl_ulong *)(m))[14]);      \
    B2B_G(v, 0, 5, 10, 15, ((cl_ulong *)(m))[ 2], ((cl_ulong *)(m))[ 6]);      \
    B2B_G(v, 1, 6, 11, 12, ((cl_ulong *)(m))[ 5], ((cl_ulong *)(m))[10]);      \
    B2B_G(v, 2, 7,  8, 13, ((cl_ulong *)(m))[ 4], ((cl_ulong *)(m))[ 0]);      \
    B2B_G(v, 3, 4,  9, 14, ((cl_ulong *)(m))[15], ((cl_ulong *)(m))[ 8]);      \
                                                                               \
    B2B_G(v, 0, 4,  8, 12, ((cl_ulong *)(m))[ 9], ((cl_ulong *)(m))[ 0]);      \
    B2B_G(v, 1, 5,  9, 13, ((cl_ulong *)(m))[ 5], ((cl_ulong *)(m))[ 7]);      \
    B2B_G(v, 2, 6, 10, 14, ((cl_ulong *)(m))[ 2], ((cl_ulong *)(m))[ 4]);      \
    B2B_G(v, 3, 7, 11, 15, ((cl_ulong *)(m))[10], ((cl_ulong *)(m))[15]);      \
    B2B_G(v, 0, 5, 10, 15, ((cl_ulong *)(m))[14], ((cl_ulong *)(m))[ 1]);      \
    B2B_G(v, 1, 6, 11, 12, ((cl_ulong *)(m))[11], ((cl_ulong *)(m))[12]);      \
    B2B_G(v, 2, 7,  8, 13, ((cl_ulong *)(m))[ 6], ((cl_ulong *)(m))[ 8]);      \
    B2B_G(v, 3, 4,  9, 14, ((cl_ulong *)(m))[ 3], ((cl_ulong *)(m))[13]);      \
                                                                               \
    B2B_G(v, 0, 4,  8, 12, ((cl_ulong *)(m))[ 2], ((cl_ulong *)(m))[12]);      \
    B2B_G(v, 1, 5,  9, 13, ((cl_ulong *)(m))[ 6], ((cl_ulong *)(m))[10]);      \
    B2B_G(v, 2, 6, 10, 14, ((cl_ulong *)(m))[ 0], ((cl_ulong *)(m))[11]);      \
    B2B_G(v, 3, 7, 11, 15, ((cl_ulong *)(m))[ 8], ((cl_ulong *)(m))[ 3]);      \
    B2B_G(v, 0, 5, 10, 15, ((cl_ulong *)(m))[ 4], ((cl_ulong *)(m))[13]);      \
    B2B_G(v, 1, 6, 11, 12, ((cl_ulong *)(m))[ 7], ((cl_ulong *)(m))[ 5]);      \
    B2B_G(v, 2, 7,  8, 13, ((cl_ulong *)(m))[15], ((cl_ulong *)(m))[14]);      \
    B2B_G(v, 3, 4,  9, 14, ((cl_ulong *)(m))[ 1], ((cl_ulong *)(m))[ 9]);      \
                                                                               \
    B2B_G(v, 0, 4,  8, 12, ((cl_ulong *)(m))[12], ((cl_ulong *)(m))[ 5]);      \
    B2B_G(v, 1, 5,  9, 13, ((cl_ulong *)(m))[ 1], ((cl_ulong *)(m))[15]);      \
    B2B_G(v, 2, 6, 10, 14, ((cl_ulong *)(m))[14], ((cl_ulong *)(m))[13]);      \
    B2B_G(v, 3, 7, 11, 15, ((cl_ulong *)(m))[ 4], ((cl_ulong *)(m))[10]);      \
    B2B_G(v, 0, 5, 10, 15, ((cl_ulong *)(m))[ 0], ((cl_ulong *)(m))[ 7]);      \
    B2B_G(v, 1, 6, 11, 12, ((cl_ulong *)(m))[ 6], ((cl_ulong *)(m))[ 3]);      \
    B2B_G(v, 2, 7,  8, 13, ((cl_ulong *)(m))[ 9], ((cl_ulong *)(m))[ 2]);      \
    B2B_G(v, 3, 4,  9, 14, ((cl_ulong *)(m))[ 8], ((cl_ulong *)(m))[11]);      \
                                                                               \
    B2B_G(v, 0, 4,  8, 12, ((cl_ulong *)(m))[13], ((cl_ulong *)(m))[11]);      \
    B2B_G(v, 1, 5,  9, 13, ((cl_ulong *)(m))[ 7], ((cl_ulong *)(m))[14]);      \
    B2B_G(v, 2, 6, 10, 14, ((cl_ulong *)(m))[12], ((cl_ulong *)(m))[ 1]);      \
    B2B_G(v, 3, 7, 11, 15, ((cl_ulong *)(m))[ 3], ((cl_ulong *)(m))[ 9]);      \
    B2B_G(v, 0, 5, 10, 15, ((cl_ulong *)(m))[ 5], ((cl_ulong *)(m))[ 0]);      \
    B2B_G(v, 1, 6, 11, 12, ((cl_ulong *)(m))[15], ((cl_ulong *)(m))[ 4]);      \
    B2B_G(v, 2, 7,  8, 13, ((cl_ulong *)(m))[ 8], ((cl_ulong *)(m))[ 6]);      \
    B2B_G(v, 3, 4,  9, 14, ((cl_ulong *)(m))[ 2], ((cl_ulong *)(m))[10]);      \
                                                                               \
    B2B_G(v, 0, 4,  8, 12, ((cl_ulong *)(m))[ 6], ((cl_ulong *)(m))[15]);      \
    B2B_G(v, 1, 5,  9, 13, ((cl_ulong *)(m))[14], ((cl_ulong *)(m))[ 9]);      \
    B2B_G(v, 2, 6, 10, 14, ((cl_ulong *)(m))[11], ((cl_ulong *)(m))[ 3]);      \
    B2B_G(v, 3, 7, 11, 15, ((cl_ulong *)(m))[ 0], ((cl_ulong *)(m))[ 8]);      \
    B2B_G(v, 0, 5, 10, 15, ((cl_ulong *)(m))[12], ((cl_ulong *)(m))[ 2]);      \
    B2B_G(v, 1, 6, 11, 12, ((cl_ulong *)(m))[13], ((cl_ulong *)(m))[ 7]);      \
    B2B_G(v, 2, 7,  8, 13, ((cl_ulong *)(m))[ 1], ((cl_ulong *)(m))[ 4]);      \
    B2B_G(v, 3, 4,  9, 14, ((cl_ulong *)(m))[10], ((cl_ulong *)(m))[ 5]);      \
                                                                               \
    B2B_G(v, 0, 4,  8, 12, ((cl_ulong *)(m))[10], ((cl_ulong *)(m))[ 2]);      \
    B2B_G(v, 1, 5,  9, 13, ((cl_ulong *)(m))[ 8], ((cl_ulong *)(m))[ 4]);      \
    B2B_G(v, 2, 6, 10, 14, ((cl_ulong *)(m))[ 7], ((cl_ulong *)(m))[ 6]);      \
    B2B_G(v, 3, 7, 11, 15, ((cl_ulong *)(m))[ 1], ((cl_ulong *)(m))[ 5]);      \
    B2B_G(v, 0, 5, 10, 15, ((cl_ulong *)(m))[15], ((cl_ulong *)(m))[11]);      \
    B2B_G(v, 1, 6, 11, 12, ((cl_ulong *)(m))[ 9], ((cl_ulong *)(m))[14]);      \
    B2B_G(v, 2, 7,  8, 13, ((cl_ulong *)(m))[ 3], ((cl_ulong *)(m))[12]);      \
    B2B_G(v, 3, 4,  9, 14, ((cl_ulong *)(m))[13], ((cl_ulong *)(m))[ 0]);      \
                                                                               \
    B2B_G(v, 0, 4,  8, 12, ((cl_ulong *)(m))[ 0], ((cl_ulong *)(m))[ 1]);      \
    B2B_G(v, 1, 5,  9, 13, ((cl_ulong *)(m))[ 2], ((cl_ulong *)(m))[ 3]);      \
    B2B_G(v, 2, 6, 10, 14, ((cl_ulong *)(m))[ 4], ((cl_ulong *)(m))[ 5]);      \
    B2B_G(v, 3, 7, 11, 15, ((cl_ulong *)(m))[ 6], ((cl_ulong *)(m))[ 7]);      \
    B2B_G(v, 0, 5, 10, 15, ((cl_ulong *)(m))[ 8], ((cl_ulong *)(m))[ 9]);      \
    B2B_G(v, 1, 6, 11, 12, ((cl_ulong *)(m))[10], ((cl_ulong *)(m))[11]);      \
    B2B_G(v, 2, 7,  8, 13, ((cl_ulong *)(m))[12], ((cl_ulong *)(m))[13]);      \
    B2B_G(v, 3, 4,  9, 14, ((cl_ulong *)(m))[14], ((cl_ulong *)(m))[15]);      \
                                                                               \
    B2B_G(v, 0, 4,  8, 12, ((cl_ulong *)(m))[14], ((cl_ulong *)(m))[10]);      \
    B2B_G(v, 1, 5,  9, 13, ((cl_ulong *)(m))[ 4], ((cl_ulong *)(m))[ 8]);      \
    B2B_G(v, 2, 6, 10, 14, ((cl_ulong *)(m))[ 9], ((cl_ulong *)(m))[15]);      \
    B2B_G(v, 3, 7, 11, 15, ((cl_ulong *)(m))[13], ((cl_ulong *)(m))[ 6]);      \
    B2B_G(v, 0, 5, 10, 15, ((cl_ulong *)(m))[ 1], ((cl_ulong *)(m))[12]);      \
    B2B_G(v, 1, 6, 11, 12, ((cl_ulong *)(m))[ 0], ((cl_ulong *)(m))[ 2]);      \
    B2B_G(v, 2, 7,  8, 13, ((cl_ulong *)(m))[11], ((cl_ulong *)(m))[ 7]);      \
    B2B_G(v, 3, 4,  9, 14, ((cl_ulong *)(m))[ 5], ((cl_ulong *)(m))[ 3]);      \
}                                                                              \
while (0)

// blake2b initialization
#define B2B_INIT(ctx, aux)                                                     \
do                                                                             \
{                                                                              \
    ((cl_ulong *)(aux))[0] = ((ctx_t *)(ctx))->h[0];                           \
    ((cl_ulong *)(aux))[1] = ((ctx_t *)(ctx))->h[1];                           \
    ((cl_ulong *)(aux))[2] = ((ctx_t *)(ctx))->h[2];                           \
    ((cl_ulong *)(aux))[3] = ((ctx_t *)(ctx))->h[3];                           \
    ((cl_ulong *)(aux))[4] = ((ctx_t *)(ctx))->h[4];                           \
    ((cl_ulong *)(aux))[5] = ((ctx_t *)(ctx))->h[5];                           \
    ((cl_ulong *)(aux))[6] = ((ctx_t *)(ctx))->h[6];                           \
    ((cl_ulong *)(aux))[7] = ((ctx_t *)(ctx))->h[7];                           \
                                                                               \
    B2B_IV(aux + 8);                                                           \
                                                                               \
    ((cl_ulong *)(aux))[12] ^= ((ctx_t *)(ctx))->t[0];                         \
    ((cl_ulong *)(aux))[13] ^= ((ctx_t *)(ctx))->t[1];                         \
}                                                                              \
while (0)

#define CAST(x) (((union { __typeof__(x) a; cl_ulong b; })x).b)

/*
/// // blake2b mixing
/// #define B2B_FINAL(ctx, aux)                                                    \
/// do                                                                             \
/// {                                                                              \
///     ((cl_ulong *)(aux))[16] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[ 0];         \
///     ((cl_ulong *)(aux))[17] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[ 1];         \
///     ((cl_ulong *)(aux))[18] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[ 2];         \
///     ((cl_ulong *)(aux))[19] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[ 3];         \
///     ((cl_ulong *)(aux))[20] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[ 4];         \
///     ((cl_ulong *)(aux))[21] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[ 5];         \
///     ((cl_ulong *)(aux))[22] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[ 6];         \
///     ((cl_ulong *)(aux))[23] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[ 7];         \
///     ((cl_ulong *)(aux))[24] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[ 8];         \
///     ((cl_ulong *)(aux))[25] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[ 9];         \
///     ((cl_ulong *)(aux))[26] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[10];         \
///     ((cl_ulong *)(aux))[27] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[11];         \
///     ((cl_ulong *)(aux))[28] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[12];         \
///     ((cl_ulong *)(aux))[29] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[13];         \
///     ((cl_ulong *)(aux))[30] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[14];         \
///     ((cl_ulong *)(aux))[31] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[15];         \
///                                                                                \
///     B2B_MIX(aux, aux + 16);                                                    \
///                                                                                \
///     ((ctx_t *)(ctx))->h[0] ^= ((cl_ulong *)(aux))[0] ^ ((cl_ulong *)(aux))[ 8];\
///     ((ctx_t *)(ctx))->h[1] ^= ((cl_ulong *)(aux))[1] ^ ((cl_ulong *)(aux))[ 9];\
///     ((ctx_t *)(ctx))->h[2] ^= ((cl_ulong *)(aux))[2] ^ ((cl_ulong *)(aux))[10];\
///     ((ctx_t *)(ctx))->h[3] ^= ((cl_ulong *)(aux))[3] ^ ((cl_ulong *)(aux))[11];\
///     ((ctx_t *)(ctx))->h[4] ^= ((cl_ulong *)(aux))[4] ^ ((cl_ulong *)(aux))[12];\
///     ((ctx_t *)(ctx))->h[5] ^= ((cl_ulong *)(aux))[5] ^ ((cl_ulong *)(aux))[13];\
///     ((ctx_t *)(ctx))->h[6] ^= ((cl_ulong *)(aux))[6] ^ ((cl_ulong *)(aux))[14];\
///     ((ctx_t *)(ctx))->h[7] ^= ((cl_ulong *)(aux))[7] ^ ((cl_ulong *)(aux))[15];\
/// }                                                                              \
/// while (0)
*/

// blake2b mixing
#define B2B_FINAL(ctx, aux)                                                    \
do                                                                             \
{                                                                              \
    ((cl_ulong *)(aux))[16] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[ 0];         \
    ((cl_ulong *)(aux))[17] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[ 1];         \
    ((cl_ulong *)(aux))[18] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[ 2];         \
    ((cl_ulong *)(aux))[19] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[ 3];         \
    ((cl_ulong *)(aux))[20] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[ 4];         \
    ((cl_ulong *)(aux))[21] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[ 5];         \
    ((cl_ulong *)(aux))[22] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[ 6];         \
    ((cl_ulong *)(aux))[23] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[ 7];         \
    ((cl_ulong *)(aux))[24] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[ 8];         \
    ((cl_ulong *)(aux))[25] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[ 9];         \
    ((cl_ulong *)(aux))[26] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[10];         \
    ((cl_ulong *)(aux))[27] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[11];         \
    ((cl_ulong *)(aux))[28] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[12];         \
    ((cl_ulong *)(aux))[29] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[13];         \
    ((cl_ulong *)(aux))[30] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[14];         \
    ((cl_ulong *)(aux))[31] = ((cl_ulong *)(((ctx_t *)(ctx))->b))[15];         \
                                                                               \
    B2B_MIX(aux, aux + 16);                                                    \
                                                                               \
    ((ctx_t *)(ctx))->h[0] ^= ((cl_ulong *)(aux))[0] ^ ((cl_ulong *)(aux))[ 8];\
    ((ctx_t *)(ctx))->h[1] ^= ((cl_ulong *)(aux))[1] ^ ((cl_ulong *)(aux))[ 9];\
    ((ctx_t *)(ctx))->h[2] ^= ((cl_ulong *)(aux))[2] ^ ((cl_ulong *)(aux))[10];\
    ((ctx_t *)(ctx))->h[3] ^= ((cl_ulong *)(aux))[3] ^ ((cl_ulong *)(aux))[11];\
    ((ctx_t *)(ctx))->h[4] ^= ((cl_ulong *)(aux))[4] ^ ((cl_ulong *)(aux))[12];\
    ((ctx_t *)(ctx))->h[5] ^= ((cl_ulong *)(aux))[5] ^ ((cl_ulong *)(aux))[13];\
    ((ctx_t *)(ctx))->h[6] ^= ((cl_ulong *)(aux))[6] ^ ((cl_ulong *)(aux))[14];\
    ((ctx_t *)(ctx))->h[7] ^= ((cl_ulong *)(aux))[7] ^ ((cl_ulong *)(aux))[15];\
}                                                                              \
while (0)

// blake2b intermediate mixing procedure on host
#define HOST_B2B_H(ctx, aux)                                                   \
do                                                                             \
{                                                                              \
    ((ctx_t *)(ctx))->t[0] += BUF_SIZE_8;                                      \
    ((ctx_t *)(ctx))->t[1] += 1 - !(((ctx_t *)(ctx))->t[0] < BUF_SIZE_8);      \
                                                                               \
    B2B_INIT(ctx, aux);                                                        \
    B2B_FINAL(ctx, aux);                                                       \
                                                                               \
    ((ctx_t *)(ctx))->c = 0;                                                   \
}                                                                              \
while (0)

// blake2b intermediate mixing procedure on host
#define HOST_B2B_H_LAST(ctx, aux)                                              \
do                                                                             \
{                                                                              \
    ((ctx_t *)(ctx))->t[0] += ((ctx_t *)(ctx))->c;                             \
    ((ctx_t *)(ctx))->t[1]                                                     \
        += 1 - !(((ctx_t *)(ctx))->t[0] < ((ctx_t *)(ctx))->c);                \
                                                                               \
    while (((ctx_t *)(ctx))->c < BUF_SIZE_8)                                   \
    {                                                                          \
        ((ctx_t *)(ctx))->b[((ctx_t *)(ctx))->c++] = 0;                        \
    }                                                                          \
                                                                               \
    B2B_INIT(ctx, aux);                                                        \
                                                                               \
    ((cl_ulong *)(aux))[14] = ~((cl_ulong *)(aux))[14];                        \
                                                                               \
    B2B_FINAL(ctx, aux);                                                       \
}                                                                              \
while (0)

////////////////////////////////////////////////////////////////////////////////
//  Little-Endian to Big-Endian convertation
////////////////////////////////////////////////////////////////////////////////
#define REVERSE_ENDIAN(p)                                                      \
    ((((cl_ulong)((uint8_t *)(p))[0]) << 56) ^                                 \
    (((cl_ulong)((uint8_t *)(p))[1]) << 48) ^                                  \
    (((cl_ulong)((uint8_t *)(p))[2]) << 40) ^                                  \
    (((cl_ulong)((uint8_t *)(p))[3]) << 32) ^                                  \
    (((cl_ulong)((uint8_t *)(p))[4]) << 24) ^                                  \
    (((cl_ulong)((uint8_t *)(p))[5]) << 16) ^                                  \
    (((cl_ulong)((uint8_t *)(p))[6]) << 8) ^                                   \
    ((cl_ulong)((uint8_t *)(p))[7]))

#define INPLACE_REVERSE_ENDIAN(p)                                              \
do                                                                             \
{                                                                              \
    *((cl_ulong *)(p))                                                         \
    = ((((cl_ulong)((uint8_t *)(p))[0]) << 56) ^                               \
    (((cl_ulong)((uint8_t *)(p))[1]) << 48) ^                                  \
    (((cl_ulong)((uint8_t *)(p))[2]) << 40) ^                                  \
    (((cl_ulong)((uint8_t *)(p))[3]) << 32) ^                                  \
    (((cl_ulong)((uint8_t *)(p))[4]) << 24) ^                                  \
    (((cl_ulong)((uint8_t *)(p))[5]) << 16) ^                                  \
    (((cl_ulong)((uint8_t *)(p))[6]) << 8) ^                                   \
    ((cl_ulong)((uint8_t *)(p))[7]));                                          \
}                                                                              \
while (0)





////////////////////////////////////////////////////////////////////////////////
//  Wrappers for function calls
////////////////////////////////////////////////////////////////////////////////
#define FREE(x)                                                                \
do                                                                             \
{                                                                              \
    if (x)                                                                     \
    {                                                                          \
        free(x);                                                               \
        (x) = NULL;                                                            \
    }                                                                          \
}                                                                              \
while (0)

#define CALL(func, name)                                                       \
do                                                                             \
{                                                                              \
    if (!(func))                                                               \
    {                                                                          \
        fprintf(stderr, "ERROR:  " name " failed at %s: %d\n",__FILE__,__LINE__);\
        exit(EXIT_FAILURE);                                                    \
    }                                                                          \
}                                                                              \
while (0)

#define FUNCTION_CALL(res, func, name)                                         \
do                                                                             \
{                                                                              \
    if (!((res) = (func)))                                                     \
    {                                                                          \
        fprintf(stderr, "ERROR:  " name " failed at %s: %d\n",__FILE__,__LINE__);\
        exit(EXIT_FAILURE);                                                    \
    }                                                                          \
}                                                                              \
while (0)

#define CALL_STATUS(func, name, status)                                        \
do                                                                             \
{                                                                              \
    if ((func) != (status))                                                    \
    {                                                                          \
        fprintf(stderr, "ERROR:  " name " failed at %s: %d\n",__FILE__,__LINE__);\
        exit(EXIT_FAILURE);                                                    \
    }                                                                          \
}                                                                              \
while (0)

#define FUNCTION_CALL_STATUS(res, func, name, status)                          \
do                                                                             \
{                                                                              \
    if ((res = func) != (status))                                              \
    {                                                                          \
        fprintf(stderr, "ERROR:  " name " failed at %s: %d\n",__FILE__,__LINE__);\
        exit(EXIT_FAILURE);                                                    \
    }                                                                          \
}                                                                              \
while (0)
#define PERSISTENT_CALL(func)                                                  \
do {} while (!(func))

#define PERSISTENT_FUNCTION_CALL(res, func)                                    \
do {} while (!((res) = (func)))

#define PERSISTENT_CALL_STATUS(func, status)                                   \
do {} while ((func) != (status))

#define PERSISTENT_FUNCTION_CALL_STATUS(func, status)                          \
do {} while (((res) = (func)) != (status))

#endif
