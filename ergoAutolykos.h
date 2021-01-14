#pragma once

/*******************************************************************************

	AUTOLYKOS -- Autolykos puzzle cycle

*******************************************************************************/
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#endif

//#include "bip39/include/bip39/bip39.h"
#include "httpapi.h"

#include "cryptography.h"
#include "definitions.h"
#include "easylogging++.h"
#include "jsmn.h"
#include "clMining.h"
#include "clPreHash.h"
#include "processing.h"
#include "request.h"
#include <ctype.h>
#include <curl/curl.h>
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
#include <vector>
#include <random>
#include "queue.h"


#ifdef _WIN32
#include <io.h>
#define R_OK 4       
#define W_OK 2       
#define F_OK 0       
#define access _access
#else
#include <unistd.h>
#endif
//
//INITIALIZE_EASYLOGGINGPP
//
//using namespace std::chrono;
using namespace std;
namespace ch = std::chrono;
using namespace std::chrono;
class ergoAutolykos
{
private:
	CLWarpper **clw;
	
public:
	ergoAutolykos();
	~ergoAutolykos();


	////////////////////////////////////////////////////////////////////////////////
	//  Miner thread cycle
	////////////////////////////////////////////////////////////////////////////////
	static void MinerThread(CLWarpper *oclWrapper,const int deviceId, const int totalGPUCards, info_t * info, std::vector<double>* hashrates, std::vector<int>* tstamps, BlockQueue<MinerShare>* shQueue);
	////////////////////////////////////////////////////////////////////////////////
	static void SenderThread(info_t * info, BlockQueue<MinerShare>* shQueue);
	////////////////////////////////////////////////////////////////////////////////

//  
////////////////////////////////////////////////////////////////////////////////
	int startAutolykos(int argc, char ** argv);

};

