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

	   	int Prehash(
		uint64_t N_LEN,
		// data: height
		cl_uint  height,
		// hashes
		cl_mem   hashes
		);

};

