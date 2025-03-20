#include <Arduino.h>
#include <DebugLog.h>
#include <HttpTools.h>

#define BUS_STOPS 2
#define BUS_TIMES 5
#define PEOPLE_COUNT 5
#define SERVER_ADDRESS "http://192.168.1.17"

//Weather Data
typedef struct {
    char outsideTemperature[4];
    char rainChance[4];
    char sunEvent[9];
    char sunTime[6];
} Weather;

//Recycling
typedef enum {
    GENERAL_WASTE,
    RECYCLING
} CollectionType;

typedef struct {
    char date[22];
    CollectionType type;
} Recycling;

//Bus Data
typedef struct  {
    char route[4];
    char destination[32];
    char due[8];
} BusTime;

typedef struct  {
    char name[32];
    size_t timeCount;
    BusTime times[BUS_TIMES];
} BusStop;

typedef struct {
    char name[6];
    bool atHome;
} Person;

//Full State
typedef struct {
    char currentDate[22];
    Weather weather;
    Recycling recycling;
    BusStop busStops[BUS_STOPS];
    Person people[PEOPLE_COUNT];
} State;

/**
 * Fetch the state from the dashboard server
 */
boolean fetchState(State &state);