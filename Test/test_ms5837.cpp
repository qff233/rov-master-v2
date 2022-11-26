#include "ms5837.h"
#include <wiringPi.h>
#include <wiringSerial.h>
#include <iostream>
#include <elog.h>

int main() {
    
    // 初始化 EasyLogger
    elog_init();
    /* 设置 EasyLogger 日志格式 */
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);

    elog_set_text_color_enabled(true);
    elog_start();
    wiringPiSetup();
    MS5837 ms5837;
    int fd = ms5837.getFd();
    std::cout << "start" << std::endl;
    if(fd < 0) {
        std::cout << "error" << std::endl;
        return -1;
    }
	
    for(int i = 0; i < 100; ++i) {
	delay(50);
        while(serialDataAvail(fd)) {
            uint8_t data = serialGetchar(fd);
            if(!ms5837.inputData(data)) break;
	    // std::cout << "fd" << std::endl;
        }
   	std::cout << "test.cpp：" << ms5837.getData().temperature << std::endl;
    }
    return 0;
}
