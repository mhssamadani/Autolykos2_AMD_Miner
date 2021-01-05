#include <iostream>
using namespace std;
#include "ergoAutolykos.h"
#include "ergoTester.h"

INITIALIZE_EASYLOGGINGPP
namespace ch = std::chrono;
//#define  _TEST_

#include "Autolykos.h"
int main(int argc, char* argv[])
{
#ifdef _TEST_
	ergoTester* eTester = new ergoTester();
	eTester->testErgo(argc, argv);

#else
	ergoAutolykos* Autolykos = new ergoAutolykos();
	Autolykos->startAutolykos(argc, argv);
#endif

    return 0;
}

