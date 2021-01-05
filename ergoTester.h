/**********************************************************************
Copyright ©2015 Advanced Micro Devices, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

•	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
•	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or
 other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

// For clarity,error checking has been omitted.

#ifndef a1
#define a1

#include <CL/cl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>

#define SUCCESS 0
#define FAILURE 1

using namespace std;
//namespace ch = std::chrono;
//using namespace std::chrono;


#include "cryptography.h"
#include "definitions.h"
#include "easylogging++.h"
#include "request.h"
#include "clPreHash.h"
#include "clMining.h"

#include <ctype.h>
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




class ergoTester
{
public:

	ergoTester()
	{

	}
	~ergoTester()
	{

	}
CLWarpper *clw;

/* convert the kernel file into a string */
int convertToString(const char *filename, std::string& s)
{
	size_t size;
	char*  str;
	std::fstream f(filename, (std::fstream::in | std::fstream::binary));

	if (f.is_open())
	{
		size_t fileSize;
		f.seekg(0, std::fstream::end);
		size = fileSize = (size_t)f.tellg();
		f.seekg(0, std::fstream::beg);
		str = new char[size + 1];
		if (!str)
		{
			f.close();
			return 0;
		}

		f.read(str, fileSize);
		f.close();
		str[size] = '\0';
		s = str;
		delete[] str;
		return 0;
	}
	cout << "Error: failed to open file\n:" << filename << endl;
	return FAILURE;
}



