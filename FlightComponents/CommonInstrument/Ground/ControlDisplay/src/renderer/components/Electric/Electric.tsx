import { Card } from 'antd';
import { useState, useEffect } from 'react';

const getV33Color = (voltage: number) => {
  if (voltage < 2.38) return '#faa61a';
  if (voltage > 3.46) return '#faa61a';
  return '#46c46d';
};

const getV5Color = (voltage: number) => {
  if (voltage < 4.5) return '#faa61a';
  if (voltage > 5.5) return '#faa61a';
  return '#46c46d';
};

const getV12Color = (voltage: number) => {
  if (voltage < 10.8) return '#faa61a';
  if (voltage > 13.2) return '#faa61a';
  return '#46c46d';
};

const Electric = () => {
  const [voltage33, setVoltage33] = useState<string>('0');
  const [voltage5, setVoltage5] = useState<string>('0');
  const [voltage12, setVoltage12] = useState<string>('0');

  useEffect(() => {
    window.electronAPI.statusRecieved(() => {
      setVoltage33(window.electronAPI.store.get('3.3v'));
      setVoltage5(window.electronAPI.store.get('5v'));
      setVoltage12(window.electronAPI.store.get('12v'));
    });
    return () => {
      window.electronAPI.remove('status-recieved');
    };
  }, []);

  return (
    <Card title="ELECTRIC" style={{ margin: '8px' }} bordered={false}>
      <div style={{ display: 'flex', marginTop: '24px' }}>
        <span style={{ color: '#b9bbbe', margin: 'auto auto auto 0' }}>
          ロジック
        </span>
        <span
          style={{
            fontSize: '1.25em',
            color: getV33Color(Number(voltage33)),
            margin: 'auto 0 auto auto',
          }}
        >
          {voltage33}
        </span>
        <span
          style={{
            fontSize: '1.25em',
            color: '#b9bbbe',
            marginLeft: '8px',
          }}
        >
          V
        </span>
      </div>
      <div style={{ display: 'flex', marginTop: '8px' }}>
        <span style={{ color: '#b9bbbe', margin: 'auto auto auto 0' }}>
          5V供給
        </span>
        <span
          style={{
            fontSize: '1.25em',
            color: getV5Color(Number(voltage5)),
            margin: 'auto 0 auto auto',
          }}
        >
          {voltage5}
        </span>
        <span
          style={{
            fontSize: '1.25em',
            color: '#b9bbbe',
            marginLeft: '8px',
          }}
        >
          V
        </span>
      </div>
      <div style={{ display: 'flex', marginTop: '8px' }}>
        <span style={{ color: '#b9bbbe', margin: 'auto auto auto 0' }}>
          12V供給
        </span>
        <span
          style={{
            fontSize: '1.25em',
            color: getV12Color(Number(voltage12)),
            margin: 'auto 0 auto auto',
          }}
        >
          {voltage12}
        </span>
        <span
          style={{
            fontSize: '1.25em',
            color: '#b9bbbe',
            marginLeft: '8px',
          }}
        >
          V
        </span>
      </div>
    </Card>
  );
};

export default Electric;
