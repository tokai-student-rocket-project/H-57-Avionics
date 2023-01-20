import { Card } from 'antd';
import { useEffect, useState } from 'react';
import ProjectLogo from '../../images/logo/project-logo.png';
import MissionLogo from '../../images/logo/mission-logo.png';
import Body from '../../images/body.png';

const Mission = () => {
  const [now, setNow] = useState<string>();

  useEffect(() => {
    setInterval(() => {
      const nowRaw = new Date();
      setNow(`${nowRaw.toDateString()}\n${nowRaw.toTimeString()}`);
    }, 1000);
  }, []);

  return (
    <Card
      title="MISSION"
      style={{ margin: '8px', width: '100%' }}
      bordered={false}
      extra={<img src={ProjectLogo} alt="ProjectLogo" height="36px" />}
    >
      <div style={{ marginTop: '24px' }}>
        <div
          style={{
            display: 'flex',
            alignItems: 'center',
            marginTop: '16px',
            width: '100%',
          }}
        >
          <img src={MissionLogo} alt="MissionLogo" height="64px" />
          <div style={{ marginLeft: '16px', width: '200px' }}>
            <div style={{ fontSize: '1.5em', lineHeight: '1' }}>TSRP</div>
            <div style={{ fontSize: '3em', lineHeight: '1' }}>H-57</div>
          </div>
          <div style={{ marginLeft: '16px', fontSize: '1.05em' }}>
            <div>Taiki, Hokkaido</div>
            <div>{now}</div>
          </div>
        </div>
        <div
          style={{ display: 'block', alignItems: 'center', marginTop: '16px' }}
        >
          <img src={Body} alt="Body" width="100%" />
        </div>
      </div>
    </Card>
  );
};

export default Mission;
