

#include"cl_warpper.h"

#include "OCLdefs.h"

#include <stdio.h>
#include <string.h>
#include "easylogging++.h"
int CLWarpper::instance_count = 0;
static int _gpus = 0;



CLWarpper::CLWarpper(int gpu) {
	init(gpu);
	if (instance_count == 0)
	{
		instance_count++;
	}
}

CLWarpper::CLWarpper() {
	init(0);
	if (instance_count == 0)
	{
		instance_count++;
	}
}

CLWarpper::CLWarpper(cl_platform_id platform_id, cl_device_id device) {
	commonConstructor(platform_id, device);
	if (instance_count == 0)
	{
		instance_count++;
	}
}

CLWarpper::~CLWarpper() {
	instance_count--;
	if (instance_count == 0)
	{
	}
	if (queue != 0) {
		clReleaseCommandQueue(*queue);
		delete queue;
	}
	if (context != 0) {
		clReleaseContext(*context);
		delete context;
	}
}
void CLWarpper::init(int gpuIndex) {

	error = 0;
	queue = 0;
	context = 0;


    int i, j;
    char* value;
    size_t valueSize;
    cl_uint platformCount;
    cl_platform_id* platforms;
    cl_uint deviceCount;
    cl_device_id* device_ids;
    cl_uint maxComputeUnits;

    clGetPlatformIDs(0, NULL, &platformCount);
    platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * platformCount);
    clGetPlatformIDs(platformCount, platforms, NULL);


    // get all devices
    clGetDeviceIDs(platforms[gpuIndex], CL_DEVICE_TYPE_ALL, 0, NULL, &deviceCount);
    device_ids = (cl_device_id*) malloc(sizeof(cl_device_id) * deviceCount);
    clGetDeviceIDs(platforms[gpuIndex], CL_DEVICE_TYPE_ALL, deviceCount, device_ids, NULL);

	device = device_ids[0];
	//delete[] device_ids;


	// Context
	context = new cl_context();
	*context = clCreateContext(0, 1, &device, NULL, NULL, &error);
	if (error != CL_SUCCESS) {
		throw std::runtime_error("Error creating OpenCL context, OpenCL errorcode: " + errorMessage(error));
	}
	// Command-queue
	queue = new cl_command_queue;
	*queue = clCreateCommandQueue(*context, device, 0, &error);
	if (error != CL_SUCCESS) {
		throw std::runtime_error("Error creating OpenCL command queue, OpenCL errorcode: " + errorMessage(error));
	}

	m_gpuIndex = gpuIndex;
}
void CLWarpper::commonConstructor(cl_platform_id platform_id, cl_device_id device)
{
	queue = 0;
	context = 0;

	this->platform_id = platform_id;
	this->device = device;

	// Context
	context = new cl_context();
	*context = clCreateContext(0, 1, &device, NULL, NULL, &error);
	if (error != CL_SUCCESS) {
		//throw std::runtime_error("Error creating OpenCL context, OpenCL errocode: " + errorMessage(error));
		std::cout << "\n Error creating OpenCL context";
		m_gpuIndex = -1;
	}
	// Command-queue
	queue = new cl_command_queue;
	*queue = clCreateCommandQueue(*context, device, 0, &error);
	if (error != CL_SUCCESS) {

		std::cout << "\n Error creating OpenCL command queue";
		//throw std::runtime_error("Error creating OpenCL command queue, OpenCL errorcode: " + errorMessage(error));
		m_gpuIndex = -1;
	}
	m_gpuIndex = _gpus++;
}
int CLWarpper::roundUp(int quantization, int minimum) {
	return ((minimum + quantization - 1) / quantization * quantization);
}
int CLWarpper::getPower2Upperbound(int value) {
	int upperbound = 1;
	while (upperbound < value) {
		upperbound <<= 1;
	}
	return upperbound;
}

