#pragma OPENCL EXTENSION cl_amd_printf : enable


#include "OCLdecs.h"////problem with relative path


#define reverseBytesInt(input,output) \
do \ 
{ \
void * p = &input; \
uchar4 bytesr = ((uchar4 *)p)[0].wzyx; \
output = *((cl_uint *)&bytesr); \
} \
while (0)

const  __constant cl_ulong ivals[8] = {
	0x6A09E667F2BDC928, 0xBB67AE8584CAA73B, 0x3C6EF372FE94F82B,
	0xA54FF53A5F1D36F1, 0x510E527FADE682D1, 0x9B05688C2B3E6C1F,
	0x1F83D9ABFB41BD6B, 0x5BE0CD19137E2179 };




__kernel void BlockMiningStep1(global const cl_uint *data, const cl_ulong base,
	// precalculated hashes
	global const cl_uint* hashes,
	// intermediate Hashes
	global cl_uint *BHashes) {



	cl_uint tid;
	cl_uint r[9] = { 0 };

	cl_ulong aux[32];


	cl_uint j;
	cl_uint non[NONCE_SIZE_32];
	cl_ulong tmp;
	cl_ulong hsh;
	cl_ulong h2;
	cl_uint h3;


#pragma unroll
	for (int ii = 0; ii < 4; ii++)
	{
		tid = (NONCES_PER_ITER / 4) * ii + get_global_id(0);
		if (tid < NONCES_PER_ITER)
		{

			cl_uint CV;
			fn_Add(((cl_uint *)&base)[0], tid, 0, non[0], CV);
			non[1] = 0;
			fn_Add(((cl_uint *)&base)[1], 0, CV, non[1], CV);

			cl_ulong tmp;
			reverseBytesInt(non[1], ((cl_uint *)(&tmp))[0]);
			reverseBytesInt(non[0], ((cl_uint *)(&tmp))[1]);

			//--------------------------hash 
			B2B_IV(aux);
			B2B_IV(aux + 8);
			aux[0] = ivals[0];
			((cl_ulong *)(aux))[12] ^= 40;
			((cl_ulong *)(aux))[13] ^= 0;

			((cl_ulong *)(aux))[14] = ~((cl_ulong *)(aux))[14];

			((cl_ulong *)(aux))[16] = ((global cl_ulong *)data)[0];
			((cl_ulong *)(aux))[17] = ((global cl_ulong *)data)[1];
			((cl_ulong *)(aux))[18] = ((global cl_ulong *)data)[2];
			((cl_ulong *)(aux))[19] = ((global cl_ulong *)data)[3];
			((cl_ulong *)(aux))[20] = tmp;
			((cl_ulong *)(aux))[21] = 0;
			((cl_ulong *)(aux))[22] = 0;
			((cl_ulong *)(aux))[23] = 0;
			((cl_ulong *)(aux))[24] = 0;
			((cl_ulong *)(aux))[25] = 0;
			((cl_ulong *)(aux))[26] = 0;
			((cl_ulong *)(aux))[27] = 0;
			((cl_ulong *)(aux))[28] = 0;
			((cl_ulong *)(aux))[29] = 0;
			((cl_ulong *)(aux))[30] = 0;
			((cl_ulong *)(aux))[31] = 0;

			B2B_MIX(aux, aux + 16);

			cl_ulong hsh;
			//#pragma unroll
			//    for (j = 0; j < NUM_SIZE_32; j += 2) 
			//	{
			//	  j = 6;
				//3 = 6 >> 1;
			hsh = ivals[3];
			hsh ^= ((cl_ulong *)(aux))[3] ^ ((cl_ulong *)(aux))[11];

			//     r[6] =  ((cl_uint*)(&hsh))[0];
			//     r[7] = ((cl_uint*)(&hsh))[1];
			//   }
			reverseBytesInt(((cl_uint*)(&hsh))[1], ((cl_uint *)(&h2))[0]);
			reverseBytesInt(((cl_uint*)(&hsh))[0], ((cl_uint *)(&h2))[1]);

			//----------------------------------------------------------------------------------------
			//((uint8_t*)&h2)[0] = ((uint8_t*)r)[31];
			//((uint8_t*)&h2)[1] = ((uint8_t*)r)[30];
			//((uint8_t*)&h2)[2] = ((uint8_t*)r)[29];
			//((uint8_t*)&h2)[3] = ((uint8_t*)r)[28];
			//((uint8_t*)&h2)[4] = ((uint8_t*)r)[27];
			//((uint8_t*)&h2)[5] = ((uint8_t*)r)[26];
			//((uint8_t*)&h2)[6] = ((uint8_t*)r)[25];
			//((uint8_t*)&h2)[7] = ((uint8_t*)r)[24];

			h3 = h2 % N_LEN;
			//--------------------------read hash from lookup
			cl_uint tmpL;
#pragma unroll 8
			for (int i = 0; i < 8; ++i)
			{
				tmpL = hashes[(h3 << 3) + i];
				reverseBytesInt(tmpL, r[7 - i]);
			}
			//------------------------------------------------------

			//--------------------------hash 
			B2B_IV(aux);
			B2B_IV(aux + 8);
			aux[0] = ivals[0];
			((cl_ulong *)(aux))[12] ^= 71;//31+32+8;
			((cl_ulong *)(aux))[13] ^= 0;

			((cl_ulong *)(aux))[14] = ~((cl_ulong *)(aux))[14];

			uint8_t bT[72];
#pragma unroll 
			for (j = 0; j < 31; ++j)
				bT[j] = ((uint8_t *)r)[j + 1];
#pragma unroll 
			for (j = 31; j < 63; ++j)
				bT[j] = ((global uint8_t *)data)[j - 31];
#pragma unroll 
			for (j = 63; j < 71; ++j)
				bT[j] = ((uint8_t *)&tmp)[j - 63];
			bT[71] = 0;

			((cl_ulong *)(aux))[16] = ((cl_ulong *)bT)[0];
			((cl_ulong *)(aux))[17] = ((cl_ulong *)bT)[1];
			((cl_ulong *)(aux))[18] = ((cl_ulong *)bT)[2];
			((cl_ulong *)(aux))[19] = ((cl_ulong *)bT)[3];
			((cl_ulong *)(aux))[20] = ((cl_ulong *)bT)[4];
			((cl_ulong *)(aux))[21] = ((cl_ulong *)bT)[5];
			((cl_ulong *)(aux))[22] = ((cl_ulong *)bT)[6];
			((cl_ulong *)(aux))[23] = ((cl_ulong *)bT)[7];
			((cl_ulong *)(aux))[24] = ((cl_ulong *)bT)[8];

			((cl_ulong *)(aux))[25] = 0;
			((cl_ulong *)(aux))[26] = 0;
			((cl_ulong *)(aux))[27] = 0;
			((cl_ulong *)(aux))[28] = 0;
			((cl_ulong *)(aux))[29] = 0;
			((cl_ulong *)(aux))[30] = 0;
			((cl_ulong *)(aux))[31] = 0;

			B2B_MIX(aux, aux + 16);

#pragma unroll
			for (j = 0; j < NUM_SIZE_32; j += 2)
			{
				hsh = ivals[j >> 1];
				hsh ^= ((cl_ulong *)(aux))[j >> 1] ^ ((cl_ulong *)(aux))[8 + (j >> 1)];

				reverseBytesInt(((cl_uint*)(&hsh))[0], r[j]);
				BHashes[THREADS_PER_ITER*j + tid] = r[j];
				reverseBytesInt(((cl_uint*)(&hsh))[1], r[j + 1]);
				BHashes[THREADS_PER_ITER*(j + 1) + tid] = r[j + 1];
			}
		}  // if
	} // for
}

