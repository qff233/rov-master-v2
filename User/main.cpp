#include <easylogging++.h>
#include "utils.h"
#include "macro.h"

#include "Drivers/jy901.h"
#include "Drivers/ms5837.h"
#include "Drivers/pca9685.h"

#include "Event/event.h"
#include "Event/event_manager.h"
#include "Event/sensor_jy901.h"
#include "Event/sensor_ms5837.h"
#include "Event/control_pca9685.h"

#include "Connection/rpc_server.h"
#include "Connection/method_move.h"
#include "Connection/method_exit.h"
#include "Connection/method_depth.h"
#include "Connection/method_catcher.h"
#include "Connection/method_get_info.h"
#include "Connection/method_get_feedbacks.h"
#include "Connection/method_update_firmware.h"
#include "Connection/method_save_parameters.h"
#include "Connection/method_load_parameters.h"
#include "Connection/method_set_depth_locked.h"
#include "Connection/method_set_direction_locked.h"
#include "Connection/method_set_propeller_values.h"
#include "Connection/method_set_debug_mode_enabled.h"
#include "Connection/method_set_propeller_parameters.h"
#include "Connection/method_set_control_loop_parameters.h"
#include "Connection/method_set_propeller_pwm_freq_calibration.h"

#include <unistd.h>


INITIALIZE_EASYLOGGINGPP

int main() 
{
    el::Loggers::configureFromGlobal("./logger.conf");

    Global<Config>::New();
    Global<JY901>::New();
    Global<MS5837>::New();
    Global<PCA9685>::New();
    Global<ControlBase>::New<ControlV2>();
    Global<EventManager>::New("Driver");
    Global<RPCServer>::New("0.0.0.0", 8888);

    Global<Config>::Get()->load_from_file("config.json");

/****************Drivers********************/
    using Type = EventManager::EventType;
    add_event<EventJY901>(Type::READ);
    add_event<EventMS5837>(Type::READ);
    add_event<EventPCA9685>(Type::WRITE);
    Global<EventManager>::Get()->start();

/****************RPC_Method*****************/
    ADD_METHOD(get_info);
    ADD_METHOD(move, "x", "y", "z", "rot");
    ADD_METHOD(set_depth_locked);
    ADD_METHOD(set_direction_locked);
    // ADD_METHOD(catch);
    Global<RPCServer>::Get()->addMethod("catch", jsonrpccxx::GetHandle(Method::catcher));  // 这个catch是关键字 做不了函数名 单拿出来 后面上位机改一下
    Global<RPCServer>::Get()->start();
/*******************************************/

    while (true)
    {
        sleep(1);
        LOG(INFO) << Global<JY901>::Get()->getZGyro();
    }
    
    Global<RPCServer>::Delete();
    Global<EventManager>::Delete();
    Global<ControlBase>::Delete();
    Global<JY901>::Delete();
    Global<MS5837>::Delete();
    Global<PCA9685>::Delete();
    Global<Config>::Delete();
    return 0;
}