////////////////////////////////////////////////////////////////////////////////
//  Test solutions correctness
////////////////////////////////////////////////////////////////////////////////
int TestSolutions(
	const info_t * info,
	const uint8_t * x,
	const uint8_t * w
)
{
	LOG(INFO) << "Solutions test started";
	LOG(INFO) << "Set keepPrehash = " << ((info->keepPrehash) ? "true" : "false");
	bool keepPrehash = info->keepPrehash;

	//========================================================================//
	//  Check GPU memory
	//========================================================================//
	size_t freeMem = 0;
	size_t totalMem = 0;

	cl_ulong max_mem_alloc_size = clw->getDeviceInfoInt64(CL_DEVICE_MAX_MEM_ALLOC_SIZE);
	cl_ulong mem_size = clw->getDeviceInfoInt64(CL_DEVICE_GLOBAL_MEM_SIZE);

	LOG(INFO) << "GPU " << clw->m_gpuIndex << " mem_size: " << clw->getGlobalSizeMB() << "  (MB) , max_mem_alloc_size: " << clw->getMaxAllocSizeMB() << " (MB) ";
	size_t uctx_t_count = max_mem_alloc_size / sizeof(uctx_t);

	size_t memCount = (N_LEN / uctx_t_count) + 1;
	size_t n_len = N_LEN;

	if (max_mem_alloc_size < MIN_FREE_MEMORY)
	{
		LOG(ERROR) << "GPU " << clw->m_gpuIndex << " Not enough GPU memory for mining,"
			<< " minimum 2.8 GiB needed";

		return 0;
	}

	if (keepPrehash && (memCount > 2 || memCount < 0))
	{
		LOG(ERROR) << "GPU " << clw->m_gpuIndex << "C: Error in Memory Alloc for KeepPrehash";
		keepPrehash = false;
	}

	//========================================================================//
	//  Host memory allocation
	//========================================================================//
	// hash context
	// (212 + 4) bytes
	ctx_t ctx_h;
	//========================================================================//
	//  Device memory allocation
	//========================================================================//
	int deviceId = 0;
	LOG(INFO) << "GPU " << deviceId << " allocating memory";
	size_t allocatedMem = 0;

	//-------------------------------------------------------------
	// boundary for puzzle
	// (2 * PK_SIZE_8 + 2 + 4 * NUM_SIZE_8 + 212 + 4) bytes // ~0 MiB
	cl_mem bound_d = clw->Createbuffer((NUM_SIZE_8 + DATA_SIZE_8) * sizeof(char), CL_MEM_READ_WRITE);
	if (bound_d == NULL)
	{
		LOG(INFO) << "GPU " << deviceId << "error in  allocating hashbound_des_d";
		return 0;
	}
	cl_uint* hbound_d = (cl_uint*)malloc((NUM_SIZE_8 + DATA_SIZE_8) * sizeof(char));
	allocatedMem += (NUM_SIZE_8 + DATA_SIZE_8) * sizeof(char);
	//-------------------------------------------------------------
	//-------------------------------------------------------------
	// pool boundary for puzzle
	// (2 * PK_SIZE_8 + 2 + 4 * NUM_SIZE_8 + 212 + 4) bytes // ~0 MiB
	cl_mem pbound_d = clw->Createbuffer((NUM_SIZE_8 + DATA_SIZE_8) * sizeof(char), CL_MEM_READ_WRITE);
	if (pbound_d == NULL)
	{
		LOG(INFO) << "GPU " << deviceId << "error in  allocating phashbound_des_d";
		return 0 ;
	}
	cl_uint* hpbound_d = (cl_uint*)malloc((NUM_SIZE_8 + DATA_SIZE_8) * sizeof(char));
	allocatedMem += (NUM_SIZE_8 + DATA_SIZE_8) * sizeof(char);
	//-------------------------------------------------------------

	// data: pk || mes || w || padding || x || sk || ctx
	//cl_uint * data_d = bound_d + NUM_SIZE_32;
	cl_mem data_d = clw->Createbuffer((2 * PK_SIZE_8 + 2 + 3 * NUM_SIZE_8 + 212 + 4) * sizeof(char), CL_MEM_READ_WRITE);
	if (data_d == NULL)
	{
		LOG(INFO) << "GPU " << deviceId << "error in  allocating data_d";
		return 0;
	}
	cl_uint* hdata_d = (cl_uint*)malloc((2 * PK_SIZE_8 + 2 + 3 * NUM_SIZE_8 + 212 + 4) * sizeof(char));
	allocatedMem += (2 * PK_SIZE_8 + 2 + 3 * NUM_SIZE_8 + 212 + 4) * sizeof(char);

	// precalculated hashes
	// N_LEN * NUM_SIZE_8 bytes // 2 GiB
	cl_mem hashes_d = clw->Createbuffer((cl_uint)N_LEN * NUM_SIZE_8 * sizeof(char), CL_MEM_READ_WRITE);
	if (hashes_d == NULL)
	{
		LOG(INFO) << "GPU " << deviceId << "error in  allocating hashes_d";
		return 0;
	}
	allocatedMem += (cl_uint)N_LEN * NUM_SIZE_8 * sizeof(char);
	//cl_uint* hhashes_d = (cl_uint*)malloc((cl_uint)N_LEN * NUM_SIZE_8 * sizeof(char));


	/////////--------------------------------------------------------------------------------------
	// WORKSPACE_SIZE_8 bytes // depends on macros, now ~512 MiB
	// potential solutions of puzzle
	cl_mem res_d = clw->Createbuffer((NUM_SIZE_8 + sizeof(cl_uint)) * sizeof(char), CL_MEM_WRITE_ONLY);
	if (res_d == NULL)
	{
		LOG(INFO) << "GPU " << deviceId << "error in  allocating res_d";
		return 0;
	}
	cl_uint* hres_d = (cl_uint*)malloc((NUM_SIZE_8 + sizeof(cl_uint)) * sizeof(char));
	allocatedMem += (NUM_SIZE_8 + sizeof(cl_uint)) * sizeof(char);

	// indices of unfinalized hashes
	cl_mem indices_d = clw->Createbuffer(sizeof(cl_uint), CL_MEM_READ_WRITE);
	if (indices_d == NULL)
	{
		LOG(INFO) << "GPU " << deviceId << "error in  allocating indices_d";
		return 0;
	}
	cl_uint* hindices_d = (cl_uint*)malloc(sizeof(cl_uint));
	allocatedMem += sizeof(cl_uint);

	memset(hindices_d, 0, sizeof(cl_uint));
	clw->CopyBuffer(indices_d, hindices_d, sizeof(cl_uint), false);

	cl_mem count_d = clw->Createbuffer(sizeof(cl_uint), CL_MEM_READ_WRITE);
	if (count_d == NULL)
	{
		LOG(INFO) << "GPU " << deviceId << "error in  allocating count_d";
		return 0;
	}

	cl_uint* hcount_d = (cl_uint*)malloc(sizeof(cl_uint));
	allocatedMem += sizeof(cl_uint);

	memset(hcount_d, 0, sizeof(cl_uint));
	clw->CopyBuffer(count_d, hcount_d, sizeof(cl_uint), false);

	/////////--------------------------------------------------------------------------------------

	/////////--------------------------------------------------------------------------------------
	// WORKSPACE_SIZE_8 bytes // depends on macros, now ~512 MiB
	// potential solutions of puzzle
	size_t www = WORKSPACE_SIZE_8;
	cl_mem Pres_d = clw->Createbuffer(((NUM_SIZE_8 + sizeof(cl_uint))*MAX_POOL_RES) * sizeof(char), CL_MEM_WRITE_ONLY);
	if (Pres_d == NULL)
	{
		LOG(INFO) << "GPU " << deviceId << "error in  allocating Pres_d";
		return 0;
	}
	cl_uint* hPres_d = (cl_uint*)malloc(((NUM_SIZE_8 + sizeof(cl_uint))*MAX_POOL_RES) * sizeof(char));
	allocatedMem += ((NUM_SIZE_8 + sizeof(cl_uint))*MAX_POOL_RES);

	// indices of ..........
	cl_mem Pindices_d = clw->Createbuffer(MAX_POOL_RES*sizeof(cl_uint), CL_MEM_READ_WRITE);
	if (Pindices_d == NULL)
	{
		LOG(INFO) << "GPU " << deviceId << "error in  allocating Pindices_d";
		return 0;
	}
	cl_uint* hPindices_d = (cl_uint*)malloc(MAX_POOL_RES*sizeof(cl_uint));
	allocatedMem += (MAX_POOL_RES*sizeof(cl_uint));

	memset(hPindices_d, 0, MAX_POOL_RES*sizeof(cl_uint));
	clw->CopyBuffer(Pindices_d, hPindices_d, MAX_POOL_RES*sizeof(cl_uint), false);
	cl_mem pResCount_d = clw->Createbuffer(sizeof(cl_uint), CL_MEM_READ_WRITE);
	if (pResCount_d == NULL)
	{
		LOG(INFO) << "GPU " << deviceId << "error in  allocating pResCount_d";
		return 0;
	}

	cl_uint* hpResCount_d = (cl_uint*)malloc(sizeof(cl_uint));
	allocatedMem += sizeof(cl_uint);

	memset(hpResCount_d, 0, sizeof(cl_uint));
	clw->CopyBuffer(pResCount_d, hpResCount_d, sizeof(cl_uint), false);

	/////////--------------------------------------------------------------------------------------




	// unfinalized hash contexts
	// if keepPrehash == true // N_LEN * 80 bytes // 5 GiB
	cl_mem uctxs_d[100];
	cl_ulong memSize[100];
	uctxs_d[0] = uctxs_d[1] = NULL;
	if (keepPrehash)
	{
		size_t preS = (cl_uint)N_LEN * sizeof(uctx_t);
		if (memCount > 2 || memCount < 0)
		{
			LOG(INFO) << "C: Error in Memory Alloc for KeepPrehash";
			keepPrehash = false;

		}
		else if ((allocatedMem + preS) < mem_size)
		{

			size_t last = N_LEN % uctx_t_count;
			size_t sz = 0;
			for (size_t i = 0; i < memCount; i++)
			{
				if (i == memCount - 1)
				{
					uctxs_d[i] = clw->Createbuffer(last * sizeof(uctx_t), CL_MEM_READ_WRITE);
					//huctxs_d[i] = (uctx_t*)malloc(last);
					memSize[i] = i * uctx_t_count + last;
				}
				else
				{
					uctxs_d[i] = clw->Createbuffer(uctx_t_count * sizeof(uctx_t), CL_MEM_READ_WRITE);
					//huctxs_d[i] = (uctx_t*)malloc(uctx_t_count * sizeof(uctx_t));
					memSize[i] = i * uctx_t_count + uctx_t_count;
				}
				if (uctxs_d[i] == NULL)
				{
					LOG(INFO) << "A: Error in Memory Alloc for KeepPrehash";
					keepPrehash = false;
				}

			}
		}
		else
		{
			LOG(INFO) << "B: Error in Memory Alloc for KeepPrehash";
			keepPrehash = false;

		}

	}

	//========================================================================//
	//  Data transfer form host to device
	//========================================================================//
	// copy boundary
	memcpy(hbound_d, (void*)info->bound, NUM_SIZE_8);

	// copy Pboundary
	memcpy(hpbound_d, (void*)info->poolbound, NUM_SIZE_8);


	//// copy public key
	memcpy(hdata_d, (void*)info->pk, PK_SIZE_8);

	//// copy message
	memcpy((uint8_t*)hdata_d + PK_SIZE_8, info->mes, NUM_SIZE_8);

	//// copy one time public key
	memcpy((uint8_t*)hdata_d + PK_SIZE_8 + NUM_SIZE_8, (void*)w, PK_SIZE_8);

	//// copy one time secret key
	memcpy((cl_uint*)hdata_d + COUPLED_PK_SIZE_32 + NUM_SIZE_32, (void*)x, NUM_SIZE_8);

	unsigned long b3 = COUPLED_PK_SIZE_32 + NUM_SIZE_32;
	//ret = clEnqueueWriteBuffer(commandQueue, (cl_mem)((cl_uint *)data_d+b3) , CL_TRUE, 0, NUM_SIZE_8, x, 0, NULL, NULL);

	//// copy secret key
	memcpy((cl_uint*)hdata_d + COUPLED_PK_SIZE_32 + 2 * NUM_SIZE_32, (void*)info->sk, NUM_SIZE_8);


	cl_int ret = clw->CopyBuffer(bound_d, hbound_d, (NUM_SIZE_8 + DATA_SIZE_8) * sizeof(char), false);
	ret = clw->CopyBuffer(pbound_d, hpbound_d, (NUM_SIZE_8 + DATA_SIZE_8) * sizeof(char), false);
	ret = clw->CopyBuffer(data_d, hdata_d, (2 * PK_SIZE_8 + 2 + 3 * NUM_SIZE_8 + 212 + 4) * sizeof(char), false);
	////========================================================================//
	////  Test solutions
	////========================================================================//
	cl_ulong base = 0;
	PreHashClass *ph = new PreHashClass(clw);

	if (keepPrehash)
	{
		ph->hUncompleteInitPrehash(data_d, uctxs_d,memSize, memCount);
	}

	
	std::chrono::milliseconds ms = std::chrono::milliseconds::zero();

	std::chrono::milliseconds start = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
		);

