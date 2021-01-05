
#include "OCLdecs.h"////problem with relative path

__kernel void FinalPrehashMultSecKey(
    // data: pk || mes || w || padding || x || sk
    global const cl_uint * data,
    // hashes
    global cl_uint * hashes
)
{


   cl_uint j;
    cl_uint tid =get_local_id(0);

    // shared memory
    __local  cl_uint sdata[ROUND_NUM_SIZE_32];

	#pragma unroll
    for (int i = 0; i < NUM_SIZE_32_BLOCK; ++i)
    {
        sdata[NUM_SIZE_32_BLOCK * tid + i]
            = data[
                NUM_SIZE_32_BLOCK * tid + NUM_SIZE_32 + COUPLED_PK_SIZE_32 + i
            ];
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    tid = get_global_id(0);
	
	if (tid < N_LEN)
	{
		// one-time secret key
		// NUM_SIZE_8 bytes
        __local cl_uint * x = sdata;
        // local memory
        cl_uint h[NUM_SIZE_32];

#pragma unroll
        for (int j = 0; j < NUM_SIZE_8; ++j)
        {
             ((uint8_t *)h)[j]
                 = ((__global uint8_t *)hashes)[(tid + 1) * NUM_SIZE_8 - j - 1]; 
        }

        __private cl_uint r[NUM_SIZE_32 << 1];


		//====================================================================//
		//  r[0, ..., 7, 8] = h[0] * x
		//====================================================================//
		// initialize r[0, ..., 7]
//#pragma unroll



		cl_ulong tmp;

		for (int j = 0; j < 8; j += 2)
		{
			tmp = 0;
			/*r[j]*/tmp = ((cl_ulong)h[0] * (cl_ulong)x[j]);
			  r[j] = tmp;

			tmp = tmp >> 32;
			// asm volatile (
			  //   "mul.lo.u32 %0, %1, %2;": "=r"(r[j]): "r"(h[0]), "r"(x[j])
			 //);
			//tmp = (cl_ulong)h[0] * (cl_ulong)x[j]/* + (cl_ulong)tmp*/;
		
			r[j + 1] = tmp;
			//asm volatile (
			  //  "mul.hi.u32 %0, %1, %2;": "=r"(r[j + 1]): "r"(h[0]), "r"(x[j])
			//);
		}


		//====================================================================//
		cl_uint cv;
		fn_MadLo(h[0], x[1],0, r[1], cv);
		//asm volatile (
		  //  "mad.lo.cc.u32 %0, %1, %2, %0;": "+r"(r[1]): "r"(h[0]), "r"(x[1])
		//);

		 fn_MadHi(h[0],x[1],cv,r[2], cv);
		// asm volatile (
			// "madc.hi.cc.u32 %0, %1, %2, %0;": "+r"(r[2]): "r"(h[0]), "r"(x[1])
		 //);

		 #pragma unroll
		for (int j = 3; j < 6; j += 2)
		{

			fn_MadLo(h[0], x[j],cv, r[j], cv);
			//asm volatile ("madc.lo.cc.u32 %0, %1, %2, %0;":"+r"(r[j]): "r"(h[0]), "r"(x[j]));

			fn_MadHi(h[0], x[j], cv, r[j+1], cv);
			//asm volatile ("madc.hi.cc.u32 %0, %1, %2, %0;":"+r"(r[j + 1]): "r"(h[0]), "r"(x[j]));
		}




				//????x[7]
		 fn_MadLo(h[0], x[7], cv, r[7], cv);
		//asm volatile ("madc.lo.cc.u32 %0, %1, %2, %0;": "+r"(r[7]) : "r"(h[0]), "r"(x[7]));

		// initialize r[8]

		r[8] = 0;
		fn_MadHi(h[0],x[7], cv, r[8], cv);
		//asm volatile ("madc.hi.u32 %0, %1, %2, 0;": "=r"(r[8]) : "r"(h[0]), "r"(x[7]));


//		//====================================================================//
//		//  r[i, ..., i + 7, i + 8] += h[i] * x
//		//====================================================================//
//#pragma unroll

		cv = 0;

		for (int i = 1; i < NUM_SIZE_32; ++i)
		{
			cv = 0;
			fn_MadLo(h[i], x[0], cv, r[i], cv);
			//asm volatile ("mad.lo.cc.u32 %0, %1, %2, %0;":"+r"(r[i]) : "r"(h[i]), "r"(x[0]));

			fn_MadHi(h[i], x[0], cv, r[i+1], cv);
			//asm volatile ("madc.hi.cc.u32 %0, %1, %2, %0;":"+r"(r[i + 1]) : "r"(h[i]), "r"(x[0]));

//#pragma unroll

			for (int j = 2; j < 8; j += 2)
			{
				fn_MadLo(h[i], x[j], cv, r[i+j], cv);
				//asm volatile ("madc.lo.cc.u32 %0, %1, %2, %0;":"+r"(r[i + j]) : "r"(h[i]), "r"(x[j]));

				fn_MadHi(h[i], x[j], cv, r[i + j+1], cv);
				//asm volatile ("madc.hi.cc.u32 %0, %1, %2, %0;":"+r"(r[i + j + 1]) : "r"(h[i]), "r"(x[j]));
			}

			// initialize r[i + 8]

			r[i + 8] = cv;
			//asm volatile ("addc.u32 %0, 0, 0;": "=r"(r[i + 8]));

			//====================================================================//


			cv = 0;

			 fn_MadLo(h[i], x[1], cv, r[i + 1], cv);//asm volatile ("mad.lo.cc.u32 %0, %1, %2, %0;":"+r"(r[i + 1]) : "r"(h[i]), "r"(x[1]));
			
			fn_MadHi(h[i], x[1], cv, r[i +2], cv);
			//asm volatile ("madc.hi.cc.u32 %0, %1, %2, %0;":"+r"(r[i + 2]) : "r"(h[i]), "r"(x[1]));

//#pragma unroll

			for (int j = 3; j < 6; j += 2)
			{

				fn_MadLo(h[i], x[j], cv,r[i + j], cv);
				//asm volatile ("madc.lo.cc.u32 %0, %1, %2, %0;":"+r"(r[i + j]) : "r"(h[i]), "r"(x[j]));
				
				fn_MadHi(h[i], x[j], cv, r[i + j+1], cv);
				//asm volatile ("madc.hi.cc.u32 %0, %1, %2, %0;":"+r"(r[i + j + 1]) : "r"(h[i]), "r"(x[j]));
			}


			 fn_MadLo(h[i], x[7], cv, r[i + 7], cv);
			//asm volatile ("madc.lo.cc.u32 %0, %1, %2, %0;":"+r"(r[i + 7]) : "r"(h[i]), "r"(x[7]));

			fn_MadHi(h[i], x[7], cv, r[i + 8], cv);
			//asm volatile ("madc.hi.u32 %0, %1, %2, %0;":"+r"(r[i + 8]) : "r"(h[i]), "r"(x[7]));
		}


		//====================================================================//
		//  Mod Q
		//====================================================================//

		cl_uint d[2];
		cl_uint med[6];
		cl_uint carry;
		med[4] = 0;

#pragma unroll

		for (int i = (NUM_SIZE_32 - 1) << 1; i >= NUM_SIZE_32; i -= 2)
		{
			int idx = i; //>> 1;
			d[0] = r[idx];
			d[1] = r[idx+1];
			//i >>= 1;
			//*((cl_ulong *)d) = ((cl_ulong *)r)[i >> 1];

			
			//====================================================================//
			//  med[0, ..., 5] = d * Q
			//====================================================================//

			med[0] = d[0] * (cl_uint)q0_s;
			//asm volatile ("mul.lo.u32 %0, %1, " q0_s ";": "=r"(med[0]) : "r"(d[0]));

			med[1] = ((cl_ulong)d[0] * (cl_ulong)q0_s)>>32;
			//asm volatile ("mul.hi.u32 %0, %1, " q0_s ";": "=r"(med[1]) : "r"(d[0]));

			med[2] = d[0] * (cl_uint)q2_s;
			//asm volatile ("mul.lo.u32 %0, %1, " q2_s ";": "=r"(med[2]) : "r"(d[0]));

			med[3] = ((cl_ulong)d[0] * (cl_ulong)q2_s)>>32;
			//asm volatile ("mul.hi.u32 %0, %1, " q2_s ";": "=r"(med[3]) : "r"(d[0]));
			
			cv = 0;

			fn_MadLo(d[0], (cl_uint)q1_s, cv, med[1], cv);
			//asm volatile ("mad.lo.cc.u32 %0, %1, " q1_s ", %0;": "+r"(med[1]) : "r"(d[0]));

			fn_MadHi(d[0], (cl_uint)q1_s, cv, med[2], cv);
			//asm volatile ("madc.hi.cc.u32 %0, %1, " q1_s ", %0;": "+r"(med[2]) : "r"(d[0]));

			fn_MadLo(d[0], (cl_uint)q3_s, cv, med[3], cv);
			//asm volatile ("madc.lo.cc.u32 %0, %1, " q3_s ", %0;": "+r"(med[3]) : "r"(d[0]));
			med[4] = 0;
			fn_MadHi(d[0], (cl_uint)q3_s, cv, med[4], cv);
			//asm volatile ("madc.hi.u32 %0, %1, " q3_s ", 0;": "=r"(med[4]) : "r"(d[0]));


			//====================================================================//


			cv = 0;

			fn_MadLo(d[1], (cl_uint)q0_s, cv, med[1], cv);
			//asm volatile ("mad.lo.cc.u32 %0, %1, " q0_s ", %0;": "+r"(med[1]) : "r"(d[1]));

			fn_MadHi(d[1], (cl_uint)q0_s, cv, med[2], cv);
			//asm volatile ("madc.hi.cc.u32 %0, %1, " q0_s ", %0;": "+r"(med[2]) : "r"(d[1]));

			fn_MadLo(d[1], (cl_uint)q2_s, cv, med[3], cv);
			//asm volatile ("madc.lo.cc.u32 %0, %1, " q2_s ", %0;": "+r"(med[3]) : "r"(d[1]));

			fn_MadHi(d[1], (cl_uint)q2_s, cv, med[4], cv);
			//asm volatile ("madc.hi.cc.u32 %0, %1," q2_s", %0;": "+r"(med[4]) : "r"(d[1]));

			med[5] = cv;
			//asm volatile ("addc.u32 %0, 0, 0;": "=r"(med[5]));

			cv = 0;

			fn_MadLo(d[1], (cl_uint)q1_s, cv, med[2], cv);
			//asm volatile ("mad.lo.cc.u32 %0, %1, " q1_s ", %0;": "+r"(med[2]) : "r"(d[1]));

			fn_MadHi(d[1], (cl_uint)q1_s, cv, med[3], cv);
			//asm volatile ("madc.hi.cc.u32 %0, %1, " q1_s ", %0;": "+r"(med[3]) : "r"(d[1]));

			fn_MadLo(d[1], (cl_uint)q3_s, cv, med[4], cv);
			//asm volatile ("madc.lo.cc.u32 %0, %1, " q3_s ", %0;": "+r"(med[4]) : "r"(d[1]));

			fn_MadHi(d[1], (cl_uint)q3_s, cv, med[5], cv);
			//asm volatile ("madc.hi.u32 %0, %1, " q3_s ", %0;": "+r"(med[5]) : "r"(d[1]));


			//====================================================================//
			//  x[i/2 - 2, i/2 - 3, i/2 - 4] -= d * Q
			//====================================================================//

			fn_Sub(r[i - 8], med[0], 0, r[i - 8], cv);

			

			//asm volatile ("sub.cc.u32 %0, %0, %1;": "+r"(r[i - 8]) : "r"(med[0]));

//#pragma unroll

			cl_uint oldcv;
			for (int j = 1; j < 6; ++j)
			{
				fn_Sub(r[i+j - 8], med[j], cv, r[i+j - 8], cv);
				//asm volatile ("subc.cc.u32 %0, %0, %1;": "+r"(r[i + j - 8]) : "r"(med[j]));
			}

	
				fn_Sub(r[i - 2], 0, cv, r[i - 2], cv);
			//asm volatile ("subc.cc.u32 %0, %0, 0;": "+r"(r[i - 2]));

			 fn_Sub(r[i - 1], 0, cv, r[i - 1], cv);
			//asm volatile ("subc.u32 %0, %0, 0;": "+r"(r[i - 1]));

			//====================================================================//
			//  x[i/2 - 1, i/2 - 2] += 2 * d
			//====================================================================//

			carry = d[1] >> 31;
			d[1] = (d[1] << 1) | (d[0] >> 31);
			d[0] <<= 1;


					 

			cl_uint a1 = r[i - 4];
			cl_uint a2 = d[0];
			cl_uint a3 = 0;
			cl_uint a4 = cv;

			//cv = fn_Add(r[i - 4], d[0], 0, r[i - 4]);
			fn_Add(r[i - 4], d[0], 0, r[i - 4], cv);
			//asm volatile ("add.cc.u32 %0, %0, %1;": "+r"(r[i - 4]) : "r"(d[0]));

			fn_Add(r[i - 3], d[1], cv, r[i - 3], cv);
			//asm volatile ("addc.cc.u32 %0, %0, %1;": "+r"(r[i - 3]) : "r"(d[1]));

			fn_Add(r[i - 2], carry, cv, r[i - 2], cv);
			//asm volatile ("addc.cc.u32 %0, %0, %1;": "+r"(r[i - 2]) : "r"(carry));

			fn_Add(r[i - 1],0, cv, r[i - 1], cv);
			//asm volatile ("addc.u32 %0, %0, 0;": "+r"(r[i - 1]));



		}


		//====================================================================//
		//  Last 256 bit correction
		//====================================================================//

		 fn_Sub(r[0], (cl_uint)q0_s, 0, r[0], cv);
		//asm volatile ("sub.cc.u32 %0, %0, " q0_s ";": "+r"(r[0]));
		
		fn_Sub(r[1], (cl_uint)q1_s, cv, r[1], cv);
		//asm volatile ("subc.cc.u32 %0, %0, " q1_s ";": "+r"(r[1]));
		
		fn_Sub(r[2], (cl_uint)q2_s, cv, r[2], cv);
		//asm volatile ("subc.cc.u32 %0, %0, " q2_s ";": "+r"(r[2]));

		fn_Sub(r[3], (cl_uint)q3_s, cv, r[3], cv);
		//asm volatile ("subc.cc.u32 %0, %0, " q3_s ";": "+r"(r[3]));

		fn_Sub(r[4], (cl_uint)q4_s, cv, r[4], cv);
		//asm volatile ("subc.cc.u32 %0, %0, " q4_s ";": "+r"(r[4]));


#pragma unroll

		for (int j = 5; j < 8; ++j)
		{
			  fn_Sub(r[j], (cl_uint)qhi_s, cv, r[j], cv);
			//asm volatile ("subc.cc.u32 %0, %0, " qhi_s ";": "+r"(r[j]));
		}


		//====================================================================//

		  fn_Sub(0,0, cv, carry, cv);
		//asm volatile ("subc.u32 %0, 0, 0;": "=r"(carry));


		carry = 0 - carry;

		//====================================================================//

					 //return ;

		  fn_MadLo(carry, (cl_uint)q0_s, 0, r[0], cv);
		//asm volatile ("mad.lo.cc.u32 %0, %1, " q0_s ", %0;": "+r"(r[0]) : "r"(carry));
		
		  fn_MadLo(carry, (cl_uint)q1_s, cv, r[1], cv);
		//asm volatile ("madc.lo.cc.u32 %0, %1, " q1_s ", %0;": "+r"(r[1]) : "r"(carry));
		
		  fn_MadLo(carry, (cl_uint)q2_s, cv, r[2], cv);
		//asm volatile ("madc.lo.cc.u32 %0, %1, " q2_s ", %0;": "+r"(r[2]) : "r"(carry));

		  fn_MadLo(carry, (cl_uint)q3_s, cv, r[3], cv);
		//asm volatile ("madc.lo.cc.u32 %0, %1, " q3_s ", %0;": "+r"(r[3]) : "r"(carry));
		
		  fn_MadLo(carry, (cl_uint)q4_s, cv, r[4], cv);
		//asm volatile ("madc.lo.cc.u32 %0, %1, " q4_s ", %0;": "+r"(r[4]) : "r"(carry));

#pragma unroll
		for (int j = 5; j < 7; ++j)
		{
			  fn_MadLo(carry, (cl_uint)qhi_s, cv, r[j], cv);
			//asm volatile ("madc.lo.cc.u32 %0, %1, " qhi_s ", %0;": "+r"(r[j]) : "r"(carry));
		}

		  fn_MadLo(carry, (cl_uint)qhi_s, cv, r[7], cv);
		//asm volatile ("madc.lo.u32 %0, %1, " qhi_s ", %0;": "+r"(r[7]) : "r"(carry));

		//====================================================================//
		//  Dump result to global memory -- LITTLE ENDIAN
		//====================================================================//
#pragma unroll
		for (int i = 0; i < NUM_SIZE_32; ++i)
		{
			hashes[tid * NUM_SIZE_32 + i] = r[i];
		}

	}

	return;
}





////////////////////////////////////////////////////////////////////////////////
//  First iteration of hashes precalculation
////////////////////////////////////////////////////////////////////////////////
__kernel void InitPrehash(
    // data: h 
    const cl_uint  h,
    // hashes
    global cl_uint * hashes,
    // indices of invalid range hashes
    global cl_uint * invalid
)
{
    cl_uint tid =get_local_id(0);

    // shared memory
    cl_uint height = h;



    tid = get_global_id(0);

    if (tid < N_LEN)
    {

        cl_uint j;



        // local memory
        // 472 bytes
        cl_uint ldata[118];

        // 32 * 64 bits = 256 bytes 
        cl_ulong * aux = (cl_ulong *)ldata;
        // (212 + 4) bytes 
        ctx_t * ctx = (ctx_t *)(ldata + 64);


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
        //  Hash tid
        //====================================================================//
#pragma unroll
        for (j = 0; ctx->c < BUF_SIZE_8 && j < INDEX_SIZE_8; ++j)
        {
            ctx->b[ctx->c++] = ((const uint8_t *)&tid)[INDEX_SIZE_8 - j - 1];
        }

        //====================================================================//
        //  Hash height
        //====================================================================//
#pragma unroll
        for (j = 0; ctx->c < BUF_SIZE_8 && j < HEIGHT_SIZE ; ++j)
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

            for ( ; ctx->c < BUF_SIZE_8 && j < CONST_MES_SIZE_8; ++j)
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
            ((uint8_t *)ldata)[NUM_SIZE_8 - j - 1]
                = (ctx->h[j >> 3] >> ((j & 7) << 3)) & 0xFF;
        }


        //====================================================================//
        //  Dump result to global memory -- BIG ENDIAN
        //====================================================================//
#pragma unroll
        for (int i = 0; i < NUM_SIZE_8; ++i)
        {
            ((uint8_t __global *)hashes)[(tid + 1) * NUM_SIZE_8 - i - 1]
                = ((uint8_t *)ldata)[i];
        }


        j = ((cl_ulong *)ldata)[3] < Q3
            || ((cl_ulong *)ldata)[3] == Q3 && (
                ((cl_ulong *)ldata)[2] < Q2
                || ((cl_ulong *)ldata)[2] == Q2 && (
                    ((cl_ulong *)ldata)[1] < Q1
                    || ((cl_ulong *)ldata)[1] == Q1
                    && ((cl_ulong *)ldata)[0] < Q0
                )
            );


        //invalid[tid] = (1 - j) * (tid + 1);

					// drop
					
		//	for(int i=NUM_SIZE_8-1;i>0;i--)
		//	{
		//		((uint8_t *)ldata)[i] = ((uint8_t *)ldata)[i-1];
		//	}
		//	((uint8_t *)ldata)[0] = 0;
		

			
            #pragma unroll
            for (int i = 0; i < NUM_SIZE_8-1; ++i) 
            {
                    //((uint8_t global*)hashes)[(tid + 1) * NUM_SIZE_8 - i -1] = ((uint8_t *)ldata)[i];
					((uint8_t global*)hashes)[tid * NUM_SIZE_8 +i ] = ((uint8_t *)ldata)[i];
            }
			((uint8_t global*)hashes)[tid * NUM_SIZE_8 +31 ] = 0; 

		

	}

    return;
}
///////////////////////////////////////////////////////////////////////////////
//  Uncompleted first iteration of hashes precalculation
////////////////////////////////////////////////////////////////////////////////
__kernel void UncompleteInitPrehash(
	// data: pk || mes || w || padding || x || sk
	global const cl_uint  * data,
	// hashes
	global uctx_t * uctxs1,
	global uctx_t * uctxs2,
	cl_ulong  memsize1,
	cl_ulong  memsize2,
	cl_uint memCount

)
{
	cl_uint tid = get_local_id(0);

	// shared memory
	__local  cl_uint sdata[ROUND_PK_SIZE_32];

#pragma unroll
	for (int i = 0; i < PK_SIZE_32_BLOCK; ++i)
	{
		sdata[PK_SIZE_32_BLOCK * tid + i] = data[PK_SIZE_32_BLOCK * tid + i];
	}

	barrier(CLK_LOCAL_MEM_FENCE);

	tid = get_global_id(0);

	if (tid  < N_LEN)
	{
		cl_uint j;

		// public key
		// PK_SIZE_8 bytes
		__local cl_uint * pk = sdata;

		// local memory
		// 472 bytes
		cl_uint ldata[118];

		// 32 * 64 bits = 256 bytes 
		cl_ulong * aux = (cl_ulong *)ldata;
		// (212 + 4) bytes 
		ctx_t * ctx = (ctx_t *)(ldata + 64);

		//====================================================================//
		//  Initialize context
		//====================================================================//
		for (int am = 0; am < BUF_SIZE_8; am++)
		{
			ctx->b[am] = 0;
		}
		B2B_IV(ctx->h);
		ctx->h[0] ^= 0x01010000 ^ NUM_SIZE_8;
		ctx->t[0] = 0;
		ctx->t[1] = 0;
		ctx->c = 0;

		//====================================================================//
		//  Hash tid
		//====================================================================//
#pragma unroll
		for (j = 0; ctx->c < BUF_SIZE_8 && j < INDEX_SIZE_8; ++j)
		{
			ctx->b[ctx->c++] = ((const uint8_t *)&tid)[INDEX_SIZE_8 - j - 1];
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
		//  Hash public key
		//====================================================================//
#pragma unroll
		for (j = 0; ctx->c < BUF_SIZE_8 && j < PK_SIZE_8; ++j)
		{
			ctx->b[ctx->c++] = ((const uint8_t __local *)pk)[j];
		}

#pragma unroll
		for (; j < PK_SIZE_8; )
		{
			HOST_B2B_H(ctx, aux);

#pragma unroll
			for (; ctx->c < BUF_SIZE_8 && j < PK_SIZE_8; ++j)
			{
				ctx->b[ctx->c++] = ((const uint8_t __local *)pk)[j];
			}
		}

		//====================================================================//
		//  Dump result to global memory
		//====================================================================//
		if (tid < memsize1)
		{
#pragma unroll
			for (int i = 0; i < 16; ++i)
			{
				((cl_uint global *)uctxs1[tid].h)[i] = ((cl_uint *)ctx->h)[i];
			}

#pragma unroll
			for (int i = 0; i < 4; ++i)
			{
				((cl_uint global *)uctxs1[tid].t)[i] = ((cl_uint *)ctx->t)[i];
			}
		}
		else
		{
#pragma unroll
			for (int i = 0; i < 16; ++i)
			{
				((cl_uint global *)uctxs2[tid- memsize1].h)[i] = ((cl_uint *)ctx->h)[i];
				
			}

#pragma unroll
			for (int i = 0; i < 4; ++i)
			{
				((cl_uint global *)uctxs2[tid- memsize1].t)[i] = ((cl_uint *)ctx->t)[i];
			}
		}
	}

	return;
}

__kernel  void CompleteInitPrehash(
	// data: pk || mes || w || padding || x || sk
	global const cl_uint * data,
	// unfinalized hashes contexts
	global const uctx_t * uctxs1,
	global const uctx_t * uctxs2,
	cl_ulong  memsize1,
	cl_ulong  memsize2,
	cl_uint memCount,
	// hashes
	global cl_uint * hashes,
	// indices of invalid range hashes
	global cl_uint * invalid
)
{
	cl_uint j;
	cl_uint tid = get_local_id(0);

	// shared memory
	__local  cl_uint sdata[ROUND_NP_SIZE_32];

#pragma unroll
	for (int i = 0; i < NP_SIZE_32_BLOCK; ++i)
	{
		sdata[NP_SIZE_32_BLOCK * tid + i]
			= data[NP_SIZE_32_BLOCK * tid + NUM_SIZE_32 + i];
	}

	barrier(CLK_LOCAL_MEM_FENCE);

	tid = get_global_id(0);


	if (tid < N_LEN)
	{
		// mes || w
		// PK_SIZE_8 + NUM_SIZE_8 bytes
		__local uint8_t * rem = (__local uint8_t *)sdata + PK_SIZE_8 - NUM_SIZE_8;
		/*uint8_t rem[ROUND_NP_SIZE_32*4];
		for (int rr = 0; rr<ROUND_NP_SIZE_32*4; rr++)
		{
			uint8_t tmpData = ((uint8_t __local *)sdata + PK_SIZE_8 - NUM_SIZE_8)[rr];
			rem[rr] = tmpData ;
		}
		*/
		/*
		OR
		for (int rr = 0; rr<ROUND_NP_SIZE_32; rr++)
		{
			rem[rr] = *((uint8_t *)sdata + PK_SIZE_8 - NUM_SIZE_8 )+rr;
		}

		*/

		// local memory
		// 472 bytes
		cl_uint ldata[118];

		// 32 * 64 bits = 256 bytes 
		cl_ulong * aux = (cl_ulong *)ldata;
		// (212 + 4) bytes 
		ctx_t * ctx = (ctx_t *)(ldata + 64);


		//====================================================================//
		//  Initialize context
		//====================================================================//

		ctx->c = CONTINUE_POS;

#pragma unroll
		for (
			j = CONST_MES_SIZE_8 - BUF_SIZE_8 + PK_SIZE_8 - 1;
			ctx->c < BUF_SIZE_8 && j < CONST_MES_SIZE_8;
			++j
			)
		{
			ctx->b[ctx->c++]
				= (
					!((7 - (j & 7)) >> 1)
					* ((j >> 3) >> (((~(j & 7)) & 1) << 3))
					) & 0xFF;
		}

		ctx->c = 0;


#pragma unroll
		for (; j < CONST_MES_SIZE_8; ++j)
		{
			ctx->b[ctx->c++]
				= (
					!((7 - (j & 7)) >> 1)
					* ((j >> 3) >> (((~(j & 7)) & 1) << 3))
					) & 0xFF;
		}


#pragma unroll
		for (int i = 0; i < NUM_SIZE_32; ++i)
		{
			((cl_uint *)(ctx->b + ctx->c))[i] = ((cl_uint __global*)data)[i];
		}

		// last byte of public key
		ctx->b[ctx->c + NUM_SIZE_8] = ((uint8_t __global*)data)[NUM_SIZE_8];


		ctx->c += PK_SIZE_8;
		if (tid < memsize1)
		{
#pragma unroll
			for (int i = 0; i < 16; ++i)
			{
				((cl_uint *)ctx->h)[i] = ((cl_uint __global*)uctxs1[tid].h)[i];
			}

#pragma unroll
			for (int i = 0; i < 4; ++i)
			{
				((cl_uint *)ctx->t)[i] = ((cl_uint __global*)uctxs1[tid].t)[i];
				
			}
		}
		else
		{
#pragma unroll
			for (int i = 0; i < 16; ++i)
			{
				((cl_uint *)ctx->h)[i] = ((cl_uint __global*)uctxs2[tid- memsize1].h)[i];
			}

#pragma unroll
			for (int i = 0; i < 4; ++i)
			{
				((cl_uint *)ctx->t)[i] = ((cl_uint __global*)uctxs2[tid- memsize1].t)[i];
			}

		}



		//====================================================================//
		//  Hash public key, message & one-time public key
		//====================================================================//
#pragma unroll
		for (j = 0; ctx->c < BUF_SIZE_8 && j < PK_SIZE_8 + NUM_SIZE_8; ++j)
		{
			ctx->b[ctx->c++] = rem[j];
		}


#pragma unroll
		for (; j < PK_SIZE_8 + NUM_SIZE_8; )
		{
			HOST_B2B_H(ctx, aux);

#pragma unroll
			for (; ctx->c < BUF_SIZE_8 && j < PK_SIZE_8 + NUM_SIZE_8; ++j)
			{
				ctx->b[ctx->c++] = rem[j];
			}
		}


		//====================================================================//
		//  Finalize hash
		//====================================================================//
		HOST_B2B_H_LAST(ctx, aux);


#pragma unroll
		for (j = 0; j < NUM_SIZE_8; ++j)
		{
			((uint8_t *)ldata)[NUM_SIZE_8 - j - 1]
				= (ctx->h[j >> 3] >> ((j & 7) << 3)) & 0xFF;
		}


		//====================================================================//    
		//  Dump result to global memory -- BIG ENDIAN
		//====================================================================//
		j = ((cl_ulong *)ldata)[3] < Q3
			|| ((cl_ulong *)ldata)[3] == Q3 && (
			((cl_ulong *)ldata)[2] < Q2
				|| ((cl_ulong *)ldata)[2] == Q2 && (
				((cl_ulong *)ldata)[1] < Q1
					|| ((cl_ulong *)ldata)[1] == Q1
					&& ((cl_ulong *)ldata)[0] < Q0
					)
				);

//#pragma unroll
		for (int i = 0; i < NUM_SIZE_8; ++i)
		{
			((uint8_t __global *)hashes)[tid * NUM_SIZE_8 + NUM_SIZE_8 - i - 1]
				= ((uint8_t *)ldata)[i];
		}

		// rehash out of bounds hash
		while (!j)
		{
			//aminm memset(ctx->b, 0, BUF_SIZE_8);
#pragma unroll
			for (int am = 0; am < BUF_SIZE_8; am++)
			{
				ctx->b[am] = 0;
			}
			B2B_IV(ctx->h);

			ctx->h[0] ^= 0x01010000 ^ NUM_SIZE_8;

			//aminm memset(ctx->t, 0, 16);
			ctx->t[0] = 0;
			ctx->t[1] = 0;

			ctx->c = 0;


			//====================================================================//
			//  Hash previous hash
			//====================================================================//
#pragma unroll
			for (j = 0; ctx->c < BUF_SIZE_8 && j < NUM_SIZE_8; ++j)
			{
				ctx->b[ctx->c++]
					= ((const uint8_t  __global *)(hashes + tid * NUM_SIZE_32))[j];
			}

#pragma unroll
			for (; j < NUM_SIZE_8; )
			{
				HOST_B2B_H(ctx, aux);

#pragma unroll
				for (; ctx->c < BUF_SIZE_8 && j < NUM_SIZE_8; ++j)
				{
					ctx->b[ctx->c++]
						= ((const uint8_t __global *)(hashes + tid * NUM_SIZE_32))[j];
				}
			}


			//====================================================================//
			//  Finalize hash
			//====================================================================//
			HOST_B2B_H_LAST(ctx, aux);

#pragma unroll
			for (j = 0; j < NUM_SIZE_8; ++j)
			{
				((uint8_t *)ldata)[NUM_SIZE_8 - j - 1]
					= (ctx->h[j >> 3] >> ((j & 7) << 3)) & 0xFF;
			}


			//====================================================================//
			//  Dump result to global memory -- BIG ENDIAN
			//====================================================================//
			j = ((cl_ulong *)ldata)[3] < Q3
				|| ((cl_ulong *)ldata)[3] == Q3 && (
				((cl_ulong *)ldata)[2] < Q2
					|| ((cl_ulong *)ldata)[2] == Q2 && (
					((cl_ulong *)ldata)[1] < Q1
						|| ((cl_ulong *)ldata)[1] == Q1
						&& ((cl_ulong *)ldata)[0] < Q0
						)
					);


#pragma unroll
			for (int i = 0; i < NUM_SIZE_8; ++i)
			{
				((uint8_t __global *)hashes)[(tid + 1) * NUM_SIZE_8 - i - 1]
					= ((uint8_t *)ldata)[i];

				
			}

	

		}


	
	}



	return;
}

