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

class PreHashClass
{

	CLWarpper *cl ;
	cl_kernel kernel;
	std::shared_ptr<CLProgram> program;
public:
	PreHashClass(CLWarpper *cll);
	~PreHashClass();


	// first iteration of hashes precalculation
	void hInitPrehash(
		// data: pk || mes || w || padding || x || sk
		const cl_uint * data,
		// hashes
		cl_uint * hashes,
		// indices of invalid range hashes
		cl_uint * invalid
		);

	// uncompleted first iteration of hashes precalculation
	void hUncompleteInitPrehash(
		// data: pk
		const cl_mem  data,
		// unfinalized hash contexts
		cl_mem * uctxs,
		cl_ulong *memsize,
		cl_uint memCount
		);

	// complete first iteration of hashes precalculation
	void hCompleteInitPrehash(
		// data: pk || mes || w || padding || x || sk
		const cl_uint * data,
		// unfinalized hash contexts
		const uctx_t * uctxs,
		// hashes
		cl_uint * hashes,
		// indices of invalid range hashes
		cl_uint * invalid
		);

	// hashes by secret key multiplication modulo Q 
	void hFinalPrehashMultSecKey(
		// data: pk || mes || w || padding || x || sk
		cl_mem data,
		// hashes
		cl_mem hashes
		);


	int Prehash(
		const int keep,
		// data: height
		cl_uint  height,
		// hashes
		cl_mem   hashes,
		// indices of invalid range hashes
		cl_mem   invalid
		);

};

