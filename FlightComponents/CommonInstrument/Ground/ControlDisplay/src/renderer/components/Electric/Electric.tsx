import { Card, Tooltip } from 'antd';
import { useState, useEffect } from 'react';
import {
  FaBatteryEmpty,
  FaBatteryFull,
  FaBatteryHalf,
  FaBatteryQuarter,
  FaBatteryThreeQuarters,
} from 'react-icons/fa';

const getBatteryLevel = (voltage: number) => {
  const maximumVoltage = 12.0;
  const minimumVoltage = 0;

  if (voltage < minimumVoltage) return 0;

  return (
    ((voltage - minimumVoltage) / (maximumVoltage - minimumVoltage)) * 100.0
  );
};

const getBatteryColor = (batteryLevel: number) => {
  if (batteryLevel < 55) return '#ed4245';
  if (batteryLevel < 60) return '#faa61a';
  return '#46c46d';
};

const getBatteryIcon = (batteryLevel: number) => {
  const color = getBatteryColor(batteryLevel);

  if (batteryLevel === 0)
    return <FaBatteryEmpty size={20} cursor="pointer" color={color} />;
  if (batteryLevel < 25)
    return <FaBatteryQuarter size={20} cursor="pointer" color={color} />;
  if (batteryLevel < 50)
    return <FaBatteryHalf size={20} cursor="pointer" color={color} />;
  if (batteryLevel < 75)
    return <FaBatteryThreeQuarters size={20} cursor="pointer" color={color} />;
  return <FaBatteryFull size={20} cursor="pointer" color={color} />;
};

const getV33Color = (voltage: number) => {
  if (voltage === 0) return '#ed4245';
  if (voltage < 2.38) return '#faa61a';
  if (voltage > 3.46) return '#faa61a';
  return '#46c46d';
};

const getV5Color = (voltage: number) => {
  if (voltage === 0) return '#ed4245';
  if (voltage < 4.5) return '#faa61a';
  if (voltage > 5.5) return '#faa61a';
  return '#46c46d';
};

const getV12Color = (voltage: number) => {
  if (voltage === 0) return '#ed4245';
  if (voltage < 10.8) return '#faa61a';
  if (voltage > 13.2) return '#faa61a';
  return '#46c46d';
};

const Electric = () => {
  const [voltage33, setVoltage33] = useState<number>(0);
  const [voltage5, setVoltage5] = useState<number>(0);
  const [voltage12, setVoltage12] = useState<number>(0);

  useEffect(() => {
    window.electronAPI.statusRecieved(() => {
      setVoltage33(Number(window.electronAPI.store.get('3.3v')));
      setVoltage5(Number(window.electronAPI.store.get('5v')));
      setVoltage12(Number(window.electronAPI.store.get('12v')));
    });
    return () => {
      window.electronAPI.remove('status-recieved');
    };
  }, []);

  return (
    <Card
      title="ELECTRIC"
      style={{ margin: '8px' }}
      bordered={false}
      extra={
        <div style={{ display: 'flex', alignItems: 'center' }}>
          {getBatteryIcon(getBatteryLevel(voltage12))}
          <span
            style={{
              color: getBatteryColor(getBatteryLevel(voltage12)),
              fontSize: '0.85em',
              marginLeft: '4px',
            }}
          >
            {`${getBatteryLevel(voltage12).toFixed(0)}%`}
          </span>
        </div>
      }
    >
      <div style={{ marginTop: '24px', textAlign: 'center' }}>
        {getBatteryLevel(voltage12) < 60 && (
          <div style={{ color: '#ed4245' }}>バルブシステム動作不可</div>
        )}
        {getBatteryLevel(voltage12) < 55 && (
          <div style={{ color: '#ed4245' }}>不知火動作不可</div>
        )}
      </div>
      <div style={{ display: 'flex', marginTop: '8px' }}>
        <span style={{ color: '#b9bbbe', margin: 'auto auto auto 0' }}>
          ロジック
        </span>
        <span
          style={{
            fontSize: '1.25em',
            color: getV33Color(voltage33),
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
            color: getV5Color(voltage5),
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
            color: getV12Color(voltage12),
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
