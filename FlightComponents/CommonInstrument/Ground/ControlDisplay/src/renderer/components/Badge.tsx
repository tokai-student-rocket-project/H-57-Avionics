import { Typography } from 'antd';
import { useState } from 'react';

const { Title } = Typography;

const Badge = () => {
  const [flightTime, setFlightTime] = useState<number>(0);

  window.electronAPI.recievedData((_, recievedData) => {
    const recievedDataObject = JSON.parse(recievedData);
    if (recievedDataObject.t === 'f') {
      setFlightTime(Number(recievedDataObject.ft) / 1000.0);
    }
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
          {`T + ${flightTime.toFixed(2)}`}
        </Title>
      </div>
    </div>
  );
};

export default Badge;
