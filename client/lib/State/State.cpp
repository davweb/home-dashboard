#include <State.h>

boolean fetchState(State &state) {
    JsonDocument doc;

    if (getJsonFromUrl(doc, SERVER_ADDRESS)) {
        strcpy(state.currentDate, doc["date"]);

        //Weather
        strcpy(state.weather.outsideTemperature, doc["weather"]["temperature"]);
        strcpy(state.weather.rainChance, doc["weather"]["rain"]);
        strcpy(state.weather.sunEvent, doc["weather"]["sun"]["event"]);
        strcpy(state.weather.sunTime, doc["weather"]["sun"]["time"]);

        //Recycling
        strcpy(state.recycling.date, doc["recycling"]["date"]);

        if (strcmp(doc["recycling"]["type"], "General Waste") == 0) {
            state.recycling.type = GENERAL_WASTE;
        }
        else {
            state.recycling.type = RECYCLING;
        }

        //Buses
        for (int i = 0; i < BUS_STOPS; i++) {
            JsonObject busData = doc["bus_stops"][i];
            strcpy(state.busStops[i].name, busData["name"]);

            JsonArray busTimes = busData["buses"];
            state.busStops[i].timeCount = busTimes.size();

            for (int j = 0; j < state.busStops[i].timeCount; j++) {
                strcpy(state.busStops[i].times[j].route, busTimes[j]["route"]);
                strcpy(state.busStops[i].times[j].destination, busTimes[j]["destination"]);
                strcpy(state.busStops[i].times[j].due, busTimes[j]["due"]);
            };
        }

        //People
        JsonObject peopleData = doc["at_home"];
        int index = 0;

        for (JsonPair kv : peopleData) {
            strcpy(state.people[index].name, kv.key().c_str());
            state.people[index].atHome = kv.value().as<bool>();
            index += 1;
        }

        return true;
    }
    else {
        LOG_WARN("Failed to query Server");
        return false;
    }
}
