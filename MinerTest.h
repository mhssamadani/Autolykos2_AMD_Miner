#pragma once
//
////#include "OCLdecs.h" ////problem with relative path
//
//#include <CL/cl.h>
//#include <string.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <iostream>
//#include <string>
//#include <fstream>
//
//#define SUCCESS 0
//#define FAILURE 1
//
//using namespace std;
////namespace ch = std::chrono;
////using namespace std::chrono;
//
//
//#include "definitions.h"
//#include "cl_warpper.h"
//#include <string.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <iostream>
//#include <string>
//#include <fstream>
//
//
//
//
//
//
//
//
//
//
//#define  fn_mulHi(Val1, Val2, cv, Result,ret)  \
//do  \
//{  \
//	cl_ulong tmp = (cl_ulong)Val1 * (cl_ulong)Val2 + (cl_ulong)+cv;\
//	Result = tmp >> 32;\
//	ret = 0;\
//}\
//while(0) \
//
//
//#define fn_mulLow(Val1, Val2, Result,ret)   \
//do   \
//{                                                                            \
//cl_ulong tmp = (cl_ulong)Val1 * (cl_ulong)Val2; \
//Result = tmp; \
//ret = tmp >> 32; \
//}                                                                            \
//while (0)\
//
////#define fn_mulLow(Val1, Val2, Result,ret)   \
////do                                                                           \
////{                                                                            \
////cl_ulong tmp = (cl_ulong)Val1 * (cl_ulong)Val2; \
////Result = tmp; \
////ret = tmp >> 32; \
////}                                                                            \
////while (0)
//
////#define fn_mulLow(Val1, Val2, Result,ret) \
////do   \
////{                                                                            \
////cl_ulong tmp = (cl_ulong)Val1 * (cl_ulong)Val2; \
////Result = tmp; \
////ret = tmp >> 32; \
////}                                                                            \
////while (0)\
//
//
//
//#define fn_Add(Val1, Val2, cv, Result,ret) \
//do \
//{  \
//	cl_ulong tmp = (cl_ulong)Val1 + (cl_ulong)Val2 + (cl_ulong)cv; \
//	Result = tmp; \
//	ret = tmp >> 32;\
//}  \
//while(0)
//
//
//
//#define fn_Sub(Val1, Val2, cv, Result,ret) \
//do \
//{  \
//	cl_uint newcv = 0; \
//	if (Val1 < Val2 && Val2 - Val1>cv)newcv = 1; \
//	Result = Val1 - Val2 - cv; \
//	ret = newcv; \
//} \
//while(0)
//
//#define fn_MadLo(Val1, Val2, cv, Result,ret) \
//do \
//{ \
//	cl_ulong tmp = Result; \
//	/*r[1]*/tmp += (cl_ulong)(Val1*Val2) + cv; \
//	Result = tmp; \
//	tmp = tmp >> 32; \
//	ret = tmp; \
//} \
//while(0)
//
//
//
//
//#define fn_MadHi(Val1, Val2, cv, Result,ret)  \
//do \
//{ \
//	cl_ulong tmp = Result; \
//	tmp += (((cl_ulong)Val1 * (cl_ulong)Val2) >> 32) + cv; \
//	Result = tmp; \
//	ret= tmp >> 32; \
//} \
//while(0)\
//
//
//
//
//
//
//
//static const const unsigned long long  ivals[8] = {
//	0x6A09E667F2BDC928, 0xBB67AE8584CAA73B, 0x3C6EF372FE94F82B,
//	0xA54FF53A5F1D36F1, 0x510E527FADE682D1, 0x9B05688C2B3E6C1F,
//	0x1F83D9ABFB41BD6B, 0x5BE0CD19137E2179
//};
//static void __byte_perm_ByteGather(unsigned int *input, unsigned int *output)
//{
//	uint8_t *inChar = (uint8_t *)input;
//	uint8_t *outChar = (uint8_t *)output;
//	outChar[0] = inChar[3];
//	outChar[1] = inChar[2];
//	outChar[2] = inChar[1];
//	outChar[3] = inChar[0];
//}
//
//static void BlakeHash_(int gId, const unsigned int *data, const unsigned long long  base,
//	unsigned int *BHashes, const ctx_t *ctt) {
//	unsigned int tid;
//
//	//printf("\n Start Cpu ");
//	unsigned long long  aux[32];
//
//	// ctx_t * ctx = ctt;
//	ctx_t *ctx = (ctx_t *)(ctt);
//
//	//int tt = 0;// get__id(0);
//
//	//if (tt != 0)
//	//	return;
//	//printf("\n %d ", tt);
//	unsigned int j;
//	unsigned int non[NONCE_SIZE_32];
//	//#pragma unroll
//	int ii = 0;
//	while (ii < 4)
//		//for (int ii = 0; ii < 4; ii++)
//	{
//		tid = (NONCES_PER_ITER / 4) * ii + gId;// get__id(0); // threadIdx.x + blockDim.x * blockIdx.x;
//
//											   //printf("\n %d ", 11);
//		unsigned int CV;
//
//		/*
//		asm volatile (
//		"add.cc.u32 %0, %1, %2;":
//		"=r"(non[0]): "r"(((uint32_t
//		*)&base)[0]), "r"(tid)
//		);
//		*/
//		fn_Add(((unsigned int *)&base)[0], tid, 0, non[0], CV);
//
//		/*
//		asm volatile (
//		"addc.u32 %0, %1, 0;":
//		"=r"(non[1]): "r"(((uint32_t
//		*)&base)[1])
//		);
//		*/
//		non[1] = 0;
//		fn_Add(((unsigned int *)&base)[1], 0, CV, non[1], CV);
//
//		unsigned long long  tmp;
//		//((unsigned int*)(&tmp))[0] = __byte_perm( non[1], 0 , 0x0123);
//		__byte_perm_ByteGather(&non[1], &((unsigned int *)(&tmp))[0]);
//		//((unsigned int*)(&tmp))[1] = __byte_perm( non[0], 0 , 0x0123);
//		__byte_perm_ByteGather(&non[0], &((unsigned int *)(&tmp))[1]);
//
//		B2B_IV(aux);
//		B2B_IV(aux + 8);
//		aux[0] = ivals[0];
//		((unsigned long long  *)(aux))[12] ^= 40;
//		((unsigned long long  *)(aux))[13] ^= 0;
//
//		((unsigned long long  *)(aux))[14] = ~((unsigned long long  *)(aux))[14];
//
//		((unsigned long long  *)(aux))[16] = ((unsigned long long  *)(((ctx_t *)(ctx))->b))[0];
//		((unsigned long long  *)(aux))[17] = ((unsigned long long  *)(((ctx_t *)(ctx))->b))[1];
//		((unsigned long long  *)(aux))[18] = ((unsigned long long  *)(((ctx_t *)(ctx))->b))[2];
//		((unsigned long long  *)(aux))[19] = ((unsigned long long  *)(((ctx_t *)(ctx))->b))[3];
//		((unsigned long long  *)(aux))[20] = tmp;
//		((unsigned long long  *)(aux))[21] = 0;
//		((unsigned long long  *)(aux))[22] = 0;
//		((unsigned long long  *)(aux))[23] = 0;
//		((unsigned long long  *)(aux))[24] = 0;
//		((unsigned long long  *)(aux))[25] = 0;
//		((unsigned long long  *)(aux))[26] = 0;
//		((unsigned long long  *)(aux))[27] = 0;
//		((unsigned long long  *)(aux))[28] = 0;
//		((unsigned long long  *)(aux))[29] = 0;
//		((unsigned long long  *)(aux))[30] = 0;
//		((unsigned long long  *)(aux))[31] = 0;
//
//		B2B_MIX(aux, aux + 16);
//
//		unsigned long long  hsh;
//		unsigned int tmpD;
//		//	printf("\n %d ", 33);
//		//printf("\n %d ", j);
//		//	printf("\n %d ", 44);
//#pragma unroll
//		for (j = 0; j < NUM_SIZE_32; j += 2)
//		{
//			//	printf("\n %d ", j);
//			hsh = ivals[j >> 1];
//			hsh ^= ((unsigned long long  *)(aux))[j >> 1] ^ ((unsigned long long  *)(aux))[8 + (j >> 1)];
//
//			// BHashes[THREADS_PER_ITER*j + tid] = __byte_perm (
//			// ((unsigned int*)(&hsh))[0], 0 , 0x0123);
//			__byte_perm_ByteGather(&((unsigned int *)(&hsh))[0], &tmpD);
//			BHashes[THREADS_PER_ITER * j + tid] = tmpD;
//
//			// BHashes[THREADS_PER_ITER*(j+1) + tid] = __byte_perm (
//			// ((unsigned int*)(&hsh))[1], 0 , 0x0123);
//			__byte_perm_ByteGather(&((unsigned int *)(&hsh))[1], &tmpD);
//			BHashes[THREADS_PER_ITER * (j + 1) + tid] = tmpD;
//			//if ((THREADS_PER_ITER * j + tid) == THREADS_PER_ITER)
//			//	printf("\n %d %d %d %d %d ", gId, ii, tid, j, tmpD);
//
//			//if ((THREADS_PER_ITER * (j + 1) + tid) == THREADS_PER_ITER)
//			//	printf("\n %d %d %d %d %d ", gId, ii, tid, j, tmpD);
//			//if (tid == 0)
//			//{
//			//	printf("\n tid : %d  , index : %d , tmp : %d", tid, THREADS_PER_ITER*j + tid, BHashes[THREADS_PER_ITER*j + tid]);
//			//	printf("\n tid : %d  , index : %d , tmp : %d", tid, THREADS_PER_ITER*(j + 1) + tid, BHashes[THREADS_PER_ITER*(j + 1) + tid]);
//			//}
//		}
//		ii++;
//	}
//	//for (int k = 0; k < 32; k++)
//	//{
//	//	printf("\n tid : %d  , k : %d , (uint8_t * )BHashes[k] : %d ", tid, k, ((uint8_t *)BHashes)[k]);
//	//}
//	//printf("\n End Cpu ");
//}
//
//
//static  unsigned int __funnelshift_l(unsigned int low32, unsigned int high32, unsigned int shiftWidth) {
//	unsigned int result;
//	unsigned long long  tmp;
//	((uint8_t *)(&tmp))[0] = ((uint8_t *)(&low32))[0];
//	((uint8_t *)(&tmp))[1] = ((uint8_t *)(&low32))[1];
//	((uint8_t *)(&tmp))[2] = ((uint8_t *)(&low32))[2];
//	((uint8_t *)(&tmp))[3] = ((uint8_t *)(&low32))[3];
//	((uint8_t *)(&tmp))[4] = ((uint8_t *)(&high32))[0];
//	((uint8_t *)(&tmp))[5] = ((uint8_t *)(&high32))[1];
//	((uint8_t *)(&tmp))[6] = ((uint8_t *)(&high32))[2];
//	((uint8_t *)(&tmp))[7] = ((uint8_t *)(&high32))[3];
//
//	unsigned long long  tmp2 = tmp << (shiftWidth & 31);
//
//	((uint8_t *)(&result))[0] = ((uint8_t *)(&tmp2))[4];
//	((uint8_t *)(&result))[1] = ((uint8_t *)(&tmp2))[5];
//	((uint8_t *)(&result))[2] = ((uint8_t *)(&tmp2))[6];
//	((uint8_t *)(&result))[3] = ((uint8_t *)(&tmp2))[7];
//
//	return result;
//}
//
//static void BlockMining_(
//	// boundary for puzzle
//	const cl_uint* bound,
//	// p boundary for puzzle
//	const cl_uint* pbound,
//	// data:  mes || ctx
//	const cl_uint* mes,
//	// nonce base
//	cl_ulong base,
//	// block height
//	cl_uint  height,
//	// precalculated hashes
//	const cl_uint* hashes,
//	// results
//	cl_uint* res,
//	// indices of valid solutions
//	cl_uint* valid,
//	cl_uint* vCount,
//
//	// P results
//	cl_uint* Pres,
//	// indices of P valid solutions
//	cl_uint* Pvalid,
//	cl_uint* Pcount
//
//
//	)
//
//{
//
//	cl_uint tid = 1;
//
//	// local memory
//	// 472 bytes
//	//cl_uint ldata[200];
//
//	// 32 * 64 bits = 256 bytes
//	//cl_ulong * aux = (cl_ulong *)ldata;
//	cl_ulong  aux[32];
//	int tidg = 0;
//	// (212 + 4) bytes
//	ctx_t sdata;
//	for (int i = 0; i < CTX_SIZE; ++i)
//	{
//		((uint8_t *)&sdata)[i] = (( uint8_t *)mes)[NUM_SIZE_8 + i];
//
//	}
//	ctx_t *ctx = ((ctx_t *)(&sdata));//&lctx;//(ctx_t *)(ldata + 64);
//
//									 // (4 * K_LEN) bytes
//									 //cl_uint* ind = ldata;
//	cl_uint ind[32];
//
//
//	//cl_uint* r = ind + K_LEN;
//	cl_uint r[32];  /*????*/
//
//
//	cl_uint CV;
//
//	/*
//	if (tidg == 0)
//	{
//	for (int k = 0; k < ctx->c; ++k)
//	{
//	printf( " %d  ",ctx->b[k]);
//	}
//	}
//	*/
//	//if(tidg != 0)
//	//	return;
////#pragma unroll
//	//for (int t = 0; t < NONCES_PER_THREAD; ++t)
//	//{
//
//		//if (tid < NONCES_PER_ITER)
//		//{
//			cl_uint j;
//			cl_uint pj;
//			cl_uint non[NONCE_SIZE_32];
//
//			fn_Add(((cl_uint*)& base)[0], tid, 0, non[0], CV);
//
//			non[1] = 0;
//			fn_Add(((cl_uint*)& base)[1], 0, CV, non[1], CV);
//
//
//
//			//================================================================//
//			//  Hash nonce
//			//================================================================//
//
//#pragma unroll
//			for (j = 0; ctx->c < BUF_SIZE_8 && j < NONCE_SIZE_8; ++j)
//			{
//				ctx->b[ctx->c++] = ((uint8_t *)non)[NONCE_SIZE_8 - j - 1];
//			}
//
//#pragma unroll
//			for (; j < NONCE_SIZE_8;)
//			{
//				HOST_B2B_H(ctx, aux);
//
//#pragma unroll
//				for (; ctx->c < BUF_SIZE_8 && j < NONCE_SIZE_8; ++j)
//				{
//					ctx->b[ctx->c++] = ((uint8_t*)non)[NONCE_SIZE_8 - j - 1];
//				}
//			}
//			//================================================================//
//			//  Finalize hashes
//			//================================================================//
//
//			HOST_B2B_H_LAST(ctx, aux);
//
//#pragma unroll
//			for (j = 0; j < NUM_SIZE_8; ++j)
//			{
//				((uint8_t*)r)[j] = (ctx->h[j >> 3] >> ((j & 7) << 3)) & 0xFF;
//			}
//
//			cl_ulong h2;
//			((uint8_t*)&h2)[0] = ((uint8_t*)r)[31];
//			((uint8_t*)&h2)[1] = ((uint8_t*)r)[30];
//			((uint8_t*)&h2)[2] = ((uint8_t*)r)[29];
//			((uint8_t*)&h2)[3] = ((uint8_t*)r)[28];
//			((uint8_t*)&h2)[4] = ((uint8_t*)r)[27];
//			((uint8_t*)&h2)[5] = ((uint8_t*)r)[26];
//			((uint8_t*)&h2)[6] = ((uint8_t*)r)[25];
//			((uint8_t*)&h2)[7] = ((uint8_t*)r)[24];
//			if (tid == 0)
//			{
//				for (int kj = 0; kj<32; kj++)
//					printf("%d", ((uint8_t*)r)[kj]);
//			}
//			cl_uint h3 = h2 % N_LEN;
//			uint8_t iii[4];
//			iii[0] = ((uint8_t *)(&h3))[3];
//			iii[1] = ((uint8_t *)(&h3))[2];
//			iii[2] = ((uint8_t *)(&h3))[1];
//			iii[3] = ((uint8_t *)(&h3))[0];
//
//			//====================================================================//
//			//  Initialize context
//			//====================================================================//
//			//memset(ctx->b, 0, BUF_SIZE_8);
//#pragma unroll
//			for (int am = 0; am < BUF_SIZE_8; am++)
//			{
//				ctx->b[am] = 0;
//			}
//			B2B_IV(ctx->h);
//
//
//
//			ctx->h[0] ^= 0x01010000 ^ NUM_SIZE_8;
//			//memset(ctx->t, 0, 16);
//			ctx->t[0] = 0;
//			ctx->t[1] = 0;
//			ctx->c = 0;
//
//			//====================================================================//
//			//  Hash
//			//====================================================================//
//#pragma unroll
//			for (j = 0; ctx->c < BUF_SIZE_8 && j < HEIGHT_SIZE; ++j)
//			{
//				ctx->b[ctx->c++] = iii[j];
//			}
//
//			//====================================================================//
//			//  Hash height
//			//====================================================================//
//#pragma unroll
//			for (j = 0; ctx->c < BUF_SIZE_8 && j < HEIGHT_SIZE; ++j)
//			{
//				ctx->b[ctx->c++] = ((const uint8_t *)&height)[HEIGHT_SIZE - j - 1];
//			}
//
//
//			//====================================================================//
//			//  Hash constant message
//			//====================================================================//
//#pragma unroll
//			for (j = 0; ctx->c < BUF_SIZE_8 && j < CONST_MES_SIZE_8; ++j)
//			{
//				ctx->b[ctx->c++]
//					= (
//						!((7 - (j & 7)) >> 1)
//						* ((j >> 3) >> (((~(j & 7)) & 1) << 3))
//						) & 0xFF;
//			}
//
//
//			while (j < CONST_MES_SIZE_8)
//			{
//				HOST_B2B_H(ctx, aux);
//
//				for (; ctx->c < BUF_SIZE_8 && j < CONST_MES_SIZE_8; ++j)
//				{
//					ctx->b[ctx->c++]
//						= (
//							!((7 - (j & 7)) >> 1)
//							* ((j >> 3) >> (((~(j & 7)) & 1) << 3))
//							) & 0xFF;
//				}
//			}
//
//			//====================================================================//
//			//  Finalize hash
//			//====================================================================//
//
//
//
//			HOST_B2B_H_LAST(ctx, aux);
//
//
//#pragma unroll
//			for (j = 0; j < NUM_SIZE_8; ++j)
//			{
//				((uint8_t*)r)[j] = (ctx->h[j >> 3] >> ((j & 7) << 3)) & 0xFF;
//				//((uint8_t*)r)[NUM_SIZE_8 - j - 1] = (ctx->h[j >> 3] >> ((j & 7) << 3)) & 0xFF;
//			}
//
//			//====================================================================//
//
//
//			//--------------------------------------------------
//
//			//====================================================================//
//			//  Initialize context
//			//====================================================================//
//			//memset(ctx->b, 0, BUF_SIZE_8);
//#pragma unroll
//			for (int am = 0; am < BUF_SIZE_8; am++)
//			{
//				ctx->b[am] = 0;
//			}
//			B2B_IV(ctx->h);
//
//
//
//			ctx->h[0] ^= 0x01010000 ^ NUM_SIZE_8;
//			//memset(ctx->t, 0, 16);
//			ctx->t[0] = 0;
//			ctx->t[1] = 0;
//			ctx->c = 0;
//
//			//====================================================================//
//			//  Hash
//			//====================================================================//
//#pragma unroll
//			for (j = 0; ctx->c < BUF_SIZE_8 && j < NUM_SIZE_8 - 1; ++j)
//			{
//				ctx->b[ctx->c++] = ((const uint8_t *)r)[j + 1];
//			}
//
//			//====================================================================//
//			//  Hash message
//			//====================================================================//
//#pragma unroll
//			for (j = 0; ctx->c < BUF_SIZE_8 && j < NUM_SIZE_8; ++j)
//			{
//				ctx->b[ctx->c++] = (( const uint8_t *)mes)[j];
//			}
//
//
//
//			while (j < NUM_SIZE_8)
//			{
//				HOST_B2B_H(ctx, aux);
//
//				while (ctx->c < BUF_SIZE_8 && j < NUM_SIZE_8)
//				{
//					ctx->b[ctx->c++] = (( const uint8_t *)mes)[j++];
//				}
//			}
//
//			//================================================================//
//			//  Hash nonce
//			//================================================================//
//
//#pragma unroll
//			for (j = 0; ctx->c < BUF_SIZE_8 && j < NONCE_SIZE_8; ++j)
//			{
//				ctx->b[ctx->c++] = ((uint8_t *)non)[NONCE_SIZE_8 - j - 1];
//			}
//
//#pragma unroll
//			for (; j < NONCE_SIZE_8;)
//			{
//				HOST_B2B_H(ctx, aux);
//
//#pragma unroll
//				for (; ctx->c < BUF_SIZE_8 && j < NONCE_SIZE_8; ++j)
//				{
//					ctx->b[ctx->c++] = ((uint8_t*)non)[NONCE_SIZE_8 - j - 1];
//				}
//			}
//			//====================================================================//
//			//  Finalize hash
//			//====================================================================//
//
//
//
//			HOST_B2B_H_LAST(ctx, aux);
//
//
//#pragma unroll
//			for (j = 0; j < NUM_SIZE_8; ++j)
//			{
//				((uint8_t*)r)[(j & 0xFFFFFFFC) + (3 - (j & 3))] = (ctx->h[j >> 3] >> ((j & 7) << 3)) & 0xFF;
//				//((uint8_t*)r)[NUM_SIZE_8 - j - 1] = (ctx->h[j >> 3] >> ((j & 7) << 3)) & 0xFF;
//			}
//
//
//			//================================================================//
//			//  Generate indices
//			//================================================================//
//
//#pragma unroll
//			for (int i = 1; i < INDEX_SIZE_8; ++i)
//			{
//				((uint8_t*)r)[NUM_SIZE_8 + i] = ((uint8_t*)r)[i];
//			}
//
//#pragma unroll
//			int k = 0;
//			int i = 1;
//			for (k = 0; k < K_LEN; k += INDEX_SIZE_8)
//			{
//				ind[k] = r[k >> 2] & N_MASK;
//
//#pragma unroll
//				for (i = 1; i < INDEX_SIZE_8; ++i)
//				{
//					ind[k + i] = ((r[k >> 2] << (i << 3)) | (r[(k >> 2) + 1] >> (32 - (i << 3)))) & N_MASK;
//				}
//			}
//
//
//			for(int p=0;p<32;p++)ind[p] = p;  //-------------------------------------
//			//================================================================//
//			//  Calculate result
//			//================================================================//
//			// if (tid == 0)
//			// {
//			// //printf(" ----------------");
//			// for (j = 0; j < 32; ++j)
//			// {
//
//			// 	printf("%d ", ((uint8_t  *)&hashes[ind[10] << 3])[j]);
//			// }
//			// }
//			// first addition of hashes -> r
//			fn_Add(hashes[ind[0] << 3], hashes[ind[1] << 3], 0, r[0], CV);
//			// asm volatile ("add.cc.u32 %0, %1, %2;":"=r"(r[0]): "r"(hashes[ind[0] << 3]), "r"(hashes[ind[1] << 3]));
//
//#pragma unroll
//			for (int i = 1; i < 8; ++i)
//			{
//				fn_Add(hashes[(ind[0] << 3) + i], hashes[(ind[1] << 3) + i], CV, r[i], CV);
//				//asm volatile ("addc.cc.u32 %0, %1, %2;":"=r"(r[i]):"r"(hashes[(ind[0] << 3) + i]),"r"(hashes[(ind[1] << 3) + i]));
//			}
//			r[8] = 0;
//			fn_Add(r[8], 0, CV, r[8], CV);
//			// asm volatile ("addc.u32 %0, 0, 0;": "=r"(r[8]));
//
//			// remaining additions
//
//			int k1 = 2;
//			int i1 = 1;
////#pragma unroll
//			for (k1 = 2; k1 < K_LEN; ++k1)
//			{
//				fn_Add(r[0], hashes[ind[k1] << 3], 0, r[0], CV);
//				//asm volatile ("add.cc.u32 %0, %0, %1;":"+r"(r[0]): "r"(hashes[ind[k] << 3]));
//
////#pragma unroll
//				for (i1 = 1; i1 < 8; ++i1)
//				{
//					fn_Add(r[i1], hashes[(ind[k1] << 3) + i1], CV, r[i1], CV);
//					//asm volatile ("addc.cc.u32 %0, %0, %1;":"+r"(r[i]): "r"(hashes[(ind[k] << 3) + i]));
//				}
//				fn_Add(r[8], 0, CV, r[8], CV);
//				//asm volatile ("addc.u32 %0, %0, 0;": "+r"(r[8]));
//			}
//
//
//
//
//
//
//
//			//--------------------hash f
//			//====================================================================//
//			//  Initialize context
//			//====================================================================//
//			//memset(ctx->b, 0, BUF_SIZE_8);
//#pragma unroll
//			for (int am = 0; am < BUF_SIZE_8; am++)
//			{
//				ctx->b[am] = 0;
//			}
//			B2B_IV(ctx->h);
//
//
//
//			ctx->h[0] ^= 0x01010000 ^ NUM_SIZE_8;
//			//memset(ctx->t, 0, 16);
//			ctx->t[0] = 0;
//			ctx->t[1] = 0;
//			ctx->c = 0;
//
//
//			//--------------hash
//#pragma unroll
//			for (j = 0; ctx->c < BUF_SIZE_8 && j < NUM_SIZE_8; ++j)
//			{
//				ctx->b[ctx->c++] = ((const uint8_t *)r)[NUM_SIZE_8-1-j];
//			}
//
//
//
//			while (j < NUM_SIZE_8)
//			{
//				HOST_B2B_H(ctx, aux);
//
//				while (ctx->c < BUF_SIZE_8 && j < NUM_SIZE_8)
//				{
//					ctx->b[ctx->c++] = ((const uint8_t *)r)[j++];
//				}
//			}
//			//====================================================================//
//			//  Finalize hash
//			//====================================================================//
//
//
//			HOST_B2B_H_LAST(ctx, aux);
//
//
//#pragma unroll
//			for (j = 0; j < NUM_SIZE_8; ++j)
//			{
//				//((uint8_t*)r)[(j & 0xFFFFFFFC) + (3 - (j & 3))] = (ctx->h[j >> 3] >> ((j & 7) << 3)) & 0xFF;
//				((uint8_t*)r)[NUM_SIZE_8 - j - 1] = (ctx->h[j >> 3] >> ((j & 7) << 3)) & 0xFF;
//			}
//
//			/*
//			if (tid == 0)
//			{
//			//printf(" ----------------");
//			for (j = 0; j < 32; ++j)
//			{
//
//			printf("%d ", ((uint8_t *)r)[j]);
//			}
//			}
//			*/
//
//			//================================================================//
//			//  Dump result to  memory -- LITTLE ENDIAN
//			//================================================================//
////		}
////	}
//	return;
//}
//
//static void Blake2b256_(const char * in,
//	const int len,
//	uint8_t * sk,
//	char * skstr)
//{
//	ctx_t ctx;
//	uint64_t aux[32];
//
//
//
//
//
//	//====================================================================//
//	//  Initialize context
//	//====================================================================//
//	memset(ctx.b, 0, 128);
//	B2B_IV(ctx.h);
//	ctx.h[0] ^= 0x01010000 ^ NUM_SIZE_8;
//	memset(ctx.t, 0, 16);
//	ctx.c = 0;
//
//	//====================================================================//
//	//  Hash message
//	//====================================================================//
//	for (int i = 0; i < len; ++i)
//	{
//		if (ctx.c == 128) { HOST_B2B_H(&ctx, aux); }
//
//		ctx.b[ctx.c++] = (uint8_t)(in[i]);
//	}
//
//	HOST_B2B_H_LAST(&ctx, aux);
//
//	for (int i = 0; i < NUM_SIZE_8; ++i)
//	{
//		sk[NUM_SIZE_8 - i - 1] = (ctx.h[i >> 3] >> ((i & 7) << 3)) & 0xFF;
//	}
//
//	//uint8_t s_tmp[32];
//	//for (int i = 0; i < 32 ; i+=4)
//	//{
//	//	s_tmp[i] = sk[32 - i - 4];
//	//	s_tmp[i + 1] = sk[32 - i - 3];
//	//	s_tmp[i + 2] = sk[32 - i - 2];
//	//	s_tmp[i + 3] = sk[32 - i - 1];
//	//}
//	////====================================================================//
//	////  Mod Q
//	////====================================================================//
//	//uint8_t borrow[2];
//
//	//borrow[0] = ((uint64_t*)sk)[0] < Q0;
//	//aux[0] = ((uint64_t*)sk)[0] - Q0;
//
//	//borrow[1] = ((uint64_t*)sk)[1] < Q1 + borrow[0];
//	//aux[1] = ((uint64_t*)sk)[1] - Q1 - borrow[0];
//
//	//borrow[0] = ((uint64_t*)sk)[2] < Q2 + borrow[1];
//	//aux[2] = ((uint64_t*)sk)[2] - Q2 - borrow[1];
//
//	//borrow[1] = ((uint64_t*)sk)[3] < Q3 + borrow[0];
//	//aux[3] = ((uint64_t*)sk)[3] - Q3 - borrow[0];
//
//	//if (!(borrow[1] || borrow[0])) { memcpy(sk, aux, NUM_SIZE_8); }
//
//	// convert secret key to hex string
//	LittleEndianToHexStr(sk, NUM_SIZE_8, skstr);
//
//}
//
//static void MiningADD_()
//{
//	cl_uint ldata[/*118 aminm */ 200];
//
//	// 256 bytes
//	cl_ulong* aux = (cl_ulong*)ldata;
//	// (4 * K_LEN) bytes
//	cl_uint* ind = ldata;
//	// (NUM_SIZE_8 + 4) bytes
//	cl_uint* r = ind + K_LEN;
//	// (212 + 4) bytes
//	ctx_t* ctx = (ctx_t*)(ldata + 64);
//
//	cl_uint CV;
//
//	cl_uint *hashes = new cl_uint[32 * 32];
//	memset(hashes, 0, 32 * 32);
//
//
//	FILE *iF = fopen("ind.dat", "rb");
//	uint8_t tmpB[32][32];
//
//	int t = 0;
//	for (t = 0; t < 32; t++)
//	{
//		fread(tmpB[t], 1, 32, iF);
//		ind[t] = t;
//
//		for (size_t i = 0; i < 31; i++)
//		{
//			((uint8_t *)hashes)[t * 32 + i] = tmpB[t][31 - i];
//
//		}
//		((uint8_t *)hashes)[t * 32 + 31] = 0;
//
//	}
//
//	char *LSUMM;
//	char *LB;
//	char *LSUMM2;
//	char *LB2;
//
//	int rep = 0;
//	BIGNUM* bigsum = BN_new();
//	CALL(BN_dec2bn(&bigsum, "0"), ERROR_OPENSSL);
//
//
//	BIGNUM* bigres = BN_new();
//	CALL(BN_dec2bn(&bigres, "0"), ERROR_OPENSSL);
//
//	unsigned char f[32];
//	memset(f, 0, 32);
//
//	for (rep = 0; rep < 32; rep++)
//	{
//		CALL(BN_bin2bn(&((const unsigned char *)hashes)[rep * 32], 31, bigres), ERROR_OPENSSL);
//
//		CALL(BN_add(bigsum, bigsum, bigres), ERROR_OPENSSL);
//
//		LB = BN_bn2dec(bigres);
//		LSUMM = BN_bn2dec(bigsum);
//
//
//		//LOG(INFO) << LB;
//		//LOG(INFO) << LB2;
//		//LOG(INFO) << LSUMM;
//		//LOG(INFO) << LSUMM2;
//		BN_bn2bin(bigsum, f);
//
//	}
//
//
//
//	//================================================================//
//	//  Calculate result
//	//================================================================//
//	// first addition of hashes -> r
//	//fn_Add(((uint8_t *)hashes)[ind[0] * NUM_SIZE_8], ((uint8_t *)hashes)[ind[1] * NUM_SIZE_8], 0, r[0], CV);
//	fn_Add(hashes[ind[0] << 3], hashes[ind[1] << 3], 0, r[0], CV);
//
//	for (int i = 1; i < 8; ++i)
//	{
//
//		fn_Add(hashes[(ind[0] << 3) + i], hashes[(ind[1] << 3) + i], CV, r[i], CV);
//		//asm volatile ("addc.cc.u32 %0, %1, %2;":"=r"(r[i]):"r"(hashes[(ind[0] << 3) + i]),"r"(hashes[(ind[1] << 3) + i]));
//	}
//	r[8] = 0;
//	fn_Add(r[8], 0, CV, r[8], CV);
//	// asm volatile ("addc.u32 %0, 0, 0;": "=r"(r[8]));
//
//	// remaining additions
//	int k = 2;
//	int i = 1;
//	for (k = 2; k < K_LEN; ++k)
//	{
//		fn_Add(r[0], hashes[ind[k] << 3], 0, r[0], CV);
//		//asm volatile ("add.cc.u32 %0, %0, %1;":"+r"(r[0]): "r"(hashes[ind[k] << 3]));
//
//
//		for (i = 1; i < 8; ++i)
//		{
//			fn_Add(r[i], hashes[(ind[k] << 3) + i], CV, r[i], CV);
//			//asm volatile ("addc.cc.u32 %0, %0, %1;":"+r"(r[i]): "r"(hashes[(ind[k] << 3) + i]));
//		}
//		fn_Add(r[8], 0, CV, r[8], CV);
//		//asm volatile ("addc.u32 %0, %0, 0;": "+r"(r[8]));
//	}
//
//	CALL(BN_bin2bn((const unsigned char *)r, 32, bigres), ERROR_OPENSSL);
//	LB = BN_bn2dec(bigres);
//
//	int len = 32;
//	char *skstr = new char[len * 3];
//	uint8_t *inHash = new uint8_t[len];
//
//	for (int t = 0; t < 32; t++)
//		inHash[t] = ((uint8_t *)r)[31 - t];
//	//memcpy(inHash, (uint8_t *)r, len);
//	int hashLen = len;
//
//	char hf[32];
//	Blake2b256_((const char *)inHash, hashLen, (uint8_t *)hf, skstr);
//
//
//	//char dd[100];
//	//LittleEndianOf256ToDecStr((uint8_t *)hf, dd, (uint32_t *)&len);
//	//LOG(INFO) << dd;
//
//}
