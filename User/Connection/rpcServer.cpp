//
// Created by fxf on 22-11-21.
//

#define LOG_TAG "RPC"

#include <elog.h>

#include "rpcServer.h"

using namespace std;

RPCServer::RPCServer(const std::string &address, uint16_t port)
{
    server = new Json::Rpc::TcpServer(address, port);
    if(!networking::init())
    {
        log_e("Networking initialization failed");
        exit(EXIT_FAILURE);
    }
    infoInit();
    controlInit();
    debugInit();
}
