import { Card, Tooltip, Popover } from 'antd';
import { useState, useEffect } from 'react';
import { FaRss, FaCog } from 'react-icons/fa';
import SerialportSelector from './components/SerialportSelector';
import BlinkIndicator from './components/BlinkIndicator/BlinkIndicator';

const rssiToColor = (rssi: number): string => {
  if (rssi === -999) return '#72767d';
  if (rssi < -100) return '#ed4245';
  if (rssi < -70) return '#faa61a';
  return '#46c46d';
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
      style={{ margin: '8px' }}
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
              color="#b9bbbe"
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
        </div>
      }
    >
      <BlinkIndicator />
    </Card>
  );
};

export default Telecom;
