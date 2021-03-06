#ifndef GS_MATTERHORN_RADIORECEIVER_H
#define GS_MATTERHORN_RADIORECEIVER_H

#include <boost/thread/thread.hpp>
#include <boost/array.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <utility>
#include <serial/serial.h>
#include "DataHandlers/TelemetryHandler.h"
#include "Loggers/FileLogger.h"
#include "Decoder.h"

/**
 * A telemetry handler which receives data by the mean of a serial port connected to
 * a transceiver
 */
class RadioReceiver : public TelemetryHandler {

    static constexpr uint32_t BUFFER_SIZE = 4096;

public:

    explicit RadioReceiver(const string &, const string &logTitle);

    ~RadioReceiver() override;

    void startup() override;

    vector<SensorsPacket> pollSensorsData() override;

    vector<EventPacket> pollEventsData() override;

    vector<GPSPacket> pollGPSData() override;

    void sendCommand(const uint8_t *, size_t) override;

    bool isReplayHandler() override;

private:

    /**
     * Reads all the bytes available on the serial port's buffer and forwards it to
     * the Decoder
     */
    void readSerialPort();

    void openSerialPort();

    void handleReceive(std::size_t);

    void unpackPayload();

    Decoder byteDecoder_;
    std::string devicePort_;
    serial::Serial serialPort_;
    boost::thread thread_;
    uint8_t *recvBuffer_;
    FileLogger bytesLogger_;
    std::atomic_bool threadEnabled_;
    boost::lockfree::spsc_queue<SensorsPacket> sensorsDataQueue_;
    boost::lockfree::spsc_queue<EventPacket> eventsDataQueue_;
    boost::lockfree::spsc_queue<ControlPacket> controlDataQueue_;
    boost::lockfree::spsc_queue<GPSPacket> gpsDataQueue_;

};


struct BytesReading : public ILoggable {

    BytesReading() = default;

    BytesReading(chrono::system_clock::time_point recvTime, vector<uint8_t> bytes) :
            recvTime_{recvTime}, bytes_{std::move(bytes)} {}

    chrono::system_clock::time_point recvTime_;
    vector<uint8_t> bytes_;


    string toString() const override {

        stringstream ss;
        ss << std::chrono::duration_cast<std::chrono::microseconds>(recvTime_.time_since_epoch()).count() << "\t\t";

        for (uint8_t b: bytes_) {
            ss << setw(2) << setfill('0') << std::hex << +b << " ";
        }

        return ss.str();
    }


};


#endif //GS_MATTERHORN_RADIORECEIVER_H
