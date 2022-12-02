#ifndef __USER_MACRO_H__
#define __USER_MACRO_H__

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define rad2Deg(rad) (rad * 180.0f / M_PI)
#define deg2Rad(deg) (deg * M_PI / 180.0f)
#define ADD_METHOD(name, ...) \
    Global<RPCServer>::Get()->addMethod(#name, jsonrpccxx::GetHandle(Method::name), {__VA_ARGS__})

#define BITS(x) (1 << x)

#endif