#include "l_comms.h"
#include "MES.h"

int main(int argc, char *argv[])
{
    // Initialize Logger
    Log::init();
    // Entry point to start MES
    MES Mes;
    Mes.start();
}
