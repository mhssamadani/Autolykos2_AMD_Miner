# OpenCL Miner for Autolykos v2 (Ergo) for AMD GPUs

OpenCL miner for [ergoplatform.com](https://github.com/ergoplatform)

You can find CUDA miner at:  [CUDA miner](https://github.com/mhssamadani/Autolykos2_NV_Miner)

# Quick Pooled Mining Start
1. Download the [miner](https://github.com/mhssamadani/Autolykos2_AMD_Miner/releases) for desired OS.

2. Run the [ErgoStratumProxy](https://github.com/mhssamadani/ErgoStratumProxy/releases) executable (Bundled with the miner release)
- In Windows PowerShell:
```
.\ErgoStratumProxy.exe -s <POOL_ADDRESS> -p <POOL_PORT> -u <WORKER_NAME>
```
- In linux:
```
./ErgoStratumProxy_Linux -s <POOL_ADDRESS> -p <POOL_PORT> -u <WORKER_NAME>
```

3. Run the miner 

- If neccessary, edit `config.json`; set node address to the proxy's address (by default this address is: ```{"node":"http://127.0.0.1:3000"}```)
### HTTP Info

Miner has a HTTP info page located at `http://miningnode:36207` (one can change default port by adding `-DHTTPAPI_PORT XXXX` to Makefile).

It outputs total hashrate, and per-GPU hashrates, power usages and temperatures in JSON format (relies on NVML, can fail if NVML fails - if so, JSON contains error field).


# Build

  
 ## Requirements (Linux)
 
  1. Installing AMDGPU PRO driver for Ubuntu.
  
  Download AMDGPU-PRO package for your Linux distribution from amd.com, upack the downloaded file and run: 
  
     $ amdgpu-pro-install --opencl=legacy,pal --headless
  
  2. Opencl headers
  
    $ sudo apt-get install opencl-headers
  
  3. libcurl library: to install run
  
    $ apt install libcurl4-openssl-dev
  
  4. OpenSSL 1.0.2 library: to install run
  
    $ apt install libssl-dev
  
 ## Requirements (Windows)
 
 - AMD graphic card with driver installed
 
 - Download and Install [AMD APK](amd-dev.wpengine.netdna-cdn.com/app-sdk/installers/APPSDKInstaller/3.0.130.135-GA/full/AMD-APP-SDKInstaller-v3.0.130.135-GA-windows-F-x64.exe)
 
 - Build libcurl from sources with Visual Studio [toolchain instruction](https://medium.com/@chuy.max/compile-libcurl-on-windows-with-visual-studio-2017-x64-and-ssl-winssl-cff41ac7971d) 
 
 - Download OpenSSL 1.0.2 [installer from slproweb.com](https://slproweb.com/download/Win64OpenSSL-1_0_2t.exe)
 
 - Download and install [Visual C++ redistributable](https://aka.ms/vs/16/release/vc_redist.x64.exe)
 
 ## Build (Linux)
 
 
- Change directory to Ubuntu
 
- Run `make` 

- If make completed successfully there will appear an executable **Autolykos2_AMD_Miner/Ubuntu/ErgoOpenCL** that can start with **./ErgoOpenCL** 
 
 
## Build (Windows)

- using visual studio

- add OpenCL , LibCurl , OpenSSl libreries

- Include Directories:

![](https://raw.githubusercontent.com/mhssamadani/Autolykos2_AMD_Miner/main/img/includeDir.png)


- Additional Include Directories:

![](https://raw.githubusercontent.com/mhssamadani/Autolykos2_AMD_Miner/main/img/AddInc.png)



- Additional Library Directories:


![](https://raw.githubusercontent.com/mhssamadani/Autolykos2_AMD_Miner/main/img/AddLib.png)



- Additional Dependencies:

![](https://raw.githubusercontent.com/mhssamadani/Autolykos2_AMD_Miner/main/img/AddDep.png)

# Configuration
These files must be in the same folder as Executable file:
  * Miningkernel.cl
  * Prehashkernel.cl
  * Ocldecs.h
  * Ocldefs.h
  * Libcurl.dll( in windows)::: There is a libcurl.dll file in the win64 folder, use it or better replace it with your own built file
  * Config.json
  
 Change the config.json file with your node's address. 
 
 # Stratum Proxy

In order to use this miner with a stratum pool, a stratum proxy is needed.
- Download [Ergo Stratum Proxy](https://github.com/mhssamadani/ErgoStratumProxy)
- Run proxy
- In the miner's config file set node address to the proxy's address
 (by default this address is: ```{ "node" : "http://127.0.0.1:3000" }```)


# Donations and Support
Note that the miner is free to use and we do not charge any fee from what you mine. To support all the work we're doing, we welcome donations from ERGO miners!

Bitcoin: 3KkwygpCLs1oEi9aTozFxYunoASV6ZrykJ

Bitcoin: bc1q7flay376e5mcp4ljjxpdp7r6p8yajcjm5mu6wd

ERGO: 9fFUw6DqRuyFCv13nQyoDuDz4TiR4GvVvWRcSvqzs39eBVcb5S1
