#ifndef __USER_CONTROL_PID_H__
#define __USER_CONTROL_PID_H__

class PID
{
public:
    PID(double p, double i, double d);

private:
    double m_p;
    double m_i;
    double m_d;
};

#endif