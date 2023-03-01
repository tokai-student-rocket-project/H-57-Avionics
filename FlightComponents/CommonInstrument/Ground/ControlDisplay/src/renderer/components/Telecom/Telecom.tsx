import { Card, Tooltip, Popover } from 'antd';
import { useState, useEffect } from 'react';
import { FaRss, FaCog } from 'react-icons/fa';
import {
  green,
  red,
  orange,
  emphasisHigh,
  emphasisDisabled,
} from '../../utilities/colors';
import SerialportSelector from './components/SerialportSelector';
import BlinkIndicator from './components/BlinkIndicator';

const rssiToColor = (rssi: number): string => {
  if (rssi === -999) return emphasisDisabled;
  if (rssi < -120) return red;
  if (rssi < -70) return orange;
  return green;
};

const Telecom = () => {
  const [rssi, setRssi] = useState<number>(-999);

  useEffect(() => {
    window.electronAPI.rssiRecieved(() => {
      setRssi(window.electronAPI.store.get('rssi'));
    });

    return () => {
      window.electronAPI.remove('rssi-recieved');
    };
  }, []);

  return (
    <Card
      title="TELECOM"
      bordered={false}
      extra={
        <div style={{ display: 'flex', alignItems: 'center' }}>
          <Popover
            content={<SerialportSelector />}
            placement="bottom"
            trigger="click"
          >
            <FaCog
              cursor="pointer"
              style={{ marginLeft: '16px' }}
              size={16}
              color={emphasisHigh}
            />
          </Popover>
          <Tooltip placement="bottom" title={rssi}>
            <FaRss
              style={{ marginLeft: '16px' }}
              size={16}
              cursor="pointer"
              color={rssiToColor(rssi)}
            />
          </Tooltip>
          <span
            style={{
              color: rssiToColor(rssi),
              fontSize: '0.85em',
              marginLeft: '4px',
            }}
          >
            {rssi}
          </span>
        </div>
      }
    >
      <BlinkIndicator />
    </Card>
  );
};

export default Telecom;
