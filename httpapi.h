#ifndef HTTPAPI_H
#define HTTPAPI_H

#include "httplib.h"
#include <vector>
#include <string>
//#include <nvml.h>
#include <unordered_map>
#include <sstream>
#include <chrono>

#include "cl_warpper.h"
void InitializeADL(int minerCount, CLWarpper **clw);
void HttpApiThread(std::vector<double>* hashrates, CLWarpper **clw, int minerCount);


#endif