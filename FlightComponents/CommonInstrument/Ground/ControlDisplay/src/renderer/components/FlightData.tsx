import { Col, Row, Statistic } from 'antd';
import { useState } from 'react';

const FlightData = () => {
  const [altitude, setAltitude] = useState<number>(0);
  const [speed, setSpeed] = useState<number>(0);

  let oldTime = 0;

  const accelerationToSpeed = (
    accelerationX: number,
    accelerationY: number,
    accelerationZ: number,
    deltaTime: number,
    oldSpeed: number
  ): number => {
    const deltaSpeed =
      Math.sqrt(
        accelerationX * accelerationX +
          accelerationY * accelerationY +
          accelerationZ * accelerationZ
      ) * deltaTime;

    return oldSpeed + deltaSpeed;
  };

  window.electronAPI.recievedData((_, recievedData) => {
    const recievedDataObject = JSON.parse(recievedData);
    if (recievedDataObject.t === 'f') {
      setAltitude(Number(recievedDataObject.alt));
      setSpeed(
        accelerationToSpeed(
          Number(recievedDataObject.ax) / 9.8,
          Number(recievedDataObject.ay) / 9.8,
          (Number(recievedDataObject.az) - 1.0) / 9.8,
          (Number(recievedDataObject.ft) - oldTime) / 1000.0,
          speed
        )
      );

      oldTime = Number(recievedDataObject.ft);
    }
  });

  return (
    <div
      style={{
        backgroundColor: '#202225',
        padding: '16px 64px',
        borderBottomColor: '#5865f2',
        clipPath: 'polygon(0% 100%, 0% 0%, 88% 0%, 100% 100%)',
        borderTopWidth: '0px',
        height: '96px',
        borderLeftWidth: '0px',
        borderRightWidth: '0px',
        borderBottomWidth: '4px',
        borderStyle: 'solid',
      }}
    >
      <Row gutter={16}>
        <Col span={12}>
          <Statistic title="Altitude" value={altitude.toFixed(1)} suffix="m" />
        </Col>
        <Col span={12}>
          <Statistic title="Speed" value={speed.toFixed(2)} suffix="m/s" />
        </Col>
      </Row>
    </div>
  );
};

export default FlightData;
