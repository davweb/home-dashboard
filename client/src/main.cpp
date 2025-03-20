#include <Inkplate.h>
#include <DebugLog.h>
#include <Sleep.h>
#include <Fonts.h>
#include <Sdcard.h>
#include <ClockTools.h>
#include <HttpTools.h>
#include <Wireless.h>
#include <State.h>

#define REFRESH_COUNT 10 // How often we do a full refresh

void boot();
void renderTime(bool black);
void displayValue(const String &label, const String &value);
void logBatteryLevel();
void initialise(const String &label, bool(&func)());
void renderState();
void getState();


// We store state in RTC memory so it persists across deep sleep and can be used
// to populate screen buffer
RTC_DATA_ATTR int count = 0;
RTC_DATA_ATTR bool booted = false;
RTC_DATA_ATTR bool sdCardOk = false;
RTC_DATA_ATTR bool wiFiConnected = false;
RTC_DATA_ATTR int8_t insideTemperature = 0;
RTC_DATA_ATTR double batteryVoltage = 0;
RTC_DATA_ATTR char currentTime[6] = { 0 };

RTC_DATA_ATTR State state;

// Use black and white display so partial updates are possible
Inkplate display(INKPLATE_1BIT);

SdFile file;

// Count refreshes so we know when to do a full refresh

// Entry point
void setup() {
    //TODO disable on release build?
    Serial.begin(115200);

    display.begin();

    if (!booted) {
        boot();
    }

    switch (getWakeReason())
    {
        // Woken from sleep by timer
        case ALARM:
            count += 1;

            if (count == REFRESH_COUNT) {
                count = 0;
            }

            break;
        // Woken from sleep by button
        case BUTTON:
            count = 0;
            break;
        // Was not sleeping
        case NOT_SLEEPING:
            count = 0;
            break;
    }

    // Full update every 10 wakes and on start or button press
    if (count == 0) {
        // Log battery level if enabled
        #ifdef BATTERY_LOG_FILE
            startSdCard();
            logBatteryLevel();
        #endif

        // Update State
        getState();
        getCurrentTime(currentTime);

        // Redraw the whole screen with up to date values
        display.clearDisplay();
        renderState();
        renderTime(true);
        display.display();
    }
    else {
        // Redraw the whole screen with values from before sleep and load it in the buffer
        display.clearDisplay();
        renderState();
        renderTime(true);
        display.preloadScreen();

        // Wipe old time and write new time
        renderTime(false);
        getCurrentTime(currentTime);
        renderTime(true);

        // Force a partial update
        display.partialUpdate(true);
    }

    // Sleep until the top of the next minute
    display.rtcGetRtcData();
    uint8_t second = display.rtcGetSecond();
    uint16_t timeToSleepSeconds = 60 - second;
    deepSleep(timeToSleepSeconds);
}

// Main loop
void loop() {
    // We will never loop as the Inkplate will go to sleep
}

// Perform one time initialisation
void boot() {
    LOG_DEBUG("Booting...");

    display.setFont(&RobotoMono_Regular20pt7b);
    display.clearDisplay();
    display.println("\n Booting...");
    display.display();

    initialise("Connecting to WiFi", startWiFi);
    initialise("Setting time", setRtcClock);
    initialise("Disconnecting WiFi", stopWiFi);

    display.partialUpdate();
    booted = true;
}

//initialise something by calling a function
void initialise(const String &label, bool(&func)()) {
    display.print(" ");
    display.print(label);
    display.print("... ");
    display.partialUpdate();
    bool result = func();
    display.println(result ? "Success" : "Failed");
    // We deliberately don't call a partial update here to limit updates
}


// Render the state of the Inkplate on the screen apart from the Time
void renderState() {
    display.setCursor(30, 50);
    displayValue("SD card available", sdCardOk ? "Yes" : "No");
    displayValue("Battery", String(batteryVoltage) + "V");
    displayValue("Network connected", wiFiConnected ? "Yes" : "No");
    displayValue("Inside Temperature", String(insideTemperature) + "C");
    displayValue("Outside Temperature", String(state.weather.outsideTemperature) + "C");
    displayValue("Chance of Rain", state.weather.rainChance);
    displayValue(state.weather.sunEvent, state.weather.sunTime);
    displayValue("Next Bin Collection", state.recycling.date);
    displayValue("Next Bin Type", state.recycling.type == GENERAL_WASTE ? "General Waste" : "Recycling");

    for (int i = 0; i < BUS_STOPS; i++) {
        char info[46];
        sprintf(info, "%s %s %s", state.busStops[i].times[0].route, state.busStops[i].times[0].destination, state.busStops[i].times[0].due);
        displayValue(state.busStops[i].name, info);
    }

    for (int i = 0; i < PEOPLE_COUNT; i++) {
        displayValue(String(state.people[i].name) + "'s Phone", state.people[i].atHome ? "Connected" : "Not Connected");
    }
}

// Update the state of the Inkplate
void getState() {
    LOG_TRACE("Getting State");

    startWiFi();

    sdCardOk = display.getSdCardOk();
    insideTemperature = display.readTemperature();
    batteryVoltage = display.readBattery();
    wiFiConnected = display.isConnected();

    if (wiFiConnected) {
        fetchState(state);
    }

    stopWiFi();
}

// Display a label and value on the screen
void displayValue(const String &label, const String &value) {
    int16_t x = display.getCursorX();
    display.setFont(&Roboto_Bold16pt7b);
    display.print(label);
    display.print(": ");
    display.setFont(&Roboto_Regular16pt7b);
    display.println(value);
    display.setCursor(x, display.getCursorY());
}

// Show the current time on the screen, optionally in white to clear previous value
void renderTime(bool black) {
    display.setTextColor(black ? BLACK : WHITE);
    display.setFont(&Roboto_Bold24pt7b);
    display.setCursor(1060, 50);
    display.print(currentTime);
}

#ifdef BATTERY_LOG_FILE
    // Log battery level to file on the SD card
    void logBatteryLevel() {
        char dataToWrite[25];
        char dateTime[20];
        getCurrentDateTime(dateTime);

        sprintf(dataToWrite,
            "%s,%.2f",
            dateTime,
            display.readBattery());

        LOG_TRACE("Battery level", dataToWrite);

        if (!display.getSdCardOk()) {
            LOG_WARN("SD card not available");
            return;
        }

        const char* fileName = BATTERY_LOG_FILE;

        if (!file.open(fileName, FILE_WRITE))
        {
            LOG_WARN("Error writing battery level to file");
        }
        else
        {
            file.write(dataToWrite);
            file.write("\n");
            file.close();
        }
    }
#endif



