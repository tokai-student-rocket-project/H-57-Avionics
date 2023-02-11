import { Card } from 'antd';
import { useState, useEffect } from 'react';
import {
  FaBatteryEmpty,
  FaBatteryFull,
  FaBatteryHalf,
  FaBatteryQuarter,
  FaBatteryThreeQuarters,
} from 'react-icons/fa';
import { green, red, orange, emphasisMidium } from '../../utilities/colors';

const getBatteryLevel = (voltage: number) => {
  const maximumVoltage = 12.0;
  const minimumVoltage = 0;

  if (voltage < minimumVoltage) return 0;

  return (
    ((voltage - minimumVoltage) / (maximumVoltage - minimumVoltage)) * 100.0
  );
};

const getBatteryColor = (batteryLevel: number) => {
  if (batteryLevel < 55) return red;
  if (batteryLevel < 60) return orange;
  return green;
};

const getBatteryIcon = (batteryLevel: number) => {
  const color = getBatteryColor(batteryLevel);

  if (batteryLevel === 0) return <FaBatteryEmpty size={20} color={color} />;
  if (batteryLevel < 25) return <FaBatteryQuarter size={20} color={color} />;
  if (batteryLevel < 50) return <FaBatteryHalf size={20} color={color} />;
  if (batteryLevel < 75)
    return <FaBatteryThreeQuarters size={20} color={color} />;
  return <FaBatteryFull size={20} color={color} />;
};

const getV33Color = (voltage: number) => {
  if (voltage === 0) return red;
  if (voltage < 2.38) return orange;
  if (voltage > 3.46) return orange;
  return green;
};

const getV5Color = (voltage: number) => {
  if (voltage === 0) return red;
  if (voltage < 4.5) return orange;
  if (voltage > 5.5) return orange;
  return green;
};

const getV12Color = (voltage: number) => {
  if (voltage === 0) return red;
  if (voltage < 10.8) return orange;
  if (voltage > 13.2) return orange;
  return green;
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
      <div
        style={{
          margin: '0px -16px',
          textAlign: 'center',
          color: red,
          fontWeight: 'bold',
        }}
      >
        {getBatteryLevel(voltage12) < 60 && <div>バルブシステム動作不可</div>}
        {getBatteryLevel(voltage12) < 55 && <div>不知火動作不可</div>}
      </div>
      <div
        style={{
          display: 'grid',
          gridTemplateColumns: 'auto 1fr auto auto',
          alignItems: 'end',
          marginTop: '8px',
        }}
      >
        <div style={{ color: emphasisMidium }}>回路</div>
        <div />
        <div
          style={{
            fontSize: '1.25em',
            color: getV33Color(voltage33),
            margin: 'auto 0 auto auto',
          }}
        >
          {voltage33}
        </div>
        <div style={{ color: emphasisMidium }}>
          <span style={{ fontSize: '1.125em', marginLeft: '4px' }}>V</span>
          <span style={{ marginLeft: '4px' }}>/ 3.3V</span>
        </div>
        <div style={{ color: emphasisMidium }}>供給1</div>
        <div />
        <div
          style={{
            fontSize: '1.25em',
            color: getV5Color(voltage5),
            margin: 'auto 0 auto auto',
          }}
        >
          {voltage5}
        </div>
        <div style={{ color: emphasisMidium }}>
          <span style={{ fontSize: '1.125em', marginLeft: '4px' }}>V</span>
          <span style={{ marginLeft: '4px' }}>/ 5V</span>
        </div>
        <div style={{ color: emphasisMidium }}>供給2</div>
        <div />
        <div
          style={{
            fontSize: '1.25em',
            color: getV12Color(voltage12),
            margin: 'auto 0 auto auto',
          }}
        >
          {voltage12}
        </div>
        <div style={{ color: emphasisMidium }}>
          <span style={{ fontSize: '1.125em', marginLeft: '4px' }}>V</span>
          <span style={{ marginLeft: '4px' }}>/ 12V</span>
        </div>
      </div>
    </Card>
  );
};

export default Electric;
