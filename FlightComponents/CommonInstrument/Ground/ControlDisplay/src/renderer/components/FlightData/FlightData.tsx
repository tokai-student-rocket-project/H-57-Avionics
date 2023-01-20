import { Col, Row, Statistic } from 'antd';
import { useState, useEffect } from 'react';

const getColor = (mode: string, isAccent: boolean) => {
  if (mode === 'STANDBY') return '#faa61a';
  if (mode === 'PARASHUTE') return '#46c46d';
  if (mode === 'LAND') return '#46c46d';

  return isAccent ? '#5865f2' : 'white';
};

const FlightData = () => {
  const [altitude, setAltitude] = useState<number>();
  const [acceleration, setAcceleration] = useState<number>();

  const [flightMode, setFlightMode] = useState<string>('');

  useEffect(() => {
    window.electronAPI.flightDataRecieved(() => {
      setAltitude(Number(window.electronAPI.store.get('altitude')));
      setAcceleration(
        Number(window.electronAPI.store.get('acceleration')) * 9.80665
      );
    });

    window.electronAPI.statusRecieved(() => {
      setFlightMode(
        ['STANDBY', 'THRUST', 'CLIMB', 'DESCENT', 'PARASHUTE', 'LAND'][
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
            value={altitude?.toFixed(1)}
            suffix="m"
          />
        </Col>
        <Col span={12}>
          <Statistic
            valueStyle={{ color: 'white' }}
            title="Acceleration"
            value={acceleration?.toFixed(1)}
            suffix="m/s^2"
          />
        </Col>
      </Row>
    </div>
  );
};

export default FlightData;
