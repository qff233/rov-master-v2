#include "rpc_server.h"

RPCServer::RPCServer(const std::string& address, uint16_t port) noexcept 
    : m_address(address), m_port(port)
{
    m_httpServer.Post("/",
        [this](const httplib::Request &req, httplib::Response &res){
            this->PostAction(req, res);
        });
}

RPCServer::~RPCServer() noexcept 
{
    this->stop();
}

int RPCServer::start() noexcept 
{
    if(m_httpServer.is_running())
        return -1;
    m_thread = std::thread([this]() { this->m_httpServer.listen(this->m_address.c_str(), this->m_port); });
    return 0;
}

void RPCServer::stop() noexcept
{
    if(m_httpServer.is_running()) {
        m_httpServer.stop();
        m_thread.join();
    }
}

int RPCServer::addMethod(const std::string& method_name, jsonrpccxx::MethodHandle callback, const std::vector<std::string>& params) noexcept 
{
    m_rpcserver.Add(method_name, callback, params);
}

void RPCServer::PostAction(const httplib::Request &req, httplib::Response &res) 
{
    res.status = 200;
    res.set_content(m_rpcserver.HandleRequest(req.body), "application/json");
}

const std::string& RPCServer::getAddress() const noexcept 
{
    return m_address;
}

uint16_t RPCServer::getPort() const noexcept 
{
    return m_port;
}