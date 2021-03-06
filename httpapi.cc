#include "httpapi.h"

using namespace httplib;
#ifndef __linux__
#define __withADL__
#endif

inline int key(std::pair<int,int> x)
{
    return 100*x.first + x.second;
}


// outputs JSON with GPUs hashrates, temps, and power usages
#include "adl.h"
#include "easylogging++.h"
#include "sysfs-gpu-controls.h"

void InitializeADL(int minerCount, CLWarpper **clw)
{
#ifdef __withADL__
	initializeAdapters();
	cl_device_topology_amd topo;

	for (size_t j = 0; j < minerCount; j++)
	{
		clw[j]->getDeviceTopology(&topo);

		for (size_t i = 0; i < GPU_Lists.size(); i++)
		{
			if (topo.pcie.bus == GPU_Lists[i].ADL_AapterInfo.iBusNumber)
			{
				GPU_Lists[i].GPU_Index = j;
			}
		}
	}
#else
	cl_device_topology_amd topo;

	for (size_t j = 0; j < minerCount; j++)
	{
		int status = clw[j]->getDeviceTopology(&topo);

	      memset(gpusList[j].sysfs_info.pcie_index, 0xff, sizeof(gpusList[j].sysfs_info.pcie_index));
	      if (status == CL_SUCCESS && topo.raw.type == CL_DEVICE_TOPOLOGY_TYPE_PCIE_AMD)
	      {
	        uint8_t *pcie_index = gpusList[j].sysfs_info.pcie_index;
	        pcie_index[0] = topo.pcie.bus;
	        pcie_index[1] = topo.pcie.device;
	        pcie_index[2] = topo.pcie.function;
	      }

	}
	init_sysfs_hwcontrols(minerCount);
#endif
}



void HttpApiThread(std::vector<double>* hashrates, CLWarpper **clw,int minerCount)
{

	
//	 float temp,power;
//	 int iBusNumber, fanspeed, fanpercent;
    std::chrono::time_point<std::chrono::system_clock> timeStart;
    timeStart = std::chrono::system_clock::now();
    
    Server svr;

 	svr.Get("/", [&](const Request& req, Response& res) {


#ifdef __withADL__
		bool ret = GetAdlInfo();
#endif
        for(int gpuIndex = 0; gpuIndex < minerCount; gpuIndex++)
        {

#ifdef __withADL__


			for (size_t k = 0; k < GPU_Lists.size(); k++)
			{
				if (GPU_Lists[k].GPU_Index == k)
				{
					memcpy(&(gpusList[gpuIndex].adl_info),&(GPU_Lists[k]),sizeof(ADLInfo));
				}
			}


//			iBusNumber = gpuInfo.ADL_AapterInfo.iBusNumber;
//			temp = gpuInfo.ADLODN_Temperature;
//			power = 0;
//			fanspeed = 0;
//			fanpercent = 0;

#else
			bool ret = sysfs_gpu_stats(gpuIndex,&gpusList[gpuIndex].sysfs_info.LastTemp,&gpusList[gpuIndex].sysfs_info.LastPower,&gpusList[gpuIndex].sysfs_info.LastFanSpeed,&gpusList[gpuIndex].sysfs_info.LastFanPercent);


//			iBusNumber = gpusList[gpuIndex].sysfs_info.pcie_index[0];
//			temp = gpusList[gpuIndex].sysfs_info.LastTemp;
//			power = gpusList[gpuIndex].sysfs_info.LastPower;
//			fanspeed = gpusList[gpuIndex].sysfs_info.LastFanSpeed;
//			fanpercent = gpusList[gpuIndex].sysfs_info.LastFanPercent;
#endif
        }

 		std::unordered_map<int, double> hrMap;
        std::stringstream strBuf;
        strBuf << "{ ";


            strBuf << " \"hs\" : [ " ;
            for(int i = 0; i < minerCount; i++)
            {
            	strBuf <<   (*hashrates)[i];
				if(i != minerCount-1 ) strBuf << " , ";
            }
            strBuf << " ] , ";
            strBuf << " \"hs_units\" : \"mhs\" , " ;

            strBuf << " \"temp\" : [ " ;
            for(int i = 0; i < minerCount; i++)
            {
#ifdef __withADL__

            	strBuf <<   gpusList[i].adl_info.ADLODN_Temperature;
#else
            	strBuf <<   (int)gpusList[i].sysfs_info.LastTemp;
#endif
				if(i != minerCount-1 ) strBuf << " , ";
            }
            strBuf << " ] , ";

            strBuf << " \"fan\" : [ " ;
            for(int i = 0; i < minerCount; i++)
            {
#ifdef __withADL__

            	strBuf <<   gpusList[i].adl_info.ADLODN_FanPercent;
#else
            	strBuf <<   (int)gpusList[i].sysfs_info.LastFanPercent;
#endif
				if(i != minerCount-1 ) strBuf << " , ";
            }
            strBuf << " ] , ";

            std::chrono::time_point<std::chrono::system_clock> timeEnd;
            timeEnd = std::chrono::system_clock::now();
            strBuf << " \"uptime\": " << std::chrono::duration_cast<std::chrono::seconds>(timeEnd - timeStart).count() << " , ";

            float ver = 1.1;
            strBuf << " \"ver\": \"2.0\" , ";

            strBuf << " \"ar\" : [ " ;
            for(int i = 0; i < minerCount; i++)
            {
            	strBuf <<   1;
				if(i != minerCount-1 ) strBuf << " , ";
            }
            strBuf << " ] , ";

            strBuf << " \"algo\": \"AUTOLYKOS\"  , ";

            strBuf << " \"bus_numbers\" : [ " ;
            for(int i = 0; i < minerCount; i++)
            {
#ifdef __withADL__

            	strBuf <<   gpusList[i].adl_info.ADL_AapterInfo.iBusNumber;
#else
            	strBuf <<   (int)gpusList[i].sysfs_info.pcie_index[0];
#endif
				if(i != minerCount-1 ) strBuf << " , ";
            }
            strBuf << " ] ";



        strBuf << " } ";


        std::string str = strBuf.str();
		//LOG(INFO) << str;
        res.set_content(str.c_str(), "text/plain");
    });

    #ifdef HTTPAPI_PORT
    svr.listen("0.0.0.0", HTTPAPI_PORT);
    #else
		svr.listen("0.0.0.0", 36207);
    #endif
		int tr;
		tr = 0;
}