std::shared_ptr<CLWarpper> CLWarpper::createForIndexedGpu(int gpu) {
	cl_int error;
	int currentGpuIndex = 0;
	cl_platform_id platform_ids[10];
	cl_uint num_platforms;
	error = clGetPlatformIDs(10, platform_ids, &num_platforms);
	if (error != CL_SUCCESS) {
		throw std::runtime_error("Error getting OpenCL platforms ids, OpenCL errorcode: " + errorMessage(error));
	}
	if (num_platforms == 0) {
		throw std::runtime_error("Error: no OpenCL platforms available");
	}
	for (int platform = 0; platform < (int)num_platforms; platform++) {
		cl_platform_id platform_id = platform_ids[platform];

		cl_device_id device_ids[100];
		cl_uint num_devices;
		error = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_ACCELERATOR, 100, device_ids, &num_devices);
		if (error != CL_SUCCESS) {
			continue;
		}

		if ((gpu - currentGpuIndex) < (int)num_devices) {
			return std::shared_ptr<CLWarpper>(new CLWarpper(platform_id, device_ids[(gpu - currentGpuIndex)]));
		}
		else {
			currentGpuIndex += num_devices;
		}
	}
	if (gpu == 0) {
		throw std::runtime_error("No OpenCL-enabled GPUs found");
	}
	else {
		throw std::runtime_error("Not enough OpenCL-enabled GPUs found to satisfy gpu index: " + toString(gpu));
	}
}
std::shared_ptr<CLWarpper> CLWarpper::createForFirstGpu() {
	return createForIndexedGpu(0);
}
std::shared_ptr<CLWarpper> CLWarpper::createForPlatformDeviceIds(cl_platform_id platformId, cl_device_id deviceId) {
	return std::shared_ptr<CLWarpper>(new CLWarpper(platformId, deviceId));
}
std::shared_ptr<CLWarpper> CLWarpper::createForPlatformDeviceIndexes(int platformIndex, int deviceIndex) {
	cl_int error;
	cl_platform_id platform_ids[10];
	cl_uint num_platforms;
	error = clGetPlatformIDs(10, platform_ids, &num_platforms);
	if (error != CL_SUCCESS) {
		throw std::runtime_error("Error getting OpenCL platforms ids, OpenCL errorcode: " + errorMessage(error));
	}
	if (num_platforms == 0) {
		throw std::runtime_error("Error: no OpenCL platforms available");
	}
	if (platformIndex >= (int)num_platforms) {
		throw std::runtime_error("Error: OpenCL platform index " + toString(platformIndex) + " not available. There are only: " + toString(num_platforms) + " platforms available");
	}
	cl_platform_id platform_id = platform_ids[platformIndex];
	cl_device_id device_ids[100];
	cl_uint num_devices;
	error = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ALL, 100, device_ids, &num_devices);
	if (error != CL_SUCCESS) {
		throw std::runtime_error("Error getting OpenCL device ids for platform index " + toString(platformIndex) + ": OpenCL errorcode: " + errorMessage(error));
	}
	if (num_devices == 0) {
		throw std::runtime_error("Error: no OpenCL devices available for platform index " + toString(platformIndex));
	}
	if (deviceIndex >= (int)num_devices) {
		throw std::runtime_error("Error: OpenCL device index " + toString(deviceIndex) + " goes beyond the available devices on platform index " + toString(platformIndex) + ", which has " + toString(num_devices) + " devices");
	}
	return std::shared_ptr<CLWarpper>(new CLWarpper(platform_id, device_ids[deviceIndex]));
}

std::string CLWarpper::errorMessage(cl_int error) {
	return toString(error);
}
void CLWarpper::checkError(cl_int error) {
	if (error != CL_SUCCESS) {
		std::string message = toString(error);
		switch (error) {
		case CL_MEM_OBJECT_ALLOCATION_FAILURE:
			message = "CL_MEM_OBJECT_ALLOCATION_FAILURE";
			break;
		case CL_INVALID_ARG_SIZE:
			message = "CL_INVALID_ARG_SIZE";
			break;
		case CL_INVALID_BUFFER_SIZE:
			message = "CL_INVALID_BUFFER_SIZE";
			break;
		case CL_BUILD_PROGRAM_FAILURE:
			message = "CL_BUILD_PROGRAM_FAILURE";
		break;
		}
		std::cout << "opencl execution error, code " << error << " " << message << std::endl;
 		//throw std::runtime_error(std::string("OpenCL error, code: ") + message);

	}
}

