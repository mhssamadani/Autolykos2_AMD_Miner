#ifndef DEFINITIONS_H
#define DEFINITIONS_H

/*******************************************************************************

    DEFINITIONS -- Constants, Structs and Macros

*******************************************************************************/

#include "jsmn.h" 
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <time.h>
#include <atomic>
#include <mutex>
#include <string.h>

#define global
#include "OCLdefs.h"
#undef global





// puzzle global info
struct info_t
{
	// Mutex for reading/writing data from info_t safely
	std::mutex info_mutex;

	// Mutex for curl usage/maybe future websocket
	// not used now
	// std::mutex io_mutex;
	uint8_t AlgVer;
	// Puzzle data to read
	uint8_t bound[NUM_SIZE_8];
	uint8_t poolbound[NUM_SIZE_8];
	uint8_t mes[NUM_SIZE_8];
	uint8_t sk[NUM_SIZE_8];
	uint8_t pk[PK_SIZE_8];
	char skstr[NUM_SIZE_4];
	char pkstr[PK_SIZE_4 + 1];
	int keepPrehash;
	char to[MAX_URL_SIZE];
	char pool[MAX_URL_SIZE];
	uint8_t Hblock[HEIGHT_SIZE];

	// Increment when new block is sent by node
	std::atomic<uint_t> blockId;
};

// json string for CURL http requests and config 
struct json_t
{
	size_t cap;
	size_t len;
	char * ptr;
	jsmntok_t * toks;

	json_t(const int strlen, const int toklen);
	json_t(const json_t & newjson);
	~json_t(void);

	// reset len to zero
	void Reset(void) { len = 0; return; }

	// tokens access methods
	int GetTokenStartPos(const int pos) { return toks[pos].start; }
	int GetTokenEndPos(const int pos) { return toks[pos].end; }
	int GetTokenLen(const int pos) { return toks[pos].end - toks[pos].start; }

	char * GetTokenStart(const int pos) { return ptr + toks[pos].start; }
	char * GetTokenEnd(const int pos) { return ptr + toks[pos].end; }

	// token name check
	int jsoneq(const int pos, const char * str);
};


#endif // DEFINITIONS_H