//	ph->Prehash(keepPrehash, data_d, uctxs_d, memSize,memCount, hashes_d, res_d/*,ldata*/);

	ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
		) - start;

	LOG(INFO) << "Prehash time: " << ms.count() << " ms";


	//// calculate unfinalized hash of message
	MiningClass *min = new MiningClass(clw);
	min->InitMining(&ctx_h, (cl_uint *)info->mes, NUM_SIZE_8);


	//// copy context
	ret = clw->CopyBuffer(data_d, hdata_d, (2 * PK_SIZE_8 + 2 + 3 * NUM_SIZE_8 + 212 + 4) * sizeof(char), true);
	memcpy(hdata_d + COUPLED_PK_SIZE_32 + 3 * NUM_SIZE_32, &ctx_h, sizeof(ctx_t));
	ret = clw->CopyBuffer(data_d, hdata_d, (2 * PK_SIZE_8 + 2 + 3 * NUM_SIZE_8 + 212 + 4) * sizeof(char), false);

	memset(hcount_d, 0, sizeof(cl_uint));
	clw->CopyBuffer(count_d, hcount_d, sizeof(cl_uint), false);
	memset(hpResCount_d, 0, sizeof(cl_uint));
	clw->CopyBuffer(pResCount_d, hpResCount_d, sizeof(cl_uint), false);
	memset(hPindices_d, 0, MAX_POOL_RES*sizeof(cl_uint));
	clw->CopyBuffer(Pindices_d, hPindices_d, MAX_POOL_RES*sizeof(cl_uint), false);

	//// calculate solution candidates
