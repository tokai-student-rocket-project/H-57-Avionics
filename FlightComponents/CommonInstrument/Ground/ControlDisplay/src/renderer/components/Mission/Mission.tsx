import { Card } from 'antd';
import MissionLogo from '../../images/logo/mission-logo.png';
import ProjectLogo from '../../images/logo/project-logo.png';
import Body from '../../images/body.png';

const Mission = () => {
  return (
    <Card
      title="MISSION"
      bordered={false}
      extra={<img src={ProjectLogo} alt="ProjectLogo" height="36px" />}
    >
      <div
        style={{
          display: 'flex',
          alignItems: 'center',
          width: '100%',
        }}
      >
        <img src={MissionLogo} alt="MissionLogo" height="64px" />
        <div style={{ paddingLeft: '24px' }}>
          <div
            style={{
              fontSize: '1.5em',
              lineHeight: '1',
              fontWeight: 'lighter',
            }}
          >
            57th Hybrid Rocket
          </div>
          <div style={{ fontSize: '3em', lineHeight: '1' }}>TSRP-H-57</div>
        </div>
      </div>
      <div
        style={{ display: 'block', alignItems: 'center', marginTop: '16px' }}
      >
        <img src={Body} alt="Body" width="100%" />
      </div>
    </Card>
  );
};

export default Mission;
