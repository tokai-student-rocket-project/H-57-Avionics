import { Typography } from 'antd';
import { useState, useEffect } from 'react';
import { accent, green, orange } from '../../utilities/colors';

const { Title } = Typography;

const getColor = (mode: string, isAccent: boolean) => {
  if (mode === 'STANDBY') return orange;
  if (mode === 'PARASHUTE') return green;
  if (mode === 'LAND') return green;

  return isAccent ? accent : 'white';
};

const Badge = () => {
  const [flightTime, setFlightTime] = useState<string>('0.00');
  const [flightMode, setFlightMode] = useState<string>('');

  useEffect(() => {
    window.electronAPI.flightDataRecieved(() => {
      setFlightTime(window.electronAPI.store.get('flight-time'));
    });

    return () => {
      window.electronAPI.remove('flight-data-recieved');
    };
  }, []);

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
    <div>
      <div
        style={{
          backgroundColor: 'rgb(17, 17, 17, 0.85)',
          padding: '8px',
          height: '64px',
          display: 'flex',
          alignItems: 'center',
          justifyContent: 'center',
          clipPath: 'polygon(12% 100%, 0% 0%, 100% 0%, 88% 100%)',
          borderBottomColor: getColor(flightMode, true),
          borderBottomWidth: '2px',
          borderStyle: 'solid',
        }}
      >
        <Title style={{ color: 'white', marginBottom: '0' }} level={1}>
          H-57
        </Title>
      </div>
      <div
        style={{
          backgroundColor: 'rgb(17, 17, 17, 0.85)',
          padding: '8px',
          display: 'flex',
          alignItems: 'center',
          justifyContent: 'center',
          clipPath: 'polygon(20.8% 100%, 12% 0%, 88% 0%, 79.2% 100%)',
        }}
      >
        <Title style={{ color: 'white', marginBottom: '0' }} level={3}>
          {`X + ${flightTime}`}
        </Title>
      </div>
    </div>
  );
};

export default Badge;
