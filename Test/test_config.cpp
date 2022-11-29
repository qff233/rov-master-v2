#include <iostream>
#include "User/config.h"
#include <nlohmann/json.hpp>

int main() {
    Config config;

    config.load_from_file("config.json");
    std::cout << config.dump() << std::endl;

    auto a = config.getValue("abc", std::string("bcd"), [](const Json& old_value,const Json& new_value) {
        std::cout << "in_call_back" << new_value.get<std::string>() << std::endl;
    });
    std::cout << a << std::endl;
    std::cout << config.dump() << std::endl;

    config.setValue("abc", std::string("sdasdasd"));
    return 0;
}
