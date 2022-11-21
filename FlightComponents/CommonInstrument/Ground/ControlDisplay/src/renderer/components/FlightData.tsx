import { Col, Row, Statistic } from 'antd';
import { useState, useEffect } from 'react';

const getColor = (mode: string, isAccent: boolean) => {
  if (mode === 'STANDBY') return '#faa61a';
  if (mode === 'PARASHUTE') return '#46c46d';

  return isAccent ? '#5865f2' : 'white';
};

const FlightData = () => {
  const [altitude, setAltitude] = useState<string>('0.0');
  const [speed, setSpeed] = useState<string>('0.00');

  const [oldTime, setOldTime] = useState<number>(0);

  const [flightMode, setFlightMode] = useState<string>('');

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

  useEffect(() => {
    window.electronAPI.flightDataRecieved(() => {
      setAltitude(window.electronAPI.store.get('altitude'));
      setSpeed(
        accelerationToSpeed(
          Number(window.electronAPI.store.get('acceleration-x')) * 9.8,
          Number(window.electronAPI.store.get('acceleration-y')) * 9.8,
          (Number(window.electronAPI.store.get('acceleration-z')) - 1.0) * 9.8,
          Number(window.electronAPI.store.get('flight-time')) - oldTime,
          Number(speed)
        ).toFixed(2)
      );

      setOldTime(Number(window.electronAPI.store.get('flight-time')));
    });

    window.electronAPI.statusRecieved(() => {
      setFlightMode(
        ['STANDBY', 'THRUST', 'CLIMB', 'DESCENT', 'PARASHUTE'][
          Number(window.electronAPI.store.get('flight-mode'))
        ]
      );
    });

    return () => {
      window.electronAPI.remove('flight-data-recieved');
      window.electronAPI.remove('status-recieved');
    };
  }, []);

  return (
    <div
      style={{
        backgroundColor: '#202225',
        padding: '16px 128px 16px 64px',
        borderBottomColor: getColor(flightMode, true),
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
          <Statistic
            valueStyle={{ color: 'white' }}
            title="Altitude"
            value={altitude}
            suffix="m"
          />
        </Col>
        <Col span={12}>
          <Statistic
            valueStyle={{ color: 'white' }}
            title="Speed"
            value={speed}
            suffix="m/s"
          />
        </Col>
      </Row>
    </div>
  );
};

export default FlightData;
