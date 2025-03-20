typedef enum {
    NOT_SLEEPING,
    ALARM,
    BUTTON
} WakeReason;

/**
 * Return the reason the device woke up from sleep
 *
 * @return The reason the device woke up from sleep
*/
WakeReason getWakeReason();

/**
 * Put the device into deep sleep and wake after given number of seconds
 *
 * @param seconds The number of seconds to sleep
*/
void deepSleep(uint16_t seconds);
