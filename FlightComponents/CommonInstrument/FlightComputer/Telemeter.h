#ifndef TELEMETER_H_
#define TELEMETER_H_

class Telemeter
{
private:
  uint8_t statusLabel = 0x01;
  uint8_t flightDataLabel = 0x02;
  uint8_t configLabel = 0x03;
  uint8_t eventLabel = 0x04;

  void sendPacket(const uint8_t* data, const size_t size);

public:
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
    float speed,
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
    String event
  );
};

#endif