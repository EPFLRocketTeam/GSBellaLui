
#include <UI/GraphFeature.h>
#include <Utilities/GraphUtils.h>
#include "PacketDispatcher.h"
#include "MainWorker.h"

PacketDispatcher::PacketDispatcher(Worker *const containerWorker) :
        worker_{containerWorker},
        sensorsPacketQueues_{},
        gpsPacketQueues_{},
        eventPacketQueues_{},
        lastUpdates_{} {

    // Build queues for each flyable type
    for (size_t i = 0; i < FlyableType::Count; i++) {
        sensorsPacketQueues_.emplace(
                std::make_pair<FlyableType, std::vector<SensorsPacket *>>(static_cast<FlyableType>(i), {})
        );
        gpsPacketQueues_.emplace(
                std::make_pair<FlyableType, std::vector<GPSPacket *>>(static_cast<FlyableType>(i), {})
        );
        eventPacketQueues_.emplace(
                std::make_pair<FlyableType, std::vector<EventPacket *>>(static_cast<FlyableType>(i), {})
        );
    }
}

void
PacketDispatcher::dispatch(SensorsPacket *p) {
    sensorsPacketQueues_[p->flyableType_].emplace_back(p);
}

void
PacketDispatcher::dispatch(GPSPacket *p) {
    gpsPacketQueues_[p->flyableType_].emplace_back(p);
}

void
PacketDispatcher::dispatch(EventPacket *p) {
    eventPacketQueues_[p->flyableType_].emplace_back(p);
}

void
PacketDispatcher::processDataFlows() {
    //Sensor data needs to be polled first!

    //TODO: log
    //TODO: Build and transmit graph data

    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();


    QVector<QCPGraphData> altitudeDataBuffer = extractGraphData(sensorsPacketQueues_[FlyableType::ROCKET], altitudeFromReading);
    QVector<QCPGraphData> accelDataBuffer = extractGraphData(sensorsPacketQueues_[FlyableType::ROCKET], accelerationFromReading);


    emit worker_->graphDataReady(altitudeDataBuffer, GraphFeature::FEATURE1);
    emit worker_->graphDataReady(accelDataBuffer, GraphFeature::FEATURE2);

    displayFreshValues();
    releaseMemory();
}


/**
 *
 *
 * @param data A reference to a vector of telemetry structs.
 * @param extractionFct A helper function to convert the strucs to plottable objects (QCPGraphData).
 * @return A QVector of QCPGraphData.
 */
QVector<QCPGraphData>
PacketDispatcher::extractGraphData(std::vector<SensorsPacket *> &data, QCPGraphData (*extractionFct)(SensorsPacket)) {
    QVector<QCPGraphData> v;
    long long int lastTimestampSeen = 0;

    for (SensorsPacket *reading : data) {
        if (abs(lastTimestampSeen - reading->timestamp_) > UIConstants::GRAPH_DATA_INTERVAL_USECS) {
            v.append(extractionFct(*reading));
            lastTimestampSeen = reading->timestamp_;
        }
    }

    return v;
}


void
PacketDispatcher::displayFreshValues() {

    std::chrono::system_clock::time_point now{std::chrono::system_clock::now()};

    for (size_t i = 0; i < FlyableType::Count; i++) {

        auto t = static_cast<FlyableType>(i);

        if (msecsBetween(lastUpdates_[t], now) > UIConstants::NUMERICAL_SENSORS_VALUES_REFRESH_RATE) {
            lastUpdates_[t] = now;
            emitIfNonEmpty(sensorsPacketQueues_[t]);
            emitIfNonEmpty(gpsPacketQueues_[t]);
            emitIfNonEmpty(eventPacketQueues_[t]);
        }
    }
}

template<class T>
void
freeQueue(std::vector<T> &v) {

    for (auto *dp : v) {
        delete dp;
    }

    v.clear();
}

void
PacketDispatcher::releaseMemory() {

    for (auto &entry : sensorsPacketQueues_) {
        freeQueue<SensorsPacket *>(entry.second);
    }

    for (auto &entry : gpsPacketQueues_) {
        freeQueue<GPSPacket *>(entry.second);
    }

    for (auto &entry : eventPacketQueues_) {
        freeQueue<EventPacket *>(entry.second);
    }
}

template<class T>
void
PacketDispatcher::emitIfNonEmpty(const std::vector<T *> &v) {
    if (!v.empty()) {

        // Copy the content of the object pointed at by the last element in the vector
        T t = (*v.back());
        emit worker_->dataPacketReady(t);
    }
}