//	min->hBlockMining(bound_d, pbound_d, data_d, base, hashes_d, res_d, indices_d, count_d, Pres_d, Pindices_d, pResCount_d);


	uint64_t res_h[NUM_SIZE_64];
	uint32_t solFound = 0;
	uint32_t nonce;
	// copy results to host
	ret = clw->CopyBuffer(res_d, res_h, NUM_SIZE_8, true);
	ret = clw->CopyBuffer(indices_d, &nonce, sizeof(cl_uint), true);

	LOG(INFO) << "Found nonce: " << nonce - 1;
	if (nonce != 0x3381BF)
	{
		LOG(ERROR) << "Solutions test failed: wrong nonce";
		exit(EXIT_FAILURE);
	}
	

	cl_uint countOfP = 0;
	ret = clw->CopyBuffer(pResCount_d, &countOfP, sizeof(cl_uint), true);

	uint32_t tt[NUM_SIZE_32];
	if (countOfP) //pool
	{
		ret = clw->CopyBuffer(Pres_d, hPres_d, ((NUM_SIZE_8 + sizeof(cl_uint))*MAX_POOL_RES)* sizeof(char), true);
		ret = clw->CopyBuffer(Pindices_d, hPindices_d, MAX_POOL_RES*sizeof(cl_uint), true);

		for (int pj = 0; pj < countOfP; pj++)
		{
			for (int i = 0; i < NUM_SIZE_32; ++i)
			{
				tt[i] = hPres_d[pj * NUM_SIZE_32 + i];
			}

			uint64_t *tt2 = (uint64_t *)tt;
			int abc;
			abc = 0;

		}
	}



	////========================================================================//
	////  Device memory deallocation
	////========================================================================//
	clReleaseMemObject(bound_d);
	clReleaseMemObject( hashes_d);
	clReleaseMemObject(res_d);

	if (keepPrehash) 
	{
		clReleaseMemObject(uctxs_d[0]);
		clReleaseMemObject(uctxs_d[1]);
	}

	LOG(INFO) << "Solutions test passed\n";

	return EXIT_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////
