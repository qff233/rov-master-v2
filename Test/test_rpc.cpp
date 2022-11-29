#include <wiringPi.h>
#include <wiringSerial.h>
#include <iostream>

#include "User/Connection/rpc_server.h"
#include "User/Connection/method_get_info.h"

int main() 
{
    wiringPiSetup();
    RPCServer rpc;
    rpc.addMethod("get_info", jsonrpccxx::GetHandle(Method::get_info));
    rpc.start();
    sleep(100000000);
    return 0;
}
