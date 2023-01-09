#ifndef TELEMETER_H_
#define TELEMETER_H_

class Telemeter
{
private:
  uint8_t statusLabel = 0x01;
  uint8_t flightDataLabel = 0x02;
  uint8_t configLabel = 0x03;
  uint8_t eventLabel = 0x04;

  uint8_t _buffer[256];
  uint32_t _offset;

  uint32_t _event_count;

  void reservePacket(const uint8_t* data, const size_t size);

public:
  void sendDownlink();

  void sendStatus(
    uint8_t flightMode,
    bool flightPinState,
    bool shiranui3State,
    bool buzzerState,
    float v33State,
    float v5State,
    float v12State
  );

  void sendFlightData(
    float flightTime,
    float altitude,
    float acceleration,
    float yaw,
    float pitch,
    float roll
  );

  void sendConfig(
    float separationAltitude,
    float basePressure,
    float burnTime,
    float separationProtectionTime,
    float forceSeparationTime,
    float landingTime
  );

  void sendEvent(
    float flightTime,
    String event
  );
};

#endif