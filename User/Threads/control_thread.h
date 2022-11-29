//
// Created by 25016 on 2022/11/29.
//

#ifndef _CONTROL_H
#define _CONTROL_H

class Control
{
public:
    bool controlThreadInit();

private:
    rockerInfo m_rocker;
    propellerPower m_propeller;

    void rawRockerParams(rockerInfo_t& rc);
    void move();
    void depth();
    void roll();
    void yaw();
    propellerPower& getPropellerOutput();
    void *controlThread(void *arg);
}

#endif //_CONTROL_H