__kernel  void BlockMiningStep2(
	// boundary for puzzle
	global const cl_uint* bound,
	// data:  mes  
	global const cl_uint*  data,
	// nonce base
	const cl_ulong base,
	// block height
	const cl_uint height,
	// precalculated hashes
	global const cl_uint* hashes,
	// indices of valid solutions
	global cl_uint* valid,
	// solution count
	global cl_uint* vCount,
	// intermediate Hashes
	global cl_uint *BHashes
)
{

	cl_uint const tid = get_global_id(0);
	cl_uint const threadIdx = get_local_id(0);
	cl_uint const thread_id = threadIdx & 7;
	cl_uint const thrdblck_id = threadIdx;
	cl_uint const hash_id = threadIdx >> 3;

	cl_ulong aux[32] = { 0 };
	cl_uint ind[32] = { 0 };
	cl_uint r[9] = { 0 };


	uint4 v1 = { 0,0,0,0 };
	uint4 v2 = { 0,0,0,0 };
	uint4 v3 = { 0,0,0,0 };
	uint4 v4 = { 0,0,0,0 };


	__local  cl_uint shared_index[64];
	__local  cl_uint shared_data[512];

	uint8_t j = 0;

	if (tid < NONCES_PER_ITER)
	{
#pragma unroll
		for (int k = 0; k < 8; k++)
		{
			r[k] = (BHashes[k*THREADS_PER_ITER + tid]);
		}
		//================================================================//
		//  Generate indices
		//================================================================//


		((uint8_t *)r)[32] = ((uint8_t *)r)[0];
		((uint8_t *)r)[33] = ((uint8_t *)r)[1];
		((uint8_t *)r)[34] = ((uint8_t *)r)[2];
		((uint8_t *)r)[35] = ((uint8_t *)r)[3];

#pragma unroll
		for (int k = 0; k < K_LEN; k += 4)
		{
			ind[k] = r[k >> 2] & N_MASK;
			ind[k + 1] = ((r[k >> 2] << 8) | (r[(k >> 2) + 1] >> 24)) & N_MASK;
			ind[k + 2] = ((r[k >> 2] << 16) | (r[(k >> 2) + 1] >> 16)) & N_MASK;
			ind[k + 3] = ((r[k >> 2] << 24) | (r[(k >> 2) + 1] >> 8)) & N_MASK;
		}


		//================================================================//
		//  Calculate result
		//================================================================//
		shared_index[thrdblck_id] = ind[0];
		barrier(CLK_LOCAL_MEM_FENCE);

		shared_data[(hash_id << 3) + thread_id] = (hashes[(shared_index[hash_id] << 3) + thread_id]);
		shared_data[(hash_id << 3) + thread_id + 64] = (hashes[(shared_index[hash_id + 8] << 3) + thread_id]);
		shared_data[(hash_id << 3) + thread_id + 128] = (hashes[(shared_index[hash_id + 16] << 3) + thread_id]);
		shared_data[(hash_id << 3) + thread_id + 192] = (hashes[(shared_index[hash_id + 24] << 3) + thread_id]);
		shared_data[(hash_id << 3) + thread_id + 256] = (hashes[(shared_index[hash_id + 32] << 3) + thread_id]);
		shared_data[(hash_id << 3) + thread_id + 320] = (hashes[(shared_index[hash_id + 40] << 3) + thread_id]);
		shared_data[(hash_id << 3) + thread_id + 384] = (hashes[(shared_index[hash_id + 48] << 3) + thread_id]);
		shared_data[(hash_id << 3) + thread_id + 448] = (hashes[(shared_index[hash_id + 56] << 3) + thread_id]);
		barrier(CLK_LOCAL_MEM_FENCE);


		v1.x = shared_data[(thrdblck_id << 3) + 0];
		v1.y = shared_data[(thrdblck_id << 3) + 1];
		v1.z = shared_data[(thrdblck_id << 3) + 2];
		v1.w = shared_data[(thrdblck_id << 3) + 3];
		v3.x = shared_data[(thrdblck_id << 3) + 4];
		v3.y = shared_data[(thrdblck_id << 3) + 5];
		v3.z = shared_data[(thrdblck_id << 3) + 6];
		v3.w = shared_data[(thrdblck_id << 3) + 7];

		shared_index[thrdblck_id] = ind[1];
		barrier(CLK_LOCAL_MEM_FENCE);

		shared_data[(hash_id << 3) + thread_id] = (hashes[(shared_index[hash_id] << 3) + thread_id]);
		shared_data[(hash_id << 3) + thread_id + 64] = (hashes[(shared_index[hash_id + 8] << 3) + thread_id]);
		shared_data[(hash_id << 3) + thread_id + 128] = (hashes[(shared_index[hash_id + 16] << 3) + thread_id]);
		shared_data[(hash_id << 3) + thread_id + 192] = (hashes[(shared_index[hash_id + 24] << 3) + thread_id]);
		shared_data[(hash_id << 3) + thread_id + 256] = (hashes[(shared_index[hash_id + 32] << 3) + thread_id]);
		shared_data[(hash_id << 3) + thread_id + 320] = (hashes[(shared_index[hash_id + 40] << 3) + thread_id]);
		shared_data[(hash_id << 3) + thread_id + 384] = (hashes[(shared_index[hash_id + 48] << 3) + thread_id]);
		shared_data[(hash_id << 3) + thread_id + 448] = (hashes[(shared_index[hash_id + 56] << 3) + thread_id]);
		barrier(CLK_LOCAL_MEM_FENCE);

		v2.x = shared_data[(thrdblck_id << 3) + 0];
		v2.y = shared_data[(thrdblck_id << 3) + 1];
		v2.z = shared_data[(thrdblck_id << 3) + 2];
		v2.w = shared_data[(thrdblck_id << 3) + 3];
		v4.x = shared_data[(thrdblck_id << 3) + 4];
		v4.y = shared_data[(thrdblck_id << 3) + 5];
		v4.z = shared_data[(thrdblck_id << 3) + 6];
		v4.w = shared_data[(thrdblck_id << 3) + 7];



		cl_uint CV = 0;
		fn_Add(v1.x, v2.x, 0, r[0], CV);
		fn_Add(v1.y, v2.y, CV, r[1], CV);
		fn_Add(v1.z, v2.z, CV, r[2], CV);
		fn_Add(v1.w, v2.w, CV, r[3], CV);
		fn_Add(v3.x, v4.x, CV, r[4], CV);
		fn_Add(v3.y, v4.y, CV, r[5], CV);
		fn_Add(v3.z, v4.z, CV, r[6], CV);
		fn_Add(v3.w, v4.w, CV, r[7], CV);
		r[8] = 0; fn_Add(r[8], 0, CV, r[8], CV);


		// remaining additions
#pragma unroll
		for (int k = 2; k < K_LEN; ++k)
		{
			shared_index[thrdblck_id] = ind[k];
			barrier(CLK_LOCAL_MEM_FENCE);

			shared_data[(hash_id << 3) + thread_id] = (hashes[(shared_index[hash_id] << 3) + thread_id]);
			shared_data[(hash_id << 3) + thread_id + 64] = (hashes[(shared_index[hash_id + 8] << 3) + thread_id]);
			shared_data[(hash_id << 3) + thread_id + 128] = (hashes[(shared_index[hash_id + 16] << 3) + thread_id]);
			shared_data[(hash_id << 3) + thread_id + 192] = (hashes[(shared_index[hash_id + 24] << 3) + thread_id]);
			shared_data[(hash_id << 3) + thread_id + 256] = (hashes[(shared_index[hash_id + 32] << 3) + thread_id]);
			shared_data[(hash_id << 3) + thread_id + 320] = (hashes[(shared_index[hash_id + 40] << 3) + thread_id]);
			shared_data[(hash_id << 3) + thread_id + 384] = (hashes[(shared_index[hash_id + 48] << 3) + thread_id]);
			shared_data[(hash_id << 3) + thread_id + 448] = (hashes[(shared_index[hash_id + 56] << 3) + thread_id]);
			barrier(CLK_LOCAL_MEM_FENCE);

			v1.x = shared_data[(thrdblck_id << 3) + 0];
			v1.y = shared_data[(thrdblck_id << 3) + 1];
			v1.z = shared_data[(thrdblck_id << 3) + 2];
			v1.w = shared_data[(thrdblck_id << 3) + 3];
			v2.x = shared_data[(thrdblck_id << 3) + 4];
			v2.y = shared_data[(thrdblck_id << 3) + 5];
			v2.z = shared_data[(thrdblck_id << 3) + 6];
			v2.w = shared_data[(thrdblck_id << 3) + 7];



			fn_Add(r[0], v1.x, CV, r[0], CV);
			fn_Add(r[1], v1.y, CV, r[1], CV);
			fn_Add(r[2], v1.z, CV, r[2], CV);
			fn_Add(r[3], v1.w, CV, r[3], CV);
			fn_Add(r[4], v2.x, CV, r[4], CV);
			fn_Add(r[5], v2.y, CV, r[5], CV);
			fn_Add(r[6], v2.z, CV, r[6], CV);
			fn_Add(r[7], v2.w, CV, r[7], CV);
			fn_Add(r[8], 0, CV, r[8], CV);
		}


		//--------------------hash(f)--------------------
		//====================================================================//
		//  Initialize context
		//====================================================================//
		B2B_IV(aux);
		B2B_IV(aux + 8);
		aux[0] = ivals[0];
		((cl_ulong *)(aux))[12] ^= 32;
		((cl_ulong *)(aux))[13] ^= 0;

		((cl_ulong *)(aux))[14] = ~((cl_ulong *)(aux))[14];

		uint8_t *bb = (uint8_t *)(&(((cl_ulong *)(aux))[16]));
		for (j = 0; j < NUM_SIZE_8; ++j)
		{
			bb[j] = ((const uint8_t *)r)[NUM_SIZE_8 - j - 1];
		}

		((cl_ulong *)(aux))[20] = 0;
		((cl_ulong *)(aux))[21] = 0;
		((cl_ulong *)(aux))[22] = 0;
		((cl_ulong *)(aux))[23] = 0;
		((cl_ulong *)(aux))[24] = 0;
		((cl_ulong *)(aux))[25] = 0;
		((cl_ulong *)(aux))[26] = 0;
		((cl_ulong *)(aux))[27] = 0;
		((cl_ulong *)(aux))[28] = 0;
		((cl_ulong *)(aux))[29] = 0;
		((cl_ulong *)(aux))[30] = 0;
		((cl_ulong *)(aux))[31] = 0;

		B2B_MIX(aux, aux + 16);

		cl_ulong hsh;
#pragma unroll
		for (j = 0; j < NUM_SIZE_32; j += 2)
		{
			hsh = ivals[j >> 1];
			hsh ^= ((cl_ulong *)(aux))[j >> 1] ^ ((cl_ulong *)(aux))[8 + (j >> 1)];
			reverseBytesInt(((cl_uint*)&hsh)[0], r[7 - j]);
			reverseBytesInt(((cl_uint*)&hsh)[1], r[7 - j - 1]);

		}
		//================================================================//
		//  Dump result to global memory -- LITTLE ENDIAN
		//================================================================//
		j = ((cl_ulong*)r)[3] < ((cl_ulong global*)bound)[3] || ((cl_ulong*)r)[3] == ((cl_ulong global*)bound)[3] && (((cl_ulong*)r)[2] < ((cl_ulong global*)bound)[2] || ((cl_ulong*)r)[2] == ((cl_ulong global*)bound)[2] && (((cl_ulong*)r)[1] < ((cl_ulong global*)bound)[1] || ((cl_ulong*)r)[1] == ((cl_ulong global*)bound)[1] && ((cl_ulong*)r)[0] < ((cl_ulong global*)bound)[0]));

		if (j)//
		{
			cl_uint oldC = atomic_inc(vCount);

			if (oldC < MAX_POOL_RES)
			{
				valid[oldC] = tid + 1;
			}
		}
	} // if
}
