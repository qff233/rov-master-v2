#include <wiringPi.h>
#include <wiringSerial.h>
#include <iostream>

#include "User/Event/sensor_jy901.h"
#include "User/Event/sensor_ms5837.h"
#include "User/Event/event_manager.h"
#include "User/utils.h"

#include <thread>

int main() {
    Global<JY901>::New();
    Global<MS5837>::New();

    EventManager eventMgr("sensor");
    Event::ptr jy901_event = std::make_unique<EventJY901>();
    Event::ptr ms5837_event = std::make_unique<EventMS5837>();

    eventMgr.addEvent(std::move(jy901_event));
    eventMgr.addEvent(std::move(ms5837_event));

    std::thread th(&EventManager::run, &eventMgr);
    // eventMgr.run();
    while(true) {
	std::cout << Global<JY901>::Get()->getYaw() << std::endl;
    }

    Global<JY901>::Delete();
    Global<MS5837>::Delete();
    return 0;
}