//  Test performance
////////////////////////////////////////////////////////////////////////////////
int TestPerformance(
	const info_t* info,
	const uint8_t* x,
	const uint8_t* w
)
{
	LOG(INFO) << "Solutions Performance";
	LOG(INFO) << "Set keepPrehash = " << ((info->keepPrehash) ? "true" : "false");

	//========================================================================//
	//  Host memory allocation
	//========================================================================//
	// hash context
	ctx_t ctx_h;
	bool keepPrehash = info->keepPrehash;
	//========================================================================//
	//  Check GPU memory
	//========================================================================//
	size_t freeMem = 0;
	size_t totalMem = 0;

	cl_ulong max_mem_alloc_size = clw->getDeviceInfoInt64(CL_DEVICE_MAX_MEM_ALLOC_SIZE);
	cl_ulong mem_size = clw->getDeviceInfoInt64(CL_DEVICE_GLOBAL_MEM_SIZE);

	LOG(INFO) << "GPU " << clw->m_gpuIndex << " mem_size: " << clw->getGlobalSizeMB() << "  (MB) , max_mem_alloc_size: " << clw->getMaxAllocSizeMB() << " (MB) ";
	size_t uctx_t_count = max_mem_alloc_size / sizeof(uctx_t);

	size_t memCount = (N_LEN / uctx_t_count) + 1;
	size_t n_len = N_LEN;

	if (max_mem_alloc_size < MIN_FREE_MEMORY)
	{
		LOG(ERROR) << "GPU " << clw->m_gpuIndex << " Not enough GPU memory for mining,"
			<< " minimum 2.8 GiB needed";

		return 0;
	}

	if (keepPrehash && (memCount > 2 || memCount < 0))
	{
		LOG(ERROR) << "GPU " << clw->m_gpuIndex << "C: Error in Memory Alloc for KeepPrehash";
		keepPrehash = false;
	}

	//========================================================================//
	//  Device memory allocation
	//========================================================================//
	int deviceId = 0;
	LOG(INFO) << "GPU " << deviceId << " allocating memory";
	size_t allocatedMem = 0;

	//-------------------------------------------------------------
	// boundary for puzzle
	// (2 * PK_SIZE_8 + 2 + 4 * NUM_SIZE_8 + 212 + 4) bytes // ~0 MiB
	cl_mem bound_d = clw->Createbuffer((NUM_SIZE_8 + DATA_SIZE_8) * sizeof(char), CL_MEM_READ_WRITE);
	if (bound_d == NULL)
	{
		LOG(INFO) << "GPU " << deviceId << "error in  allocating hashbound_des_d";
		return 0;
	}
	cl_uint* hbound_d = (cl_uint*)malloc((NUM_SIZE_8 + DATA_SIZE_8) * sizeof(char));
	allocatedMem += (NUM_SIZE_8 + DATA_SIZE_8) * sizeof(char);
	//-------------------------------------------------------------
	//-------------------------------------------------------------
	// pool boundary for puzzle
	// (2 * PK_SIZE_8 + 2 + 4 * NUM_SIZE_8 + 212 + 4) bytes // ~0 MiB
	cl_mem pbound_d = clw->Createbuffer((NUM_SIZE_8 + DATA_SIZE_8) * sizeof(char), CL_MEM_READ_WRITE);
	if (pbound_d == NULL)
	{
		LOG(INFO) << "GPU " << deviceId << "error in  allocating phashbound_des_d";
		return 0;
	}
	cl_uint* hpbound_d = (cl_uint*)malloc((NUM_SIZE_8 + DATA_SIZE_8) * sizeof(char));
	allocatedMem += (NUM_SIZE_8 + DATA_SIZE_8) * sizeof(char);
	//-------------------------------------------------------------

	// data: pk || mes || w || padding || x || sk || ctx
	//cl_uint * data_d = bound_d + NUM_SIZE_32;
	cl_mem data_d = clw->Createbuffer((2 * PK_SIZE_8 + 2 + 3 * NUM_SIZE_8 + 212 + 4) * sizeof(char), CL_MEM_READ_WRITE);
	if (data_d == NULL)
	{
		LOG(INFO) << "GPU " << deviceId << "error in  allocating data_d";
		return 0;
	}
	cl_uint* hdata_d = (cl_uint*)malloc((2 * PK_SIZE_8 + 2 + 3 * NUM_SIZE_8 + 212 + 4) * sizeof(char));
	allocatedMem += (2 * PK_SIZE_8 + 2 + 3 * NUM_SIZE_8 + 212 + 4) * sizeof(char);

	// precalculated hashes
	// N_LEN * NUM_SIZE_8 bytes // 2 GiB
	cl_mem hashes_d = clw->Createbuffer((cl_uint)N_LEN * NUM_SIZE_8 * sizeof(char), CL_MEM_READ_WRITE);
	if (hashes_d == NULL)
	{
		LOG(INFO) << "GPU " << deviceId << "error in  allocating hashes_d";
		return 0;
	}
	allocatedMem += (cl_uint)N_LEN * NUM_SIZE_8 * sizeof(char);
	//cl_uint* hhashes_d = (cl_uint*)malloc((cl_uint)N_LEN * NUM_SIZE_8 * sizeof(char));


	/////////--------------------------------------------------------------------------------------
	// WORKSPACE_SIZE_8 bytes // depends on macros, now ~512 MiB
	// potential solutions of puzzle
	cl_mem res_d = clw->Createbuffer((NUM_SIZE_8 + sizeof(cl_uint)) * sizeof(char), CL_MEM_WRITE_ONLY);
	if (res_d == NULL)
	{
		LOG(INFO) << "GPU " << deviceId << "error in  allocating res_d";
		return 0;
	}
	cl_uint* hres_d = (cl_uint*)malloc((NUM_SIZE_8 + sizeof(cl_uint)) * sizeof(char));
	allocatedMem += (NUM_SIZE_8 + sizeof(cl_uint)) * sizeof(char);

	// indices of unfinalized hashes
	cl_mem indices_d = clw->Createbuffer(sizeof(cl_uint), CL_MEM_READ_WRITE);
	if (indices_d == NULL)
	{
		LOG(INFO) << "GPU " << deviceId << "error in  allocating indices_d";
		return 0;
	}
	cl_uint* hindices_d = (cl_uint*)malloc(sizeof(cl_uint));
	allocatedMem += sizeof(cl_uint);

	memset(hindices_d, 0, sizeof(cl_uint));
	clw->CopyBuffer(indices_d, hindices_d, sizeof(cl_uint), false);

	cl_mem count_d = clw->Createbuffer(sizeof(cl_uint), CL_MEM_READ_WRITE);
	if (count_d == NULL)
	{
		LOG(INFO) << "GPU " << deviceId << "error in  allocating count_d";
		return 0;
	}

	cl_uint* hcount_d = (cl_uint*)malloc(sizeof(cl_uint));
	allocatedMem += sizeof(cl_uint);

	memset(hcount_d, 0, sizeof(cl_uint));
	clw->CopyBuffer(count_d, hcount_d, sizeof(cl_uint), false);

	/////////--------------------------------------------------------------------------------------

	/////////--------------------------------------------------------------------------------------
	// WORKSPACE_SIZE_8 bytes // depends on macros, now ~512 MiB
	// potential solutions of puzzle
	size_t www = WORKSPACE_SIZE_8;
	cl_mem Pres_d = clw->Createbuffer(((NUM_SIZE_8 + sizeof(cl_uint))*MAX_POOL_RES) * sizeof(char), CL_MEM_WRITE_ONLY);
	if (Pres_d == NULL)
	{
		LOG(INFO) << "GPU " << deviceId << "error in  allocating Pres_d";
		return 0;
	}
	cl_uint* hPres_d = (cl_uint*)malloc(((NUM_SIZE_8 + sizeof(cl_uint))*MAX_POOL_RES) * sizeof(char));
	allocatedMem += ((NUM_SIZE_8 + sizeof(cl_uint))*MAX_POOL_RES);

	// indices of ..........
	cl_mem Pindices_d = clw->Createbuffer(MAX_POOL_RES*sizeof(cl_uint), CL_MEM_READ_WRITE);
	if (Pindices_d == NULL)
	{
		LOG(INFO) << "GPU " << deviceId << "error in  allocating Pindices_d";
		return 0;
	}
	cl_uint* hPindices_d = (cl_uint*)malloc(MAX_POOL_RES*sizeof(cl_uint));
	allocatedMem += (MAX_POOL_RES*sizeof(cl_uint));

	memset(hPindices_d, 0, MAX_POOL_RES*sizeof(cl_uint));
	clw->CopyBuffer(Pindices_d, hPindices_d, MAX_POOL_RES*sizeof(cl_uint), false);

	cl_mem pResCount_d = clw->Createbuffer(sizeof(cl_uint), CL_MEM_READ_WRITE);
	if (pResCount_d == NULL)
	{
		LOG(INFO) << "GPU " << deviceId << "error in  allocating pResCount_d";
		return 0;
	}

	cl_uint* hpResCount_d = (cl_uint*)malloc(sizeof(cl_uint));
	allocatedMem += sizeof(cl_uint);


	memset(hpResCount_d, 0, sizeof(cl_uint));
	clw->CopyBuffer(pResCount_d, hpResCount_d, sizeof(cl_uint), false);
	/////////--------------------------------------------------------------------------------------




	// unfinalized hash contexts
	// if keepPrehash == true // N_LEN * 80 bytes // 5 GiB
	cl_mem uctxs_d[100];
	cl_ulong memSize[100];
	uctxs_d[0] = uctxs_d[1] = NULL;
	if (keepPrehash)
	{
		size_t preS = (cl_uint)N_LEN * sizeof(uctx_t);
		if (memCount > 2 || memCount < 0)
		{
			LOG(INFO) << "C: Error in Memory Alloc for KeepPrehash";
			keepPrehash = false;

		}
		else if ((allocatedMem + preS) < mem_size)
		{

			size_t last = N_LEN % uctx_t_count;
			size_t sz = 0;
			for (size_t i = 0; i < memCount; i++)
			{
				if (i == memCount - 1)
				{
					uctxs_d[i] = clw->Createbuffer(last * sizeof(uctx_t), CL_MEM_READ_WRITE);
					//huctxs_d[i] = (uctx_t*)malloc(last);
					memSize[i] = i * uctx_t_count + last;
				}
				else
				{
					uctxs_d[i] = clw->Createbuffer(uctx_t_count * sizeof(uctx_t), CL_MEM_READ_WRITE);
					//huctxs_d[i] = (uctx_t*)malloc(uctx_t_count * sizeof(uctx_t));
					memSize[i] = i * uctx_t_count + uctx_t_count;
				}
				if (uctxs_d[i] == NULL)
				{
					LOG(INFO) << "A: Error in Memory Alloc for KeepPrehash";
					keepPrehash = false;
				}

			}
		}
		else
		{
			LOG(INFO) << "B: Error in Memory Alloc for KeepPrehash";
			keepPrehash = false;

		}

	}
	//========================================================================//
	//  Data transfer form host to device
	//========================================================================//
	// copy boundary
	memcpy(hbound_d, (void*)info->bound, NUM_SIZE_8);

	memcpy(hpbound_d, (void*)info->poolbound, NUM_SIZE_8);

	//// copy public key
	memcpy(hdata_d, (void*)info->pk, PK_SIZE_8);

	//// copy message
	memcpy((uint8_t*)hdata_d + PK_SIZE_8, info->mes, NUM_SIZE_8);

	//// copy one time public key
	memcpy((uint8_t*)hdata_d + PK_SIZE_8 + NUM_SIZE_8, (void*)w, PK_SIZE_8);

	//// copy one time secret key
	memcpy((cl_uint*)hdata_d + COUPLED_PK_SIZE_32 + NUM_SIZE_32, (void*)x, NUM_SIZE_8);

	unsigned long b3 = COUPLED_PK_SIZE_32 + NUM_SIZE_32;
	//ret = clEnqueueWriteBuffer(commandQueue, (cl_mem)((cl_uint *)data_d+b3) , CL_TRUE, 0, NUM_SIZE_8, x, 0, NULL, NULL);

	//// copy secret key
	memcpy((cl_uint*)hdata_d + COUPLED_PK_SIZE_32 + 2 * NUM_SIZE_32, (void*)info->sk, NUM_SIZE_8);


	cl_int ret = clw->CopyBuffer(bound_d, hbound_d, (NUM_SIZE_8 + DATA_SIZE_8) * sizeof(char), false);
	clw->CopyBuffer(data_d, hdata_d, (2 * PK_SIZE_8 + 2 + 3 * NUM_SIZE_8 + 212 + 4) * sizeof(char), false);
	////========================================================================//
	////  Test solutions
	////========================================================================//
	cl_ulong base = 0;
	PreHashClass* ph = new PreHashClass(clw);

	if (keepPrehash)
	{
		ph->hUncompleteInitPrehash(data_d, uctxs_d, memSize, memCount);
	}

	std::chrono::milliseconds ms = std::chrono::milliseconds::zero();

	LOG(INFO) << "Set keepPrehash " << keepPrehash;

	std::chrono::milliseconds start = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
		);

