#include "clMining.h"
#include "easylogging++.h"
MiningClass::MiningClass(CLWarpper *cll)
{
	cl = cll;

	const  string buildOptions = "-I .";
	program = cl->buildProgramFromFile("MiningKernel.cl", buildOptions);

	int tr;
	tr = 0;

}
MiningClass::~MiningClass()
{

}

void MiningClass::InitMining(
	// context
	ctx_t * ctx,
	// message
	const cl_uint * mes,
	// message length in bytes
	const cl_uint meslen
	)
{
	cl_ulong aux[32];

	//========================================================================//
	//  Initialize context
	//========================================================================//
	memset(ctx->b, 0, BUF_SIZE_8);
	B2B_IV(ctx->h);
	ctx->h[0] ^= 0x01010000 ^ NUM_SIZE_8;
	memset(ctx->t, 0, 16);
	ctx->c = 0;

	//========================================================================//
	//  Hash message
	//========================================================================//
	for (uint_t j = 0; j < meslen; ++j)
	{
		if (ctx->c == BUF_SIZE_8) { HOST_B2B_H(ctx, aux); }

		ctx->b[ctx->c++] = ((const uint8_t *)mes)[j];
	}

	return;
}



void MiningClass::hBlockMining(
	// boundary for puzzle
	cl_mem bound,
	// data: mes
	cl_mem mes,
	// nonce base
	const cl_ulong base,
	const cl_ulong endNonce,
	// block height
	const cl_uint  height,
	// precalculated hashes
	cl_mem hashes,
	// indices of valid solutions
	cl_mem valid,
	cl_mem vCount
	)
{
	cl_kernel kernel = program->getKernel("BlockMining");

	int id = 0;
	cl->checkError(clSetKernelArg(kernel, id++, sizeof(cl_mem), &bound));
	cl->checkError(clSetKernelArg(kernel, id++, sizeof(cl_mem), &mes));
	cl->checkError(clSetKernelArg(kernel, id++, sizeof(cl_ulong), &base));
	cl->checkError(clSetKernelArg(kernel, id++, sizeof(cl_uint), &height));
	cl->checkError(clSetKernelArg(kernel, id++, sizeof(cl_mem), &hashes));
	cl->checkError(clSetKernelArg(kernel, id++, sizeof(cl_mem), &valid));
	cl->checkError(clSetKernelArg(kernel, id++, sizeof(cl_mem), &vCount));

	int threads = THREADS_PER_ITER;
	cl_ulong check = base + threads;
	if (check > endNonce)
	{
		threads = endNonce - base;
	}
	if (threads < 0)
	{
		LOG(INFO) << " negative threads, ( base: " << base << " , endNonce: " << endNonce << " ) ";
		return;
	}
	size_t t1 = ((threads / BLOCK_DIM) + 1) * BLOCK_DIM;
	size_t global_work_size[1] = { t1 };
	size_t local_work_size[1] = { BLOCK_DIM };

	cl_int  err = clEnqueueNDRangeKernel(*cl->queue, kernel, 1, 0, global_work_size, local_work_size, 0, 0, 0);
	cl->checkError(err);
	err = clFinish(*cl->queue);
	cl->checkError(err);
}
