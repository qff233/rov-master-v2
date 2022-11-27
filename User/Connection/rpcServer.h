#ifndef __USER_CONNECTION_RPCSERVER_H__
#define __USER_CONNECTION_RPCSERVER_H__

#include <string>

#include "jsonrpccxx/server.hpp"

class RPCServer
{
public:
    RPCServer(const std::string& address = "127.0.0.1", uint16_t port = 8888);
private:
    jsonrpccxx::JsonRpc2Server m_rpcserver;
};

#endif //_RPCSERVER_H
