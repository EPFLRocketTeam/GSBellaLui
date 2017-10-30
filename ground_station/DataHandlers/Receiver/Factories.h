
#ifndef GS_MATTERHORN_FACTORIES_H
#define GS_MATTERHORN_FACTORIES_H


#include <DataStructures/datastructs.h>
#include <vector>

static_assert(sizeof(float) == 4,
              "In order for the decoder to be able to cast floats to uint32 safely, sizeof(float) should return 4");

typedef union {
    float fl;
    uint32_t uint32;
} float_cast;

class Factories {

public:
    static std::shared_ptr<IDeserializable> telemetryReadingFactory(std::vector<uint8_t>);

    static int16_t parseInt16(vector<uint8_t>::iterator &);

    static uint32_t parseUint32(vector<uint8_t>::iterator &);
};


#endif //GS_MATTERHORN_FACTORIES_H
