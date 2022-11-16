import { Card, Descriptions } from 'antd';
import { useState } from 'react';

const positiveColor = '#46c46d';
const nagativeColor = '#ed4245';

const Status = () => {
  const [flightpinState, setFlightpinState] = useState<'CLOSE' | 'OPEN'>(
    'CLOSE'
  );
  const [shiranui3State, setShiranui3State] = useState<'OFF' | 'ON'>('OFF');
  const [buzzerState, setBuzzerState] = useState<'OFF' | 'ON'>('OFF');

  window.electronAPI.recievedData((_, recievedData) => {
    const recievedDataObject = JSON.parse(recievedData);
    if (recievedDataObject.t === 's') {
      setFlightpinState(recievedDataObject.f === '1' ? 'OPEN' : 'CLOSE');
      setShiranui3State(recievedDataObject.s3 === '1' ? 'ON' : 'OFF');
      setBuzzerState(recievedDataObject.b === '1' ? 'ON' : 'OFF');
    }
  });

  return (
    <Card title="STATUS" bordered={false}>
      <Descriptions column={1} labelStyle={{ color: 'white' }}>
        <Descriptions.Item label="フライトピン">
          <span
            style={{
              color: flightpinState === 'CLOSE' ? nagativeColor : positiveColor,
            }}
          >
            {flightpinState}
          </span>
        </Descriptions.Item>
        <Descriptions.Item label="不知火Ⅲ">
          <span
            style={{
              color: shiranui3State === 'OFF' ? nagativeColor : positiveColor,
            }}
          >
            {shiranui3State}
          </span>
        </Descriptions.Item>
        <Descriptions.Item label="不知火Ⅳ">
          <span style={{ color: '#72767d' }}>N/A</span>
        </Descriptions.Item>
        <Descriptions.Item label="ブザー">
          <span
            style={{
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
