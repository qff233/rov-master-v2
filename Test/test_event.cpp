#include <wiringPi.h>
#include <wiringSerial.h>
#include <iostream>
#include <elog.h>

#include "User/Event/sensor_jy901.h"
#include "User/Event/sensor_ms5837.h"
#include "User/Event/event_manager.h"

int main() {
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

    log_i("start");

    EventManager eventMgr("sensor");
    Event::ptr jy901_event = std::make_unique<EventJY901>();
    Event::ptr ms5837_event = std::make_unique<EventMS5837>();

    eventMgr.addEvent(std::move(jy901_event));
    eventMgr.addEvent(std::move(ms5837_event));

    eventMgr.run();
    return 0;
}
