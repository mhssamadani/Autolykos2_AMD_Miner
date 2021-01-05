#pragma once

#include "cryptography.h"
#include "definitions.h"
#include "easylogging++.h"
#include "request.h"
#include "clPreHash.h"
#include "clMining.h"


#include <curl/curl.h>
#include <inttypes.h>
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>
#include <random>


#include "cryptography.h"
#include "conversion.h"
#include "definitions.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/opensslv.h>
#include <random>


//#include "InfInt.h"
class AutolykosAlg
{
public:
	unsigned long long CONST_MESS[CONST_MES_SIZE_8 / 8];

	AutolykosAlg();
	~AutolykosAlg();
	int m_iAlgVer;
	void Blake2b256(const char * in, const int len, uint8_t * sk, char * skstr);
	void Blake2b256_half1(const char * in, const int len, ctx_t &ctx);
	void Blake2b256_half2(ctx_t ctx, uint8_t * sk, char * skstr);
	void GenIdex(const char * in, const int len, cl_uint* index);
	void GenIdex2(const char * in, const int len, cl_uint* index);
	void  GenElements(const char * hh, int len, const char * ret);
	void hashIn(const char * in, const int len, uint8_t * sk);
	void hashIn2(const char * in1, const int len, uint8_t * sk);
	void decodes(
		char * mes,
		char   * pkstr,
		char   * w,
		char   *nonce,
		char  * d);

	void setSolution(
		const uint8_t * mes,
		char  * m_str,
		const uint8_t * w,
		char  * w_str,
		const uint8_t * nonce,
		char  *nonce_str,
		const uint8_t * d,
		char * d_str,
		int *len
		);
	void RunAlg(
		int AlgVer,
		uint8_t *B_mes,
		uint8_t *B_pk,
		uint8_t * B_sk,
		uint8_t * B_w,
		uint8_t * B_x,
		uint8_t *nonce,
		uint8_t *bPool,
		uint8_t *height
		);
	void TestBlake();
};

