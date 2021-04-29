#include "l_comms.h"
#include "MES.h"

int main(int argc, char *argv[])
{
    std::string opc_endpoint;
    if(argc < 2){
        opc_endpoint = "localhost:4840";
    }
    else{
        opc_endpoint = argv[1];
    }
    // Initialize Logger
    Log::init();
    // Entry point to start MES
    MES Mes(opc_endpoint);
    Mes.start();
}
