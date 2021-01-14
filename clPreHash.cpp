#include "clPreHash.h"
PreHashClass::PreHashClass(CLWarpper *cll)
{
	cl = cll;

	const  string buildOptions = " -w -I .";
	program = cl->buildProgramFromFile("PreHashKernel.cl", buildOptions);

}
PreHashClass::~PreHashClass()
{

}

// first iteration of hashes precalculation
void PreHashClass::hInitPrehash(
	// data: pk || mes || w || padding || x || sk
	const cl_uint * data,
	// hashes
	cl_uint * hashes,
	// indices of invalid range hashes
	cl_uint * invalid
	) 
{
	cl_kernel kernel = program->getKernel("InitPrehash");
}

// uncompleted first iteration of hashes precalculation
void PreHashClass::hUncompleteInitPrehash(
	// data: pk
	const cl_mem  data,
	// unfinalized hash contexts
	cl_mem * uctxs ,
	cl_ulong *memsize,
	cl_uint memCount
	)
{
	//cout << "\n GPU " << cl->m_gpuIndex << " :Running kernel (UncompleteInitPrehash) \n";
	cl_kernel kernel = program->getKernel("UncompleteInitPrehash");


	cl->checkError(clSetKernelArg(kernel, 0, sizeof(cl_mem), &data));
	cl->checkError(clSetKernelArg(kernel, 1, sizeof(cl_mem), &uctxs[0]));
	cl->checkError(clSetKernelArg(kernel, 2, sizeof(cl_mem), &uctxs[1]));
	cl->checkError(clSetKernelArg(kernel, 3, sizeof(cl_ulong), &memsize[0]));
	cl->checkError(clSetKernelArg(kernel, 4, sizeof(cl_ulong), &memsize[1]));
	cl->checkError(clSetKernelArg(kernel, 5, sizeof(cl_uint), &memCount));


	size_t t1 = ((N_LEN / BLOCK_DIM) + 1) * BLOCK_DIM;
	size_t global_work_size[1] = { t1 };
	size_t local_work_size[1] = { BLOCK_DIM };

	cl_int  err = clEnqueueNDRangeKernel(*cl->queue, kernel, 1, 0, global_work_size, local_work_size, 0, 0, 0);
	cl->checkError(err);
	err = clFinish(*cl->queue);
	cl->checkError(err);

}

// complete first iteration of hashes precalculation
void PreHashClass::hCompleteInitPrehash(
	// data: pk || mes || w || padding || x || sk
	const cl_uint * data,
	// unfinalized hash contexts
	const uctx_t * uctxs,
	// hashes
	cl_uint * hashes,
	// indices of invalid range hashes
	cl_uint * invalid
	)
{
	cl_kernel kernel = program->getKernel("CompleteInitPrehash");

}




// hashes by secret key multiplication modulo Q 
void PreHashClass::hFinalPrehashMultSecKey(
	// data: pk || mes || w || padding || x || sk
	cl_mem data,
	// hashes
	cl_mem hashes
	)
{

	cl_kernel kernel = program->getKernel("FinalPrehashMultSecKey");


	cl->checkError(clSetKernelArg(kernel, 0, sizeof(cl_mem), &data));
	cl->checkError(clSetKernelArg(kernel, 1, sizeof(cl_mem), &hashes));

	// Run the kernel.
	//cout << "\n GPU "<<cl->m_gpuIndex <<  " :Running kernel (FinalPrehash) \n";

	size_t t1 = ((N_LEN / BLOCK_DIM) + 1) * BLOCK_DIM;
	size_t global_work_size[1] = { t1 };
	size_t local_work_size[1] = { BLOCK_DIM };

	cl_int  err = clEnqueueNDRangeKernel(*cl->queue, kernel, 1, 0, global_work_size, local_work_size, 0, 0, 0);
	cl->checkError(err);
	err = clFinish(*cl->queue);
	cl->checkError(err);




}

int PreHashClass::Prehash(
	// data: height
	cl_uint  height,
	// hashes
	cl_mem   hashes
	)
{

	cl_uint len = N_LEN;

	cl_kernel kernel = program->getKernel("InitPrehash");

	cl->checkError(clSetKernelArg(kernel, 0, sizeof(cl_uint), &height));
	cl->checkError(clSetKernelArg(kernel, 1, sizeof(cl_mem), &hashes));



	size_t t1 = ((N_LEN / BLOCK_DIM) + 1) * BLOCK_DIM;
	size_t global_work_size[1] = { t1 };

	size_t local_work_size[1] = { BLOCK_DIM };

	// Run the kernel.
	//cout << "\n GPU "<<cl->m_gpuIndex <<  " :Running kernel (InitPrehash) \n";

	cl_int  err = clEnqueueNDRangeKernel(*cl->queue, kernel, 1, 0, global_work_size, local_work_size, 0, 0, 0);
	cl->checkError(err);
	err = clFinish(*cl->queue);
	cl->checkError(err);



	return EXIT_SUCCESS;
}