std::string CLWarpper::getFileContents(std::string filename) {
	std::ifstream t(filename.c_str());
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}
void CLWarpper::gpu(int gpuIndex) {
	finish();
	if (queue != 0) {
		clReleaseCommandQueue(*queue);
		delete queue;
	}
	if (context != 0) {
		clReleaseContext(*context);
		delete context;
	}

	init(gpuIndex);
}
void CLWarpper::finish() {
	error = clFinish(*queue);
	switch (error) {
	case CL_SUCCESS:
		break;
	case -36:
		throw std::runtime_error("Invalid command queue: often indicates out of bounds memory access within kernel");
	default:
		checkError(error);
	}
}

int CLWarpper::getComputeUnits() {
	return (int)this->getDeviceInfoInt64(CL_DEVICE_MAX_COMPUTE_UNITS);
}
int CLWarpper::getLocalMemorySize() {
	return (int)this->getDeviceInfoInt64(CL_DEVICE_LOCAL_MEM_SIZE);
}
int CLWarpper::getLocalMemorySizeKB() {
	return (int)(this->getDeviceInfoInt64(CL_DEVICE_LOCAL_MEM_SIZE) / 1024);
}
int CLWarpper::getMaxWorkgroupSize() {
	return (int)this->getDeviceInfoInt64(CL_DEVICE_MAX_WORK_GROUP_SIZE);
}
int CLWarpper::getMaxAllocSizeMB() {
	return (int)(this->getDeviceInfoInt64(CL_DEVICE_MAX_MEM_ALLOC_SIZE) / 1024 / 1024);
}
int CLWarpper::getGlobalSizeMB() {
	return (int)(this->getDeviceInfoInt64(CL_DEVICE_GLOBAL_MEM_SIZE) / 1024 / 1024);
}
bool CLWarpper::getSVMcapabilities()
{

	cl_device_svm_capabilities caps;

	cl_int err = clGetDeviceInfo(
		device,
		CL_DEVICE_SVM_CAPABILITIES,
		sizeof(cl_device_svm_capabilities),
		&caps,
		0
		);

	if (caps & (CL_DEVICE_SVM_COARSE_GRAIN_BUFFER | CL_DEVICE_SVM_FINE_GRAIN_BUFFER | CL_DEVICE_SVM_FINE_GRAIN_SYSTEM | CL_DEVICE_SVM_ATOMICS))
	{
		return true;
	}
	return false;

}
/* convert the kernel file into a string */
int CLWarpper::convertToString(const char *filename, std::string& s)
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
//	cout << "Error: failed to open file\n:" << filename << endl;
	return -1;
}
cl_mem CLWarpper::Createbuffer(size_t size, cl_mem_flags flags)
{
	cl_mem buff = clCreateBuffer(*context, flags, size, NULL, NULL);
	return buff;
}
cl_int CLWarpper::CopyBuffer(cl_mem clBuff, void* cpBuff , size_t size, bool cl2cp)
{
	cl_int err = 0;

	if (cl2cp)
	{
		err = clEnqueueReadBuffer(*queue, clBuff, CL_TRUE, 0, size, cpBuff, 0, NULL, NULL);
		if (err != CL_SUCCESS)
		{
			printf("Error: Failed to read output array! %d\n", err);
			//exit(1);
		}

	}
	else
	{
		err = clEnqueueWriteBuffer(*queue, clBuff, CL_TRUE, 0, size, cpBuff, 0, NULL, NULL);
		if (err != CL_SUCCESS)
		{
			printf("Error: Failed to write to source array!\n");
			//exit(1);
		}

	}
	return err;
}

