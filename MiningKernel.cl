#pragma OPENCL EXTENSION cl_amd_printf : enable

__kernel void test()
{
}
#include "OCLdecs.h"////problem with relative path
__kernel void BlockMining(
	// boundary for puzzle
	global const cl_uint* bound,
	// p boundary for puzzle
	global const cl_uint* pbound,
	// data:  mes || ctx
	global const cl_uint* mes,
	// nonce base
	const cl_ulong base,
	// block height
	 const cl_uint  height,
    // precalculated hashes
	global const cl_uint* hashes,
	// results
	global cl_uint* res,
	// indices of valid solutions
	global cl_uint* valid,
	global cl_uint* vCount,

	// P results
	global cl_uint* Pres,
	// indices of P valid solutions
	global cl_uint* Pvalid,
	global cl_uint* Pcount
	
	)

{
	cl_uint tid = get_local_id(0);

	int tidg = get_global_id(0) ;
//	if(tidg != 0)
//		return;
	// local memory
	// 472 bytes
	//cl_uint ldata[200];

	// 32 * 64 bits = 256 bytes 
	//cl_ulong * aux = (cl_ulong *)ldata;
	cl_ulong  aux[32];
		
	// (212 + 4) bytes 
	ctx_t sdata;
	for (int i = 0; i < CTX_SIZE; ++i)
	{
		((uint8_t * )&sdata)[i] = ((global uint8_t * )mes)[NUM_SIZE_8 + i];

	}
	ctx_t *ctx = ((ctx_t * )(&sdata));//&lctx;//(ctx_t *)(ldata + 64);

	// (4 * K_LEN) bytes
	//cl_uint* ind = ldata;
	cl_uint ind[32];


	//cl_uint* r = ind + K_LEN;
	cl_uint r[32];  /*????*/


	cl_uint CV;
	
#pragma unroll
	for (int t = 0; t < NONCES_PER_THREAD; ++t)
	{
		tid = get_global_id(0) + t * get_global_size(0);

		if (tid < NONCES_PER_ITER)
		{
			cl_uint j;
			cl_uint pj;
			cl_uint non[NONCE_SIZE_32];

			fn_Add(((cl_uint*)& base)[0], tid, 0, non[0], CV);

			non[1] = 0;
			fn_Add(((cl_uint*)& base)[1], 0, CV, non[1], CV);



			//================================================================//
			//  Hash nonce
			//================================================================//
#pragma unroll
			for (j = 0; ctx->c < BUF_SIZE_8 && j < NONCE_SIZE_8; ++j)
			{
				ctx->b[ctx->c++] = ((uint8_t *)non)[NONCE_SIZE_8 - j - 1];
				
			}

#pragma unroll
			for (; j < NONCE_SIZE_8;)
			{
				HOST_B2B_H(ctx, aux);

#pragma unroll
				for (; ctx->c < BUF_SIZE_8 && j < NONCE_SIZE_8; ++j)
				{
					ctx->b[ctx->c++] = ((uint8_t*)non)[NONCE_SIZE_8 - j - 1];
				}
			}
			//================================================================//
			//  Finalize hashes
			//================================================================//

			HOST_B2B_H_LAST(ctx, aux);

#pragma unroll
			for (j = 0; j < NUM_SIZE_8; ++j)
			{
				((uint8_t*)r)[j] = (ctx->h[j >> 3] >> ((j & 7) << 3)) & 0xFF;
			}

			cl_ulong h2;
			((uint8_t*)&h2)[0] = ((uint8_t*)r)[31];
			((uint8_t*)&h2)[1] = ((uint8_t*)r)[30];
			((uint8_t*)&h2)[2] = ((uint8_t*)r)[29];
			((uint8_t*)&h2)[3] = ((uint8_t*)r)[28];
			((uint8_t*)&h2)[4] = ((uint8_t*)r)[27];
			((uint8_t*)&h2)[5] = ((uint8_t*)r)[26];
			((uint8_t*)&h2)[6] = ((uint8_t*)r)[25];
			((uint8_t*)&h2)[7] = ((uint8_t*)r)[24];

			cl_uint h3 = h2 % N_LEN;
			uint8_t iii[8];
			iii[0] = ((uint8_t *)(&h3))[3];
			iii[1] = ((uint8_t *)(&h3))[2];
			iii[2] = ((uint8_t *)(&h3))[1];
			iii[3] = ((uint8_t *)(&h3))[0];

			//====================================================================//
			//  Initialize context
			//====================================================================//
			//memset(ctx->b, 0, BUF_SIZE_8);
			#pragma unroll
				for (int am = 0; am < BUF_SIZE_8; am++)
				{
					ctx->b[am] = 0;
				}
			B2B_IV(ctx->h);



			ctx->h[0] ^= 0x01010000 ^ NUM_SIZE_8;
			//memset(ctx->t, 0, 16);
			ctx->t[0] = 0;
			ctx->t[1] = 0;
			ctx->c = 0;

			//====================================================================//
			//  Hash 
			//====================================================================//
#pragma unroll
			for (j = 0; ctx->c < BUF_SIZE_8 && j < HEIGHT_SIZE; ++j)
			{
				ctx->b[ctx->c++] = iii[j];
			}

			//====================================================================//
			//  Hash height
			//====================================================================//
#pragma unroll
			for (j = 0; ctx->c < BUF_SIZE_8 && j < HEIGHT_SIZE; ++j)
			{
				ctx->b[ctx->c++] = ((const uint8_t *)&height)[j/*HEIGHT_SIZE - j - 1*/];

			}


			//====================================================================//
			//  Hash constant message
			//====================================================================//
#pragma unroll
			for (j = 0; ctx->c < BUF_SIZE_8 && j < CONST_MES_SIZE_8; ++j)
			{
				ctx->b[ctx->c++]
					= (
						!((7 - (j & 7)) >> 1)
						* ((j >> 3) >> (((~(j & 7)) & 1) << 3))
						) & 0xFF;
			}


			while (j < CONST_MES_SIZE_8)
			{
				HOST_B2B_H(ctx, aux);

				for (; ctx->c < BUF_SIZE_8 && j < CONST_MES_SIZE_8; ++j)
				{
					ctx->b[ctx->c++]
						= (
							!((7 - (j & 7)) >> 1)
							* ((j >> 3) >> (((~(j & 7)) & 1) << 3))
							) & 0xFF;
				}
			}

			//====================================================================//
			//  Finalize hash
			//====================================================================//



			HOST_B2B_H_LAST(ctx, aux);


#pragma unroll
			for (j = 0; j < NUM_SIZE_8; ++j)
			{
				((uint8_t*)r)[j] = (ctx->h[j >> 3] >> ((j & 7) << 3)) & 0xFF;
				//((uint8_t*)r)[NUM_SIZE_8 - j - 1] = (ctx->h[j >> 3] >> ((j & 7) << 3)) & 0xFF;
			}

			//====================================================================//

			//====================================================================//
			//  Initialize context
			//====================================================================//
			//memset(ctx->b, 0, BUF_SIZE_8);
#pragma unroll
			for (int am = 0; am < BUF_SIZE_8; am++)
			{
				ctx->b[am] = 0;
			}
			B2B_IV(ctx->h);



			ctx->h[0] ^= 0x01010000 ^ NUM_SIZE_8;
			//memset(ctx->t, 0, 16);
			ctx->t[0] = 0;
			ctx->t[1] = 0;
			ctx->c = 0;

			//====================================================================//
			//  Hash 
			//====================================================================//
#pragma unroll
			for (j = 0; ctx->c < BUF_SIZE_8 && j < NUM_SIZE_8 - 1; ++j)
			{
				ctx->b[ctx->c++] = ((const uint8_t *)r)[j + 1];
			}

			//====================================================================//
			//  Hash message
			//====================================================================//
#pragma unroll
			for (j = 0; ctx->c < BUF_SIZE_8 && j < NUM_SIZE_8; ++j)
			{
				ctx->b[ctx->c++] = ((global const uint8_t *)mes)[j];
			}



			while (j < NUM_SIZE_8)
			{
				HOST_B2B_H(ctx, aux);

				while (ctx->c < BUF_SIZE_8 && j < NUM_SIZE_8)
				{
					ctx->b[ctx->c++] = ((global const uint8_t *)mes)[j++];
				}
			}

			//================================================================//
			//  Hash nonce
			//================================================================//

#pragma unroll
			for (j = 0; ctx->c < BUF_SIZE_8 && j < NONCE_SIZE_8; ++j)
			{
				ctx->b[ctx->c++] = ((uint8_t *)non)[NONCE_SIZE_8 - j - 1];
			}

#pragma unroll
			for (; j < NONCE_SIZE_8;)
			{
				HOST_B2B_H(ctx, aux);

#pragma unroll
				for (; ctx->c < BUF_SIZE_8 && j < NONCE_SIZE_8; ++j)
				{
					ctx->b[ctx->c++] = ((uint8_t*)non)[NONCE_SIZE_8 - j - 1];
				}
			}
			//====================================================================//
			//  Finalize hash
			//====================================================================//



			HOST_B2B_H_LAST(ctx, aux);


#pragma unroll
			for (j = 0; j < NUM_SIZE_8; ++j)
			{
				((uint8_t*)r)[(j & 0xFFFFFFFC) + (3 - (j & 3))] = (ctx->h[j >> 3] >> ((j & 7) << 3)) & 0xFF;
				//((uint8_t*)r)[NUM_SIZE_8 - j - 1] = (ctx->h[j >> 3] >> ((j & 7) << 3)) & 0xFF;
			}
			//================================================================//
			//  Generate indices
			//================================================================//

#pragma unroll
			for (int i = 1; i < INDEX_SIZE_8; ++i)
			{
				((uint8_t*)r)[NUM_SIZE_8 + i] = ((uint8_t*)r)[i];
			}

#pragma unroll
			for (int k = 0; k < K_LEN; k += INDEX_SIZE_8)
			{
				ind[k] = r[k >> 2] & N_MASK;

#pragma unroll
				for (int i = 1; i < INDEX_SIZE_8; ++i)
				{
					ind[k + i] = ((r[k >> 2] << (i << 3)) | (r[(k >> 2) + 1] >> (32 - (i << 3)))) & N_MASK;
				}
			}
			//================================================================//
			//  Calculate result
			//================================================================//

			// first addition of hashes -> r
			fn_Add(hashes[ind[0] << 3], hashes[ind[1] << 3], 0, r[0], CV);
			// asm volatile ("add.cc.u32 %0, %1, %2;":"=r"(r[0]): "r"(hashes[ind[0] << 3]), "r"(hashes[ind[1] << 3]));
#pragma unroll
			for (int i = 1; i < 8; ++i)
			{
				fn_Add(hashes[(ind[0] << 3) + i], hashes[(ind[1] << 3) + i], CV, r[i], CV);
				//asm volatile ("addc.cc.u32 %0, %1, %2;":"=r"(r[i]):"r"(hashes[(ind[0] << 3) + i]),"r"(hashes[(ind[1] << 3) + i]));
			}
			r[8] = 0;
			fn_Add(r[8], 0, CV, r[8], CV);
			// asm volatile ("addc.u32 %0, 0, 0;": "=r"(r[8]));
			// remaining additions

#pragma unroll
			for (int k = 2; k < K_LEN; ++k)
			{
				fn_Add(r[0], hashes[ind[k] << 3], 0, r[0], CV);
				//asm volatile ("add.cc.u32 %0, %0, %1;":"+r"(r[0]): "r"(hashes[ind[k] << 3]));


#pragma unroll
				for (int i = 1; i < 8; ++i)
				{
					fn_Add(r[i], hashes[(ind[k] << 3) + i], CV, r[i], CV);
					//asm volatile ("addc.cc.u32 %0, %0, %1;":"+r"(r[i]): "r"(hashes[(ind[k] << 3) + i]));
				}
				fn_Add(r[8], 0, CV, r[8], CV);
				//asm volatile ("addc.u32 %0, %0, 0;": "+r"(r[8]));
			}


			//--------------------hash f
			//====================================================================//
			//  Initialize context
			//====================================================================//
			//memset(ctx->b, 0, BUF_SIZE_8);
#pragma unroll
			for (int am = 0; am < BUF_SIZE_8; am++)
			{
				ctx->b[am] = 0;
			}
			B2B_IV(ctx->h);



			ctx->h[0] ^= 0x01010000 ^ NUM_SIZE_8;
			//memset(ctx->t, 0, 16);
			ctx->t[0] = 0;
			ctx->t[1] = 0;
			ctx->c = 0;


			//--------------hash 
#pragma unroll
			for (j = 0; ctx->c < BUF_SIZE_8 && j < NUM_SIZE_8; ++j)
			{
				ctx->b[ctx->c++] = ((const uint8_t *)r)[NUM_SIZE_8 - j - 1];
			}

			//====================================================================//
			//  Finalize hash
			//====================================================================//


			HOST_B2B_H_LAST(ctx, aux);


#pragma unroll
			for (j = 0; j < NUM_SIZE_8; ++j)
			{
				((uint8_t*)r)[NUM_SIZE_8 - j - 1] = (ctx->h[j >> 3] >> ((j & 7) << 3)) & 0xFF;
			}
	

			//================================================================//
			//  Dump result to global memory -- LITTLE ENDIAN
			//================================================================//
			j = ((cl_ulong*)r)[3] < ((cl_ulong global*)bound)[3] || ((cl_ulong*)r)[3] == ((cl_ulong global*)bound)[3] && (((cl_ulong*)r)[2] < ((cl_ulong global*)bound)[2] || ((cl_ulong*)r)[2] == ((cl_ulong global*)bound)[2] && (((cl_ulong*)r)[1] < ((cl_ulong global*)bound)[1] || ((cl_ulong*)r)[1] == ((cl_ulong global*)bound)[1] && ((cl_ulong*)r)[0] < ((cl_ulong global*)bound)[0]));

			pj = ((cl_ulong*)r)[3] < ((cl_ulong global*)pbound)[3] || ((cl_ulong*)r)[3] == ((cl_ulong global*)pbound)[3] && (((cl_ulong*)r)[2] < ((cl_ulong global*)pbound)[2] || ((cl_ulong*)r)[2] == ((cl_ulong global*)pbound)[2] && (((cl_ulong*)r)[1] < ((cl_ulong global*)pbound)[1] || ((cl_ulong*)r)[1] == ((cl_ulong global*)pbound)[1] && ((cl_ulong*)r)[0] < ((cl_ulong global*)pbound)[0]));

			 			
			if(pj)
			{
				cl_uint oldP =  atomic_inc(Pcount);
				if(oldP < MAX_POOL_RES)
				{
					Pvalid[oldP] = tid+1;
					#pragma unroll
					for (int i = 0; i < NUM_SIZE_32; ++i)
					{
						Pres[oldP * NUM_SIZE_32 + i] = r[i];
					}
				}

			}

			if(j)//
			{

				cl_uint oldC =  atomic_inc(vCount);

				if(oldC == 0)
				{
				
					valid[0] = tid + 1;
	#pragma unroll
					for (int i = 0; i < NUM_SIZE_32; ++i)
					{
						res[i] = r[i];
					}

				}

			}

		}
		barrier(CLK_LOCAL_MEM_FENCE);
	}
	return;
}
