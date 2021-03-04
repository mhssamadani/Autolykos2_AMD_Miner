#ifndef CL_WARPPER_H
#define CL_WARPPER_H

//#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>
#include <CL/cl_ext.h>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <map>
#include <memory>
#include "OCLdefs.h"

class CLProgram {
public:
	CLProgram(cl_program);
	cl_kernel getKernel(std::string kernelname);
	~CLProgram();
private:
	std::map<std::string, cl_kernel> kernels;
	cl_program program;
};

class CLWarpper {
public:
	bool verbose;

	cl_int error;

	cl_platform_id platform_id;
	cl_device_id device;

	cl_context *context;
	cl_command_queue *queue;

	template<typename T>
	static std::string toString(T val) {
		std::ostringstream myostringstream;
		myostringstream << val;
		return myostringstream.str();
	}

	void commonConstructor(cl_platform_id platform_id, cl_device_id device);
	CLWarpper(int gpu);
	CLWarpper();
	CLWarpper(cl_platform_id platformId, cl_device_id deviceId);

	virtual ~CLWarpper();
	int m_gpuIndex;
	static int roundUp(int quantization, int minimum);

	static int getPower2Upperbound(int value);// eg pass in 320, it will return: 512
	//I would like to choose gpu,so ignore other device
	static std::shared_ptr<CLWarpper> createForFirstGpu();
	static std::shared_ptr<CLWarpper> createForIndexedGpu(int gpu);

	static std::shared_ptr<CLWarpper> createForPlatformDeviceIndexes(int platformIndex, int deviceIndex);
	static std::shared_ptr<CLWarpper> createForPlatformDeviceIds(cl_platform_id platformId, cl_device_id deviceId);

	static std::string errorMessage(cl_int error);
	static void checkError(cl_int error);

	void gpu(int gpuIndex);
	void init(int gpuIndex);
	void finish();

	int getComputeUnits();
	int getLocalMemorySize();
	int getLocalMemorySizeKB();
	int getMaxWorkgroupSize();
	int getMaxAllocSizeMB();
	int getGlobalSizeMB();
	bool getSVMcapabilities();

	std::shared_ptr<CLProgram> buildProgramFromFile(const char *filename, std::string options);
	//void* CreateSVMbuffer(uint32_t size, bool readOnly);
	cl_mem Createbuffer(size_t size, cl_mem_flags flags);
	cl_int CopyBuffer(cl_mem clBuff, void* cpBuff, size_t size, bool cl2cp);

	int convertToString(const char *filename, std::string& s);
private:
	static int instance_count;
	static std::string getFileContents(std::string filename);
public:
	int64_t getDeviceInfoInt64(cl_device_info name);
	int getDeviceTopology(cl_device_topology_amd *topo);
};






void printPlatformInfoString(std::string valuename, cl_platform_id platformId, cl_platform_info name);
void printPlatformInfo(std::string valuename, cl_platform_id platformId, cl_platform_info name);
std::string getPlatformInfoString(cl_platform_id platformId, cl_platform_info name);


//static std::shared_ptr<CLWarpper> cl = CLWarpper::c/*reateForIndexedGpu(0);
//std::shared_ptr<CLWarpper> getCLWarpper()
//{
//	return cl;
//}*/

#endif
