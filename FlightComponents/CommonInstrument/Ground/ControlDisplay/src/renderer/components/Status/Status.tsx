import { Card } from 'antd';
import { useState, useEffect } from 'react';
import { green, red, emphasisMidium } from '../../utilities/colors';

const Status = () => {
  const [flightpinState, setFlightpinState] = useState<'CLOSE' | 'OPEN'>(
    'CLOSE'
  );
  const [shiranui3State, setShiranui3State] = useState<'OFF' | 'ON'>('OFF');
  const [buzzerState, setBuzzerState] = useState<'OFF' | 'ON'>('OFF');

  useEffect(() => {
    window.electronAPI.statusRecieved(() => {
      setFlightpinState(window.electronAPI.store.get('flightpin-state'));
      setShiranui3State(window.electronAPI.store.get('shiranui3-state'));
      setBuzzerState(window.electronAPI.store.get('buzzer-state'));
    });
    return () => {
      window.electronAPI.remove('status-recieved');
    };
  }, []);

  return (
    <Card title="STATUS" style={{ marginTop: '16px' }} bordered={false}>
      <div style={{ display: 'flex' }}>
        <span
          style={{
            color: emphasisMidium,
            margin: 'auto auto auto 0',
            fontSize: '0.9em',
          }}
        >
          フライトピン
        </span>
        <span
          style={{
            fontSize: '1.15em',
            color: flightpinState === 'CLOSE' ? red : green,
            margin: 'auto 0 auto auto',
          }}
        >
          {flightpinState}
        </span>
      </div>
      <div style={{ display: 'flex', marginTop: '8px' }}>
        <span
          style={{
            color: emphasisMidium,
            margin: 'auto auto auto 0',
            fontSize: '0.9em',
          }}
        >
          不知火Ⅲ
        </span>
        <span
          style={{
            fontSize: '1.15em',
            color: shiranui3State === 'OFF' ? red : green,
            margin: 'auto 0 auto auto',
          }}
        >
          {shiranui3State}
        </span>
      </div>
      <div style={{ display: 'flex', marginTop: '8px' }}>
        <span
          style={{
            color: emphasisMidium,
            margin: 'auto auto auto 0',
            fontSize: '0.9em',
          }}
        >
          ブザー
        </span>
        <span
          style={{
            fontSize: '1.15em',
            color: buzzerState === 'OFF' ? red : green,
            margin: 'auto 0 auto auto',
          }}
        >
          {buzzerState}
        </span>
      </div>
    </Card>
  );
};

export default Status;
