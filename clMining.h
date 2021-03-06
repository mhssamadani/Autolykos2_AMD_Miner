#pragma once

#include "definitions.h"
#include "cl_warpper.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

class MiningClass
{

	CLWarpper *cl ;
	cl_kernel kernel;
	std::shared_ptr<CLProgram> program;
public:
	MiningClass(CLWarpper *cll);
	~MiningClass();

	// unfinalized hash of message
	void InitMining(
		// context
		ctx_t * ctx,
		// message
		const cl_uint * mes,
		// message length in bytes
		const cl_uint meslen
		);


	// block mining iteration
	void hBlockMining(
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
		cl_mem vCount,
		cl_mem BHashes = NULL
	);
};