//	ph->Prehash(keepPrehash, data_d, uctxs_d, memSize, memCount, hashes_d, res_d/*,ldata*/);


	ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
		) - start;

	LOG(INFO) << "Prehash time: " << ms.count() << " ms";

	//// calculate unfinalized hash of message
	MiningClass* min = new MiningClass(clw);
	min->InitMining(&ctx_h, (cl_uint*)info->mes, NUM_SIZE_8);


	//// copy context
	ret = clw->CopyBuffer(data_d, hdata_d, (2 * PK_SIZE_8 + 2 + 3 * NUM_SIZE_8 + 212 + 4) * sizeof(char), true);
	memcpy(hdata_d + COUPLED_PK_SIZE_32 + 3 * NUM_SIZE_32, &ctx_h, sizeof(ctx_t));
	ret = clw->CopyBuffer(data_d, hdata_d, (2 * PK_SIZE_8 + 2 + 3 * NUM_SIZE_8 + 212 + 4) * sizeof(char), false);

	LOG(INFO) << "BlockMining now for 1 minute";
	ms = std::chrono::milliseconds::zero();

	uint32_t sum = 0;
	int iter = 0;
	uint32_t nonce = 0;
	start = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
		);

	for (; ms.count() < 6000; ++iter)
	{

		memset(hcount_d, 0, sizeof(cl_uint));
		clw->CopyBuffer(count_d, hcount_d, sizeof(cl_uint), false);
		memset(hpResCount_d, 0, sizeof(cl_uint));
		clw->CopyBuffer(pResCount_d, hpResCount_d, sizeof(cl_uint), false);
		memset(hPindices_d, 0, MAX_POOL_RES*sizeof(cl_uint));
		clw->CopyBuffer(Pindices_d, hPindices_d, MAX_POOL_RES*sizeof(cl_uint), false);

		//// calculate solution candidates
//		min->hBlockMining(bound_d, pbound_d, data_d, base, hashes_d, res_d, indices_d, count_d, Pres_d, Pindices_d, pResCount_d);


		ret = clw->CopyBuffer(indices_d, &nonce, sizeof(cl_uint), true);

		if (nonce != 0) ++sum;

		memset(indices_d, 0, sizeof(uint32_t));

		// reduction now removed so no findsum
		//sum += FindSum(indices_d, indices_d + NONCES_PER_ITER, NONCES_PER_ITER);
		base += NONCES_PER_ITER;

		ms = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()
			) - start;
	}

	////========================================================================//
	////  Device memory deallocation
	////========================================================================//
	clReleaseMemObject(bound_d);
	clReleaseMemObject(hashes_d);
	clReleaseMemObject(res_d);

	if (info->keepPrehash)
	{
//		clReleaseMemObject(uctxs_d);
	}

	LOG(INFO) << "Found " << sum << " solutions";
	LOG(INFO) << "Hashrate: " << (double)NONCES_PER_ITER* iter
		/ ((double)1000 * ms.count()) << " MH/s";
	LOG(INFO) << "Performance test completed\n";


	return EXIT_SUCCESS;
}


