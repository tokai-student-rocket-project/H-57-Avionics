import { Card, Descriptions } from 'antd';
import { useState, useEffect } from 'react';

const positiveColor = '#46c46d';
const nagativeColor = '#ed4245';

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
    <Card title="STATUS" style={{ margin: '16px' }} bordered={false}>
      <Descriptions
        style={{ marginTop: '24px' }}
        column={1}
        labelStyle={{ color: 'white' }}
      >
        <Descriptions.Item label="フライトピン">
          <span
            style={{
              fontSize: '1.2em',
              color: flightpinState === 'CLOSE' ? nagativeColor : positiveColor,
            }}
          >
            {flightpinState}
          </span>
        </Descriptions.Item>
        <Descriptions.Item label="不知火Ⅲ">
          <span
            style={{
              fontSize: '1.2em',
              color: shiranui3State === 'OFF' ? nagativeColor : positiveColor,
            }}
          >
            {shiranui3State}
          </span>
        </Descriptions.Item>
        <Descriptions.Item label="不知火Ⅳ">
          <span style={{ fontSize: '1.2em', color: '#72767d' }}>N/A</span>
        </Descriptions.Item>
        <Descriptions.Item label="ブザー">
          <span
            style={{
              fontSize: '1.2em',
              color: buzzerState === 'OFF' ? nagativeColor : positiveColor,
            }}
          >
            {buzzerState}
          </span>
        </Descriptions.Item>
      </Descriptions>
    </Card>
  );
};

export default Status;
