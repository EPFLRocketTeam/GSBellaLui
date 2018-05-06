
#ifndef GS_MATTERHORN_FACTORIES_H
#define GS_MATTERHORN_FACTORIES_H


#include <DataStructures/Datastructs.h>
#include <vector>
#include "PayloadType.h"

static_assert(sizeof(float) == 4,
              "In order for the decoder to be able to cast floats to uint32 safely, sizeof(float) should return 4");

typedef union {
    float fl;
    uint32_t uint32;
} float_cast;

class PayloadDataConverter {

public:
    static SensorsPacket *toSensorsPacket(const FlyableType &, const uint32_t &, const std::vector<uint8_t> &);

    static SensorsPacket *toERT18SensorsPacket(const FlyableType &, const uint32_t &, const std::vector<uint8_t> &);

    static EventPacket *toEventPacket(const FlyableType &, const uint32_t &, const std::vector<uint8_t> &);

    static ControlPacket *toControlPacket(const FlyableType &, const uint32_t &, const std::vector<uint8_t> &);

    static GPSPacket *toGPSPacket(const FlyableType &, const uint32_t &, const std::vector<uint8_t> &);
};


#endif //GS_MATTERHORN_FACTORIES_H
