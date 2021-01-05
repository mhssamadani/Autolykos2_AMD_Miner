# ergoAMDminer
OpenCL miner for [ergoplatform.com](https://github.com/ergoplatform)

You can find CUDA miner at:  [CUDA miner](https://github.com/mhssamadani/Autolykos2_NV_Miner)
# Very Important Notice
*Due to ERGO's specification (unlike many other coins), it is __NOT SAFE__ to mine using any closed source miner; such a miner could steal your key easily.*
<blockquote class="twitter-tweet"><p lang="en" dir="ltr">Please be aware of closed-source miners like the following, such a miner could steal your key easily: <a href="https://t.co/Y93wqb2IkA">https://t.co/Y93wqb2IkA</a></p>&mdash; Alex Chepurnoy (@chepurnoy) <a href="https://twitter.com/chepurnoy/status/1155901160915701760?ref_src=twsrc%5Etfw">July 29, 2019</a></blockquote>

# Configuration
These files must be in the same folder as Executable file:
  * Miningkernel.cl
  * Prehashkernel.cl
  * Ocldecs.h
  * Ocldefs.h
  * Libcurl.dll( in windows)::: There is a libcurl.dll file in the win64 folder, use it or better replace it with your own built file
  * Config.json
  
 Change the config.json file with __your node's address. 
  
 # Requirements (Linux)
 
  1- Installing AMDGPU PRO driver for Ubuntu.
  
  Download AMDGPU-PRO package for your Linux distribution from amd.com, upack the downloaded file and run: 
  
     $ amdgpu-pro-install --opencl=legacy,pal --headless
  
  2 - Opencl headers
  
    $ sudo apt-get install opencl-headers
  
  3 - libcurl library: to install run
  
    $ apt install libcurl4-openssl-dev
  
  4- OpenSSL 1.0.2 library: to install run
  
    $ apt install libssl-dev
  
 # Requirements (Windows)
 
 1- AMD graphic card with driver installed
 
 2 - Download and Install [AMD APK](amd-dev.wpengine.netdna-cdn.com/app-sdk/installers/APPSDKInstaller/3.0.130.135-GA/full/AMD-APP-SDKInstaller-v3.0.130.135-GA-windows-F-x64.exe)
 
 3 - Build libcurl from sources with Visual Studio [toolchain instruction](https://medium.com/@chuy.max/compile-libcurl-on-windows-with-visual-studio-2017-x64-and-ssl-winssl-cff41ac7971d) 
 
 4 - Download OpenSSL 1.0.2 [installer from slproweb.com](https://slproweb.com/download/Win64OpenSSL-1_0_2t.exe)
 
 5 - Download and install [Visual C++ redistributable](https://aka.ms/vs/16/release/vc_redist.x64.exe)
 
 # Build (Linux)
 
 
1- Change directory to Ubuntu
 
2- Run `make` 

3- If make completed successfully there will appear an executable **Autolykos2_AMD_Miner/Ubuntu/ErgoOpenCL** that can start with **./ErgoOpenCL** 
 
 
# Build (Windows)
using visual studio

add OpenCL , LibCurl , OpenSSl libreries

Include Directories:

![](https://github.com/mhssamadani/Autolykos2_AMD_Miner/blob/master/img/includeDir.png)


Additional Include Directories:

![](https://github.com/mhssamadani/Autolykos2_AMD_Miner/blob/master/img/AddInc.png)



Additional Library Directories:


![](https://github.com/mhssamadani/Autolykos2_AMD_Miner/blob/master/img/AddLib.png)



Additional Dependencies:


![](https://github.com/mhssamadani/Autolykos2_AMD_Miner/blob/master/img/AddDep.png)



# Http info
Miner has a HTTP info page located at `http://miningnode:36207` in json format which shows status of gpus (hashrate, fan speed, temp, ...)

# Donations and Support

Note that the miner is free to use and we do not charge any fee from what you mine.
To support all the work we're doing, we welcome donations from ERGO miners!

Bitcoin: 3KkwygpCLs1oEi9aTozFxYunoASV6ZrykJ

Bitcoin: bc1q7flay376e5mcp4ljjxpdp7r6p8yajcjm5mu6wd

ERGO: 9gZPigzM9ieRaCrzRC7YZ4ECFrKmrKuazUoT5U6fcnVoY6YbHN4
