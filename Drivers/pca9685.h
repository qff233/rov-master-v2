#ifndef __DRIVERS_PCA9685_H__
#define __DRIVERS_PCA9685_H__

#ifdef __cplusplus
extern "C"
{
#endif

int pca9685Setup(const int pinBase, float freq /* = 50*/);
void pca9685PWMSetFreq(int fd, float freq, float pwm_calibration);

#ifdef __cplusplus
}
#endif

#endif
