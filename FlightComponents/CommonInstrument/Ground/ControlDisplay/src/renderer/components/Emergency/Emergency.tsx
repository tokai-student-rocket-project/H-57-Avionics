import { Card, Button, Switch } from 'antd';
import { useState } from 'react';
import { emphasisMidium } from '../../utilities/colors';

const Emergency = () => {
  const [canExecute, setCanExecute] = useState<boolean>(false);

  const emergencySeparate = () => {
    window.electronAPI.sendConfig('es', '0');
  };

  return (
    <Card bordered={false} style={{ width: '250px', height: 'auto' }}>
      <div style={{ margin: '-8px 0' }}>
        <div style={{ display: 'flex', alignItems: 'center' }}>
          <div
            style={{
              borderRight: `1px solid ${emphasisMidium}`,
              paddingRight: '8px',
            }}
          >
            <div style={{ fontSize: '1.25em' }}>EMG</div>
          </div>
          <Switch
            onChange={() => setCanExecute((prevState) => !prevState)}
            style={{ marginLeft: '16px' }}
          />
          <Button
            danger
            disabled={!canExecute}
            style={{
              width: '100%',
              marginLeft: '8px',
              backgroundColor: canExecute ? '#FF1744' : '#606060',
            }}
            onClick={emergencySeparate}
          >
            緊急分離
          </Button>
        </div>
      </div>
    </Card>
  );
};

export default Emergency;
