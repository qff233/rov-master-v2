#ifndef __USER_CONNECTION_RPCSERVER_H__
#define __USER_CONNECTION_RPCSERVER_H__

#include <string>
#include <thread>

#include "jsonrpccxx/typemapper.hpp"
#include "jsonrpccxx/server.hpp"
#include "cpp-httplib/httplib.h"

class RPCServer
{
public:
    RPCServer(const std::string& address = "0.0.0.0", uint16_t port = 8888) noexcept;
    ~RPCServer() noexcept;

    int start() noexcept;
    void stop() noexcept;

    void addMethod(const std::string& method_name, jsonrpccxx::MethodHandle callback, const std::vector<std::string>& params = {}) noexcept;

    const std::string& getAddress() const noexcept;
    uint16_t getPort() const noexcept;
private:
    void PostAction(const httplib::Request &req, httplib::Response &res);
private:
    std::string m_address;
    uint16_t m_port;
    jsonrpccxx::JsonRpc2Server m_rpcserver;
    httplib::Server m_httpServer;
    std::thread m_thread;
};

#endif //_RPCSERVER_H
