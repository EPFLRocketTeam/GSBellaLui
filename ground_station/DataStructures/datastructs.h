#ifndef DATASTRUCTS_H
#define DATASTRUCTS_H

#include <string>
#include <sstream>
#include "ILoggable.h"

struct TimedData {
    TimedData(long t) : timestamp{t} {}

protected:
    long timestamp;
};

struct DataReading {
    DataReading(float v, bool b) : value{v}, validity{b} {}

    float value;
    bool validity;
};

struct YawPitchRollReading : ILoggable {
    YawPitchRollReading(float y, float p, float r, bool b) : yaw{y}, pitch{p}, roll{r}, validity{b} {}

    float yaw;
    float pitch;
    float roll;
    bool validity;

    virtual std::string toString() override {
        std::stringstream ss;

        ss << yaw << ILoggable::DELIMITER << pitch << ILoggable::DELIMITER << roll;
        return ss.str();
    }
};

struct TelemetryReading : TimedData, ILoggable {
    TelemetryReading(long t, DataReading altitude, DataReading speed, DataReading acceleration,
                     DataReading pressure, DataReading temperature, YawPitchRollReading ypr) :
            TimedData(t), altitude{altitude}, speed{speed}, acceleration{acceleration}, pressure{pressure},
            temperature{temperature}, ypr{ypr} {}

    DataReading altitude;
    DataReading speed;
    DataReading acceleration;
    DataReading pressure;
    DataReading temperature;
    YawPitchRollReading ypr;

    virtual std::string toString() override {
        std::stringstream ss;
        ss << this->timestamp << ILoggable::DELIMITER
           << altitude.value << ILoggable::DELIMITER
           << speed.value << ILoggable::DELIMITER
           << acceleration.value << ILoggable::DELIMITER
           << pressure.value << ILoggable::DELIMITER
           << temperature.value << ILoggable::DELIMITER
           << ypr.toString();

        return ss.str();
    }
};

#endif // DATASTRUCTS_H
