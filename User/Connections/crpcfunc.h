#ifndef __CRPCFUNC_H_
#define __CRPCFUNC_H_

#include "../user/config.h"
#include "jsonrpc.h"

cJSON *info(jrpc_context *ctx, cJSON *params, cJSON *id);
cJSON *debug_info(jrpc_context *ctx, cJSON *params, cJSON *id);

cJSON *move_asyn(jrpc_context *ctx, cJSON *params, cJSON *id);
cJSON *move_syn(jrpc_context *ctx, cJSON *params, cJSON *id);
cJSON *catcher(jrpc_context *ctx, cJSON *params, cJSON *id);
cJSON *depth(jrpc_context *ctx, cJSON *params, cJSON *id);
cJSON *move_absolute(jrpc_context *ctx, cJSON *params, cJSON *id);
cJSON *move_relative(jrpc_context *ctx, cJSON *params, cJSON *id);
cJSON *direction_lock(jrpc_context *ctx, cJSON *params, cJSON *id);
cJSON *depth_lock(jrpc_context *ctx, cJSON *params, cJSON *id);

cJSON *set_debug_mode_enabled(jrpc_context *ctx, cJSON *params, cJSON *id);
cJSON *set_propeller_pwm_freq_calibration(jrpc_context *ctx, cJSON *params, cJSON *id);
cJSON *set_propeller_values(jrpc_context *ctx, cJSON *params, cJSON *id);
cJSON *set_propeller_parameters(jrpc_context *ctx, cJSON *params, cJSON *id);
cJSON *set_control_loop_parameters(jrpc_context *ctx, cJSON *params, cJSON *id);
cJSON *save(jrpc_context *ctx, cJSON *params, cJSON *id);
cJSON *load(jrpc_context *ctx, cJSON *params, cJSON *id);

cJSON *update(jrpc_context *ctx, cJSON *params, cJSON *id);

#endif