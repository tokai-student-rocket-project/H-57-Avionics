import { Col, Row, Statistic } from 'antd';
import { useState, useEffect } from 'react';

const getColor = (mode: string, isAccent: boolean) => {
  if (mode === 'STANDBY') return '#faa61a';
  if (mode === 'PARASHUTE') return '#46c46d';

  return isAccent ? '#5865f2' : 'white';
};

const FlightMode = () => {
  const [flightMode, setFlightMode] = useState<string>('');

  useEffect(() => {
    window.electronAPI.statusRecieved(() => {
      setFlightMode(
        ['STANDBY', 'THRUST', 'CLIMB', 'DESCENT', 'PARASHUTE'][
          Number(window.electronAPI.store.get('flight-mode'))
        ]
      );
    });

    return () => {
      window.electronAPI.remove('status-recieved');
    };
  }, []);

  return (
    <div
      style={{
        backgroundColor: '#202225',
        padding: '16px 64px 16px 128px',
        clipPath: 'polygon(0% 100%, 12% 0%, 100% 0%, 100% 100%)',
        borderBottomColor: getColor(flightMode, true),
        borderTopWidth: '0px',
        height: '96px',
        borderLeftWidth: '0px',
        borderRightWidth: '0px',
        borderBottomWidth: '4px',
        borderStyle: 'solid',
      }}
    >
      {' '}
      <Row gutter={16}>
        <Col span={12}>
          <Statistic
            valueStyle={{
              color: getColor(flightMode, false),
            }}
            title="FlightMode"
            value={flightMode}
          />
        </Col>
        <Col span={12} />
      </Row>
    </div>
  );
};

export default FlightMode;
