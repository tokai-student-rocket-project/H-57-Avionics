import { Col, Row, Statistic } from 'antd';
import { useState, useEffect } from 'react';
import { accent, green, orange } from '../../utilities/colors';

const getColor = (mode: string, isAccent: boolean) => {
  if (mode === 'STANDBY') return orange;
  if (mode === 'PARASHUTE') return green;
  if (mode === 'LAND') return green;

  return isAccent ? accent : 'white';
};

const FlightData = () => {
  const [altitude, setAltitude] = useState<number>(0);
  const [flightTime, setFlightTime] = useState<number>(0);
  const [acceleration, setAcceleration] = useState<number>(0);

  const [flightMode, setFlightMode] = useState<string>('');

  const [lastAltitude, setLastAltitude] = useState<number>(0);
  const [lastFlightTime, setLastFlightTime] = useState<number>(0);
  const [climbRate, setClimbRate] = useState<number>();

  useEffect(() => {
    window.electronAPI.flightDataRecieved(() => {
      setAltitude(Number(window.electronAPI.store.get('altitude')));
      setAcceleration(
        Number(window.electronAPI.store.get('acceleration')) * 9.80665
      );
      setFlightTime(Number(window.electronAPI.store.get('flight-time')));
    });

    return () => {
      window.electronAPI.remove('flight-data-recieved');
    };
  }, []);

  useEffect(() => {
    const altitudeDifference = altitude - lastAltitude;
    const timeDifference = flightTime - lastFlightTime;

    if (timeDifference === 0) return;

    setClimbRate(altitudeDifference / timeDifference);

    setLastAltitude(altitude);
    setLastFlightTime(flightTime);
  }, [altitude]);

  useEffect(() => {
    window.electronAPI.statusRecieved(() => {
      setFlightMode(
        ['STANDBY', 'THRUST', 'CLIMB', 'DESCENT', 'PARASHUTE', 'LAND'][
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
        backgroundColor: 'rgb(17, 17, 17, 0.85)',
        padding: '16px 64px 16px 32px',
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
        <Col span={8}>
          <Statistic
            valueStyle={{ color: 'white' }}
            title="Altitude"
            value={altitude?.toFixed(1)}
            suffix="m"
          />
        </Col>
        <Col span={8}>
          <Statistic
            valueStyle={{ color: 'white' }}
            title="Climb Rate"
            value={climbRate?.toFixed(2)}
            suffix="m/s"
          />
        </Col>
        <Col span={8}>
          <Statistic
            valueStyle={{ color: 'white' }}
            title="Acceleration"
            value={acceleration?.toFixed(1)}
            suffix={
              <div style={{ display: 'flex' }}>
                <div>m/s</div>
                <div style={{ fontSize: '0.5em', margin: '0px 0px auto 0px' }}>
                  2
                </div>
              </div>
            }
          />
        </Col>
      </Row>
    </div>
  );
};

export default FlightData;
