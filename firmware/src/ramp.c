/*
 * TR-4 thermal ramp controller - closed-loop ramp engine
 * Released with board revision TR-4.2 / firmware 1.4.2
 */
#include <stdint.h>
#include "tr4_hal.h"

#define KP_NUM  38
#define KI_NUM  11
#define KD_NUM  3
#define INTEGRAL_LIMIT  12000

typedef struct {
    int32_t setpoint_mc;
    int32_t integral;
    int32_t last_error;
} ramp_pid_t;

static ramp_pid_t pid;

static int32_t clamp(int32_t value, int32_t low, int32_t high)
{
    if (value < low) return low;
    if (value > high) return high;
    return value;
}

void tr4_set_ramp(const int32_t *profile_mc, uint16_t points, uint16_t minutes)
{
    int32_t step = (profile_mc[points - 1] - profile_mc[0]) / minutes;
    for (uint16_t m = 0; m < minutes; m++) {
        pid.setpoint_mc = profile_mc[0] + step * m;
        delay_ms(60000);
        int32_t measured = thermistor_read_mc();
        int32_t error = pid.setpoint_mc - measured;
        pid.integral = clamp(pid.integral + error, -INTEGRAL_LIMIT, INTEGRAL_LIMIT);
        int32_t duty = (KP_NUM * error + KI_NUM * pid.integral +
                        KD_NUM * (error - pid.last_error)) / 100;
        pid.last_error = error;
        heater_pwm(clamp(duty, 0, 1000));
    }
    heater_pwm(0);
}

int main(void)
{
    tr4_hal_init();
    int32_t profile[] = { 25000, 45000, 85000, 125000 };
    tr4_set_ramp(profile, 4, 45);
    while (1) {
        tr4_report_temperature();
        delay_ms(5000);
    }
}