//void* CLWarpper::CreateSVMbuffer(uint32_t size , bool readOnly )
//{
//
//	cl_int error;
//	cl_mem_flags flags = CL_MEM_SVM_FINE_GRAIN_BUFFER;
//	if (readOnly) {
//		flags |= CL_MEM_READ_ONLY;
//	}
//	else {
//		flags |= CL_MEM_READ_WRITE;
//	}
//
//	size_t usize = (size_t)size;
//
//	void* buff =
//		(void*)clSVMAlloc(
//			*context,                // the context where this memory is supposed to be used
//			flags,
//			usize,     // amount of memory to allocate (in bytes)
//			sizeof(cl_uint)/*0*/                       // alignment in bytes (0 means default)
//			);
//	return buff;
//
//
//}
std::shared_ptr<CLProgram> CLWarpper::buildProgramFromFile(const char *filename, std::string options)
{

	size_t src_size = 0;

	std::string sourceStr;
	int status = convertToString(filename, sourceStr);
	const char *source = sourceStr.c_str();

	size_t sourceSize2[] = { strlen(source) };
	cl_int err2;

	cl_program program2 = clCreateProgramWithSource(*context, 1, &source, sourceSize2, &error);
	checkError(error);


	error = clBuildProgram(program2, 1, &device, options.c_str(), NULL, NULL);
	checkError(error);

	char* build_log;
	size_t log_size;
	error = clGetProgramBuildInfo(program2, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
	checkError(error);
	build_log = new char[log_size + 1];
	error = clGetProgramBuildInfo(program2, device, CL_PROGRAM_BUILD_LOG, log_size, build_log, NULL);
	checkError(error);
	build_log[log_size] = '\0';
	std::string buildLogMessage(build_log);
	std::string  substr("Error");

	int find = buildLogMessage.find(substr);
	if (find > -1 ) {
		buildLogMessage = build_log;
		std::cout << buildLogMessage << std::endl;
		LOG(ERROR) << buildLogMessage; 
	}
	delete[] build_log;
	checkError(error);
	std::shared_ptr<CLProgram> res(new CLProgram(program2));
	return res;
}

int64_t CLWarpper::getDeviceInfoInt64(cl_device_info name) {

	cl_ulong value = 0;
	clGetDeviceInfo(device, name, sizeof(cl_ulong), &value, 0);
	return static_cast<int64_t>(value);
}

int CLWarpper::getDeviceTopology(cl_device_topology_amd *topo) {


	int status = clGetDeviceInfo(device, CL_DEVICE_TOPOLOGY_AMD, sizeof(cl_device_topology_amd), topo, 0);
	return status;
}


void printPlatformInfoString(std::string valuename, cl_platform_id platformId, cl_platform_info name)
{
	char buffer[256];
	buffer[0] = 0;
	clGetPlatformInfo(platformId, name, 256, buffer, 0);
	std::cout << valuename << ": " << buffer << std::endl;
}
std::string getPlatformInfoString(cl_platform_id platformId, cl_platform_info name) {
	char buffer[257];
	buffer[0] = 0;
	size_t namesize;
	cl_int error = clGetPlatformInfo(platformId, name, 256, buffer, &namesize);
	if (error != CL_SUCCESS) {
		if (error == CL_INVALID_PLATFORM) {
			throw std::runtime_error("Failed to obtain platform info for platform id " + CLWarpper::toString(platformId) + ": invalid platform");
		}
		else if (error == CL_INVALID_VALUE) {
			throw std::runtime_error("Failed to obtain platform info " + CLWarpper::toString(name) + " for platform id " + CLWarpper::toString(platformId) + ": invalid value");
		}
		else {
			throw std::runtime_error("Failed to obtain platform info " + CLWarpper::toString(name) + " for platform id " + CLWarpper::toString(platformId) + ": unknown error code: " + CLWarpper::toString(error));
		}
	}
	return std::string(buffer);
}
void printPlatformInfo(std::string valuename, cl_platform_id platformId, cl_platform_info name) {
	cl_ulong somelong = 0;
	clGetPlatformInfo(platformId, name, sizeof(cl_ulong), &somelong, 0);
	std::cout << valuename << ": " << somelong << std::endl;
}

CLProgram::CLProgram(cl_program prog) :program(prog) {}
cl_kernel CLProgram::getKernel(std::string kernelname)
{
	auto iter = kernels.find(kernelname);
	if (iter == kernels.end())
	{
		cl_int error;
		cl_kernel kernel = clCreateKernel(program, kernelname.c_str(), &error);
		if (error != CL_SUCCESS) {
			std::string exceptionMessage = "";
			switch (error) {
			case -46:
				exceptionMessage = "Invalid kernel name, code -46, kernel " + kernelname + "\n";
				break;
			default:
				exceptionMessage = "Something went wrong with clCreateKernel, OpenCL error code " + CLWarpper::toString(error) + "\n";
				break;
			}

			std::cout << "kernel build error:\n" << exceptionMessage << std::endl;
			//throw std::runtime_error(exceptionMessage);
		}
		kernels[kernelname] = kernel;
		return kernel;
	}
	else
		return iter->second;
}
CLProgram::~CLProgram()
{
	for (auto iter = kernels.begin(); iter != kernels.end(); iter++)
		clReleaseKernel(iter->second);
	clReleaseProgram(program);
}