void TestNewCrypto()
{
	char mnemonic[] = "edge talent poet tortoise trumpet dose";
	uint8_t sk[NUM_SIZE_8];
	char skstr[NUM_SIZE_4 + 10/* aminm */];
	char pkstr[PK_SIZE_4 + 1];
	uint8_t pk[PK_SIZE_8];

	GenerateSecKeyNew(mnemonic, strlen(mnemonic), sk, skstr,(char *) "");


	if (strncmp(skstr, "392F75AD23278B3CD7B060D900138F20F8CBA89ABB259B5DCF5D9830B49D8E38", NUM_SIZE_4))
	{
		printf("%.64s private key1\n", skstr);
		LOG(ERROR) << "mnemonic -> private key conversion does not work correctly";
	}
	else
	{
		LOG(INFO) << "Mnemonic -> private key conversion works OK";
	}

}


// ugly stuff, will rewrite later
void TestRequests()
{
	json_t oldreq(0, REQ_LEN);
	json_t *newreq;
	newreq = new json_t(0, REQ_LEN);
	json_t oldreqbig(0, REQ_LEN);
	info_t testinfo;

	char bigrequest[] = "{ \"msg\" : \"46b7e94915275125129581725817295812759128"
		"571925871285728572857285725285728571928517287519285718"
		"275192857192857192857192587129581729587129581728571295"
		"817295182759128751928571925871285782758782751928571827"
		"519285787bfad202ab4e3dd9cc0603c1f61f53485854028b8fa03f"
		"399544fb298\", \"b\" : 2134827235332678044033321050158"
		"7889707005372997724693988999057291299,  \"pk\" : \"039"
		"5f8d54fdd5edb7eeab3228c952d39f5e60d048178f94ac992d4f76"
		"a6dce4c71\"  }";
	WriteFunc((void*)bigrequest, sizeof(char), strlen(bigrequest), &oldreqbig);
	if (strcmp(bigrequest, oldreqbig.ptr))
	{
		LOG(ERROR) << "WriteFunc strings do not match "
			<< bigrequest << "\n" << oldreqbig.ptr;
	}


	char request[] = "{ \"msg\" : \"46b7e949bfad202ab4e3dd9cc0603c1f61f5348585"
		"4028b8fa03f399544fb298\", \"b\" : 2134827235332678044033"
		"3210501587889707005372997724693988999057291299,  \"pk\" "
		": \"0395f8d54fdd5edb7eeab3228c952d39f5e60d048178f94ac992"
		"d4f76a6dce4c71\"  }";

	WriteFunc((void*)request, sizeof(char), strlen(request), &oldreq);
	if (strcmp(request, oldreq.ptr))
	{
		LOG(ERROR) << "WriteFunc strings do not match " << request << "\n" << oldreq.ptr;
	}



	char seedstring[] = "13cc81ef0b13fd496217c7c44b16c09d923ad475d897cffd37c63"
		"a15aebf579313d67934727d94ba42687f238480eb9248da9ba21e9c1";

	GenerateSecKey(
		seedstring, strlen(seedstring), testinfo.sk,
		testinfo.skstr
	);
	GeneratePublicKey(testinfo.skstr, testinfo.pkstr, testinfo.pk);

	char shortrequest[] = "{ \"msg\" : \"46b7e\", \"b\" : 2134, , \"pb\" : 2134,  \"pk\" : \"0395"
		"f8d54fdd5edb7eeab3228c952d39f5e60d048178f94ac992d4"
		"f76a6dce4c71\"  }";
	char brokenrequest[] = " \"msg\"  \"46b7e\", \"b\" : 2134,  \"pk\" : \"0395f8"
		"d54fdd5edb7eeab3228c952d39f5e60d048178f94ac992d4f76a6"
		"dce4c71\"  }";
	char uncompleterequest[] = "{ \"msg\" : \"46b7e\", \"pk\" : \"0395f8d54fdd5edb"
		"7eeab3228c952d39f5e60d048178f94ac992d4f76a6dce4c71\""
		" }";
	char uncompleterequest2[] = "{ \"b\" : 2134,  \"pk\" : \"0395f8d54fdd5edb7eeab"
		"3228c952d39f5e60d048178f94ac992d4f76a6dce4c71\"  }";

	WriteFunc((void*)shortrequest, sizeof(char), strlen(shortrequest), newreq);
	LOG(INFO) << "Testing short request "
		<< "\n result " << ((ParseRequest(&oldreq, newreq, &testinfo, 1) == EXIT_SUCCESS) ? "OK" : "ERROR");
	delete newreq;
	newreq = new json_t(0, REQ_LEN);
	WriteFunc((void*)bigrequest, sizeof(char), strlen(bigrequest), newreq);
	LOG(INFO) << "Testing big request "
		<< "\n result " << ((ParseRequest(&oldreq, newreq, &testinfo, 1) == EXIT_SUCCESS) ? "OK" : "ERROR");
	delete newreq;
	newreq = new json_t(0, REQ_LEN);
	WriteFunc((void*)brokenrequest, sizeof(char), strlen(brokenrequest), newreq);
	LOG(INFO) << "Testing broken request "
		<< "\n result " << ((ParseRequest(&oldreq, newreq, &testinfo, 1) == EXIT_SUCCESS) ? "ERROR" : "OK");
	delete newreq;
	newreq = new json_t(0, REQ_LEN);
	WriteFunc((void*)uncompleterequest, sizeof(char), strlen(uncompleterequest), newreq);
	LOG(INFO) << "Testing uncomplete request 1 "
		<< "\n result " << ((ParseRequest(&oldreq, newreq, &testinfo, 1) == EXIT_SUCCESS) ? "ERROR" : "OK");
	delete newreq;
	newreq = new json_t(0, REQ_LEN);
	WriteFunc((void*)uncompleterequest2, sizeof(char), strlen(uncompleterequest2), newreq);
	LOG(INFO) << "Testing uncomplete request 2 "
		<< "\n result " << ((ParseRequest(&oldreq, newreq, &testinfo, 1) == EXIT_SUCCESS) ? "ERROR" : "OK");
	delete newreq;




}


