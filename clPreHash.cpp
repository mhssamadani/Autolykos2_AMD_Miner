#include "clPreHash.h"
PreHashClass::PreHashClass(CLWarpper *cll)
{
	cl = cll;

	const  string buildOptions = "  -w -I .";
	program = cl->buildProgramFromFile("PreHashKernel.cl", buildOptions);

}
PreHashClass::~PreHashClass()
{

}

int PreHashClass::Prehash(
	uint64_t N_LEN,
	// data: height
	cl_uint  height,
	// hashes
	cl_mem   hashes
	)
{

	cl_uint len = N_LEN;

	cl_kernel kernel = program->getKernel("InitPrehash");

	cl->checkError(clSetKernelArg(kernel, 0, sizeof(cl_uint), &height));
	cl->checkError(clSetKernelArg(kernel, 1, sizeof(cl_uint), &N_LEN));
	cl->checkError(clSetKernelArg(kernel, 2, sizeof(cl_mem), &hashes));



	size_t t1 = ((N_LEN / BLOCK_DIM) + 1) * BLOCK_DIM;
	size_t global_work_size[1] = { t1 };

	size_t local_work_size[1] = { BLOCK_DIM };

	// Run the kernel.
	cl_int  err = clEnqueueNDRangeKernel(*cl->queue, kernel, 1, 0, global_work_size, local_work_size, 0, 0, 0);
	cl->checkError(err);
	err = clFinish(*cl->queue);
	cl->checkError(err);



	return EXIT_SUCCESS;
}
