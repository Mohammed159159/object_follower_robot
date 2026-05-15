/**
 * @file  main.cpp
 * @brief Object-follower robot — bang-bang controller with hysteresis & PWM
 *
 * ─────────────────────────────────────────────────────────────────────
 *  PIN ASSIGNMENTS
 * ─────────────────────────────────────────────────────────────────────
 *  HC-SR04 TRIG  →  D11
 *  HC-SR04 ECHO  →  D10
 *  L298N   IN1   →  PB_0  (motor forward)
 *  L298N   IN2   →  PB_1  (motor backward)
 *  L298N   ENA   →  D3    (PWM speed control - remove the jumper!)
 *  On-board LED  →  LED1  (lit while driving)
 * ─────────────────────────────────────────────────────────────────────
 */

#include "mbed.h"

/* ═══════════════════════════════════════════════════════════════════════
 *  TUNABLE PARAMETERS
 * ═══════════════════════════════════════════════════════════════════════ */

static constexpr float DIST_UPPER  = 25.0f;  /**< cm — start driving above this */
static constexpr float DIST_LOWER  = 15.0f;  /**< cm — stop below this           */
static constexpr auto  LOOP_PERIOD = 50ms;   /**< control loop period (~20 Hz)   */
static constexpr auto  ECHO_TIMEOUT = 25ms;  /**< HC-SR04 max wait (~400 cm)     */

/* Motor Speed Setting (0.0f to 1.0f) */
static constexpr float DRIVE_SPEED = 0.5;   /**< 80% speed when driving         */

/* ═══════════════════════════════════════════════════════════════════════
 *  HARDWARE OBJECTS
 * ═══════════════════════════════════════════════════════════════════════ */

/* HC-SR04 */
static DigitalOut trig(D11, 0);  
static DigitalIn  echo(D10);     
static Timer      echo_timer;    

/* L298N motor driver */
static DigitalOut motor_f(PB_0, 0); /* IN1: motor forward  */
static DigitalOut motor_b(PB_1, 0); /* IN2: motor backward */
static PwmOut     motor_en(D3);     /* ENA: motor speed control */

/* Indicators */
static DigitalOut     led(LED1);
static BufferedSerial pc(USBTX, USBRX, 115200);

/* ═══════════════════════════════════════════════════════════════════════
 *  HC-SR04 DRIVER
 * ═══════════════════════════════════════════════════════════════════════ */

static float read_distance_cm()
{
    trig = 1;
    wait_us(10);
    trig = 0;

    echo_timer.reset();
    echo_timer.start();
    while (!echo) {
        if (echo_timer.elapsed_time() > ECHO_TIMEOUT) {
            echo_timer.stop();
            return -1.0f;
        }
    }

    echo_timer.reset();
    while (echo) {
        if (echo_timer.elapsed_time() > ECHO_TIMEOUT) {
            echo_timer.stop();
            return -1.0f;
        }
    }
    echo_timer.stop();

    auto us = std::chrono::duration_cast<std::chrono::microseconds>(
                  echo_timer.elapsed_time()).count();
    return static_cast<float>(us) / 58.0f;
}

/* ═══════════════════════════════════════════════════════════════════════
 *  MOTOR HELPERS
 * ═══════════════════════════════════════════════════════════════════════ */

/** Drive the motor forward at the specified speed (0.0 to 1.0) */
static void motor_forward(float speed)
{
    motor_b = 0;  
    motor_f = 1;   
    motor_en.write(speed); // Apply PWM duty cycle
}

/** Cut power to the motor */
static void motor_stop()
{
    motor_f = 0;  
    motor_b = 0;
    motor_en.write(0.0f); // Set PWM to 0%
}

/* ═══════════════════════════════════════════════════════════════════════
 *  SERIAL DEBUG HELPER
 * ═══════════════════════════════════════════════════════════════════════ */

static void debug_status(float dist_cm, bool moving)
{
    char buf[64];
    // Cast to int to prevent the Mbed OS float crash!
    snprintf(buf, sizeof(buf),
             "[INFO] dist = %3d cm  |  %s\r\n",
             (int)dist_cm,
             moving ? "DRIVING" : "STOPPED");
    pc.write(buf, strlen(buf));
}

/* ═══════════════════════════════════════════════════════════════════════
 *  main()
 * ═══════════════════════════════════════════════════════════════════════ */
int main()
{
    const char *boot_msg = "[BOOT] Object-follower with PWM ready.\r\n";
    pc.write(boot_msg, strlen(boot_msg));

    /* Initialize PWM frequency to 1 kHz (1000 microseconds period)
     * This makes DC motors run much smoother and quieter. */
    motor_en.period_us(1000);

    bool driving = false;

    while (true) {
        float dist = read_distance_cm();

        /* Bang-bang logic with hysteresis */
        if (dist < 0.0f) {
            driving = false; 
        } else if (dist > DIST_UPPER) {
            driving = true;
        } else if (dist < DIST_LOWER) {
            driving = false;
        }

        /* Actuate */
        if (driving) {
            motor_forward(DRIVE_SPEED); // Drive at 80% speed
            led = 1;
        } else {
            motor_stop();
            led = 0;
        }

        debug_status(dist, driving);
        ThisThread::sleep_for(LOOP_PERIOD);
    }
}