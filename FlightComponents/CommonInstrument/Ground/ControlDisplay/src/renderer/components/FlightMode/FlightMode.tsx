import { Col, Row, Statistic } from 'antd';
import { useState, useEffect } from 'react';
import { FaClock, FaLocationArrow } from 'react-icons/fa';

const getColor = (mode: string, isAccent: boolean) => {
  if (mode === 'STANDBY') return '#faa61a';
  if (mode === 'PARASHUTE') return '#46c46d';
  if (mode === 'LAND') return '#46c46d';

  return isAccent ? '#5865f2' : 'white';
};

const FlightMode = () => {
  const [flightMode, setFlightMode] = useState<string>('EXPERIMENT');
  const [now, setNow] = useState<string>();

  useEffect(() => {
    window.electronAPI.statusRecieved(() => {
      setFlightMode(
        ['STANDBY', 'THRUST', 'CLIMB', 'DESCENT', 'PARASHUTE', 'LAND'][
          Number(window.electronAPI.store.get('flight-mode'))
        ]
      );
    });

    setInterval(() => {
      const nowRaw = new Date();
      setNow(`${nowRaw.toDateString()}\n${nowRaw.toTimeString()}`);
    }, 1000);

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
        <Col span={12}>
          <div
            style={{
              color: 'white',
              lineHeight: '1.2em',
              display: 'grid',
              gridTemplateColumns: 'auto 1fr',
            }}
          >
            <FaLocationArrow style={{ margin: '2px', color: '#b9bbbe' }} />
            <div>Taiki, Hokkaido</div>
            <FaClock style={{ margin: '2px', color: '#b9bbbe' }} />
            <div>{now}</div>
          </div>
        </Col>
      </Row>
    </div>
  );
};

export default FlightMode;
