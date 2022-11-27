#include "pca9685.h"
#include <wiringPi.h>
#include <wiringPiI2C.h>
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

    PCA9685 pca9685;
    pca9685.setPwmFreq(50);
	for (int i = 0; i < 6; i++) pwmWrite(PCA9685_PIN_BASE + i, 4000);
    for (;;);
    return 0;
}
