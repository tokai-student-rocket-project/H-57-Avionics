import { Col, Row, Statistic } from 'antd';
import { useState, useEffect } from 'react';
import { FaClock, FaLocationArrow } from 'react-icons/fa';
import { accent, green, orange, emphasisMidium } from '../../utilities/colors';

const getColor = (mode: string, isAccent: boolean) => {
  if (mode === 'STANDBY') return orange;
  if (mode === 'PARASHUTE') return green;
  if (mode === 'LAND') return green;

  return isAccent ? accent : 'white';
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
        backgroundColor: 'rgb(17, 17, 17, 0.85)',
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
            <FaLocationArrow
              style={{ marginRight: '4px', color: emphasisMidium }}
            />
            <div>Taiki, Hokkaido</div>
            <FaClock style={{ marginRight: '4px', color: emphasisMidium }} />
            <div>{now}</div>
          </div>
        </Col>
      </Row>
    </div>
  );
};

export default FlightMode;
