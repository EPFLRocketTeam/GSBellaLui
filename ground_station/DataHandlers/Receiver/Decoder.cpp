#include <cassert>
#include <iostream>
#include <utility>
#include <DataStructures/datastructs.h>
#include "Decoder.h"

Decoder::Decoder() {
    currentState_ = INITIAL_STATE;
    action_ = &Decoder::seekHeader;
}

bool Decoder::processByte(uint8_t byte) {
    assert(!datagramReady());

    (this->*action_)(byte);

    return this->datagramReady();
}

bool Decoder::processHeader(std::vector<uint8_t> headerBuffer) {
    assert(headerBuffer.size() == HEADER_SIZE);

    uint32_t seqNum = 0;
    for (size_t i = 0; i < SEQUENCE_NUMBER_SIZE; i++) {
        seqNum <<= 8 * sizeof(uint8_t);
        seqNum |= headerBuffer[i];
    }

    uint8_t payloadType = headerBuffer[SEQUENCE_NUMBER_SIZE];

    if (0 <= payloadType && payloadType < static_cast<uint8_t >(DatagramPayloadType::Count)) {
        currentDatagram_.sequenceNumber_ = seqNum;
        currentDatagram_.payloadType_ = DatagramPayloadType(payloadType);
        return true;
    } else {
        std::cout << "Wrong datagram payload type!\n";
        return false;
    };
}


void Decoder::processTelemetryPayload(std::vector<uint8_t> payloadBuffer) {

    std::shared_ptr<IDeserializable>
    (*f)(std::vector<uint8_t>) = TELEMETRY_PAYLOAD_FACTORIES.at(currentDatagram_.payloadType_);

    currentDatagram_.deserializedPayload_ = std::move(f(payloadBuffer));
}

bool Decoder::datagramReady() {
    return currentDatagram_.complete;
}

Datagram Decoder::retrieveDatagram() {
    assert(datagramReady());

    Datagram r = currentDatagram_;
    resetMachine();

    return r;
}

void Decoder::jumpToNextState() {
    auto pair = STATES_TABLE.at(currentState_);

    currentState_ = pair.first;
    action_ = pair.second;
}

void Decoder::resetMachine() {
    byteBuffer_.clear();
    checksumAccumulator_.clear();
    currentDatagram_ = Datagram();
    currentState_ = INITIAL_STATE;
    action_ = &Decoder::seekHeader;
}

void Decoder::seekHeader(uint8_t byte) {
    assertBufferSmallerThan(PREAMBLE_SIZE);

    if (byte == HEADER_PREAMBLE_FLAG) {
        byteBuffer_.push_back(byte);
    } else {
        byteBuffer_.clear();
    }

    if (byteBuffer_.size() == PREAMBLE_SIZE) {
        byteBuffer_.clear();
        jumpToNextState();
    }
}

void Decoder::accumulateHeader(uint8_t byte) {
    assertBufferSmallerThan(HEADER_SIZE);

    byteBuffer_.push_back(byte);

    if (byteBuffer_.size() == HEADER_SIZE) {
        for (auto b : byteBuffer_) {
            checksumAccumulator_.push_back(b);
        }

        if (processHeader(byteBuffer_)) {
            byteBuffer_.clear();
            jumpToNextState();
        } else {
            resetMachine();
        }
    }
}

void Decoder::seekControlFlag(uint8_t byte) {
    assert(byteBuffer_.empty());

    if (byte == CONTROL_FLAG) {
        checksumAccumulator_.push_back(byte);
        jumpToNextState();
    } else {
        resetMachine();
    }
}

void Decoder::accumulatePayload(uint8_t byte) {
    assertBufferSmallerThan(PAYLOAD_TYPES_LENGTH.at(currentDatagram_.payloadType_));

    byteBuffer_.push_back(byte);
    checksumAccumulator_.push_back(byte);

    if (byteBuffer_.size() == PAYLOAD_TYPES_LENGTH.at(currentDatagram_.payloadType_)) {
        processTelemetryPayload(byteBuffer_);
        byteBuffer_.clear();
        jumpToNextState();
    }
}

void Decoder::accumulateChecksum(uint8_t byte) {
    assertBufferSmallerThan(CHECKSUM_SIZE);

    byteBuffer_.push_back(byte);

    if (byteBuffer_.size() == CHECKSUM_SIZE) {
        validatePayload();
        jumpToNextState();
    }
}

void Decoder::validatePayload() {

    //TODO: compute CRC checksum with checksumAccumulator.
    currentDatagram_.complete = true;
    jumpToNextState();
}

void Decoder::assertBufferSmallerThan(size_t bound) {
    assert(0 <= byteBuffer_.size() && byteBuffer_.size() < bound);
}

DecodingState Decoder::currentState() const {
    return currentState_;
}