int testErgo(int argc, char* argv[])
{
	int trr = (2 * PK_SIZE_8 + NUM_SIZE_8);

	//----------------------------------------
	START_EASYLOGGINGPP(argc, argv);

	el::Loggers::reconfigureAllLoggers(
		el::ConfigurationType::Format, "%datetime %level [%thread] %msg"
	);

	el::Helpers::setThreadName("test thread");

	LOG(INFO) << "Checking crypto: ";

	TestNewCrypto();

	LOG(INFO) << "Testing requests:";

	TestRequests();

	//========================================================================//
	//  Check requirements
	//========================================================================//
	int status;
	clw = new CLWarpper(0);

	// CL_DEVICE_MAX_MEM_ALLOC_SIZE
	cl_ulong max_mem_alloc_size = clw->getMaxAllocSizeMB();
	printf("  CL_DEVICE_MAX_MEM_ALLOC_SIZE:\t\t%u MByte\n", max_mem_alloc_size);

	//// CL_DEVICE_GLOBAL_MEM_SIZE
	cl_ulong mem_size = clw->getGlobalSizeMB();
	printf("  CL_DEVICE_GLOBAL_MEM_SIZE:\t\t%u MByte\n", mem_size);

	size_t freeMem = 0;
	size_t totalMem = 0;

	////if (freeMem < MIN_FREE_MEMORY)    amimnM     opencl dont detect free memory
	//if(mem_size < MIN_FREE_MEMORY)
	//{
	//	LOG(ERROR) << "Not enough GPU memory for mining,"
	//		<< " minimum 2.8 GiB needed";

	//	exit(EXIT_FAILURE);
	//}

	clw->getSVMcapabilities();
	///*Step 8: Create kernel object */
	//err = 0;
	//kernel = clCreateKernel(program, "CompleteInitPrehash", &err);

	//========================================================================//
 //  Set test info
 //========================================================================//
	info_t info;
	uint8_t x[NUM_SIZE_8];
	uint8_t w[PK_SIZE_8];
	char seed[256] = "Va'esse deireadh aep eigean, va'esse eigh faidh'ar";

	// generate secret key from seed
	GenerateSecKey(seed, 50, info.sk, info.skstr);
	// generate public key from secret key
	GeneratePublicKey(info.skstr, info.pkstr, info.pk);

	const char ref_pkstr[PK_SIZE_4 + 1]
		= "020C16DFC5E23C59357E89D44977038F0A7851CC9926B3AABB3FF9E7E6A57315AD";

	int test = !strncmp(ref_pkstr, info.pkstr, PK_SIZE_4);

	if (!test)
	{
		LOG(ERROR) << "OpenSSL: generated wrong public key";
		return EXIT_FAILURE;
	}

	((cl_ulong *)info.bound)[0] = 0xFFFFFFFFFFFFFFFF;
	((cl_ulong *)info.bound)[1] = 0xFFFFFFFFFFFFFFFF;
	((cl_ulong *)info.bound)[2] = 0xFFFFFFFFFFFFFFFF;
	((uint64_t*)info.bound)[3] = 0x000002FFFFFFFFFF;

	((cl_ulong *)info.poolbound)[0] = 0x0;
	((cl_ulong *)info.poolbound)[1] = 0x0;
	((cl_ulong *)info.poolbound)[2] = 0x0;
	((cl_ulong *)info.poolbound)[3] = 0x0;


	((cl_ulong *)info.mes)[0] = 1;
	((cl_ulong *)info.mes)[1] = 0;
	((cl_ulong *)info.mes)[2] = 0;
	((cl_ulong *)info.mes)[3] = 0;

	sprintf(seed, "%d", 0);

	// generate secret key from seed
	GenerateSecKey(seed, 1, x, info.skstr);
	// generate public key from secret key
	GeneratePublicKey(info.skstr, info.pkstr, w);


	//========================================================================//
	//  Run solutions correctness tests
	//========================================================================//

//	info.keepPrehash = 1;
//	TestSolutions(&info, x, w);

/*
	if (NONCES_PER_ITER <= 0x3D5B84)
	{
		LOG(INFO) << "Need WORKSPACE value for at least 4021125,"
			<< " skip solutions tests\n";
	}
	else
	{
		info.keepPrehash = 1;
		TestSolutions(&info, x, w);
		if (freeMem < MIN_FREE_MEMORY_PREHASH)
		{
			LOG(INFO) << "Not enough GPU memory for keeping prehashes, "
				<< "skip test\n";
		}
		else
		{
			info.keepPrehash = 0;
			TestSolutions(&info, x, w);
		}
	}
*/


	//========================================================================//
	//  Run performance tests
	//========================================================================//
//	info.keepPrehash = (freeMem >= MIN_FREE_MEMORY_PREHASH) ? 1 : 0;

	LOG(INFO) << "Test suite executable is now terminated";

	//info.keepPrehash = 0;
	//TestPerformance(&info, x, w);

	info.keepPrehash = 1;
	TestPerformance(&info, x, w);


	return SUCCESS;
}


};

#endif // !a1
