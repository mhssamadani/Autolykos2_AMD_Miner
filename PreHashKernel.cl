
#include "OCLdecs.h"////problem with relative path

////////////////////////////////////////////////////////////////////////////////
//  First iteration of hashes precalculation
////////////////////////////////////////////////////////////////////////////////
__kernel void InitPrehash(
    // data: height
    const cl_uint  h,
    // hashes
    global cl_uint * hashes
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



            #pragma unroll
            for (int i = 0; i < NUM_SIZE_8-1; ++i) 
            {
                    //((uint8_t global*)hashes)[(tid + 1) * NUM_SIZE_8 - i -1] = ((uint8_t *)ldata)[i];
					((uint8_t global*)hashes)[tid * NUM_SIZE_8 +i ] = ((uint8_t *)ldata)[i];
            }
			// drop
			((uint8_t global*)hashes)[tid * NUM_SIZE_8 +31 ] = 0; 

	}

    return;
}

