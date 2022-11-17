import { Typography } from 'antd';
import { useState, useEffect } from 'react';

const { Title } = Typography;

const Badge = () => {
  const [flightTime, setFlightTime] = useState<string>('0.00');

  window.electronAPI.flightDataUpdated(() => {
    setFlightTime(window.electronAPI.store.get('flight-time'));
  });

  return (
    <div>
      <div
        style={{
          backgroundColor: '#202225',
          padding: '8px',
          height: '64px',
          display: 'flex',
          alignItems: 'center',
          justifyContent: 'center',
          clipPath: 'polygon(12% 100%, 0% 0%, 100% 0%, 88% 100%)',
          borderBottomColor: '#5865f2',
          borderBottomWidth: '2px',
          borderStyle: 'solid',
        }}
      >
        <Title style={{ color: 'white', marginBottom: '0' }} level={1}>
          XHX-X57X
        </Title>
      </div>
      <div
        style={{
          backgroundColor: '#202225',
          padding: '8px',
          display: 'flex',
          alignItems: 'center',
          justifyContent: 'center',
          clipPath: 'polygon(20.8% 100%, 12% 0%, 88% 0%, 79.2% 100%)',
        }}
      >
        <Title style={{ color: 'white', marginBottom: '0' }} level={3}>
          {`T + ${flightTime}`}
        </Title>
      </div>
    </div>
  );
};

export default Badge;
