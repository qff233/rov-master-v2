//
// Created by fxf on 22-11-21.
//

#ifndef _RPCSERVER_H
#define _RPCSERVER_H

#include <string>
#include <jsonrpc.h>

#include "rpcApp.h"

#define RPC_SERVER_ADDRESS "127.0.0.1"
#define RPC_SERVER_PORT 8888

class RPCServer
{
public:
    RPCServer(std::string& address, uint16_t port = RPC_SERVER_PORT);

private:
    Json::Rpc::TcpServer* server;
    RPCAPP* rpcapp;

    void infoInit();
    void controlInit();
    void debugInit();
};

#endif //_RPCSERVER_H
