#ifndef __CONNECTION_PROTOCOL_JSONRPC_H__
#define __CONNECTION_PROTOCOL_JSONRPC_H__

#include <vector>
#include <memory>
#include <jsoncpp/json.hpp>

using Json = nlohmann::json;

/*  (-32700)	        Parse error语法解析错误	服务端接收到无效的json。该错误发送于服务器尝试解析json文本
    (-32600)	        Invalid Request无效请求	发送的json不是一个有效的请求对象。
    (-32601)	        Method not found找不到方法	该方法不存在或无效
    (-32602)        	Invalid params无效的参数	无效的方法参数。
    (-32603)	        Internal error内部错误	JSON-RPC内部错误。
    (-32000 to -32099)  Server error服务端错误	预留用于自定义的服务器错误。 
*/

class JsonRpc {
public:
    using JsonPtr = std::unique_ptr<Json>;
    enum class ErrorCode 
    {
        OK             = 0,
        ParseError     = -32700,
        InvalidRequest = -32600,
        MethodNotFound = -32601,
        InvalidParams  = -32602,
        InternalError  = -32603,
    };
    struct Version
    {
        int mainVersion;
        int subVersion;
    };

    JsonRpc(Version version = {2, 0});
    void parse(const std::string& json_str);
private:
    Version m_version;
    int m_id;
    std::string m_method;
    std::vector<JsonPtr> m_params;
}

#endif 