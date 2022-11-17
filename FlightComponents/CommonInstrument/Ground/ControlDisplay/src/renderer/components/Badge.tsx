import { Typography } from 'antd';
import logo from '../logo.png';

const { Title } = Typography;

const Badge = () => {
  return (
    <div>
      <div
        style={{
          backgroundColor: '#202225',
          padding: '8px',
          display: 'flex',
          alignItems: 'center',
          justifyContent: 'center',
          clipPath: 'polygon(12% 100%, 0% 0%, 100% 0%, 88% 100%)',
          borderBottomColor: '#5865f2',
          borderBottomWidth: '2px',
          borderStyle: 'solid',
        }}
      >
        <img
          src={logo}
          alt="logo"
          style={{ width: '32px', marginRight: '8px' }}
        />
        <Title style={{ color: 'white', marginBottom: '0' }} level={1}>
          HX-X57
        </Title>
      </div>
      <div
        style={{
          backgroundColor: '#202225',
          padding: '8px',
          display: 'flex',
          alignItems: 'center',
          justifyContent: 'center',
          clipPath: 'polygon(20.8% 100%, 12% 0%, 88% 0%, 79.2% 100%)',
        }}
      >
        <Title style={{ color: 'white', marginBottom: '0' }} level={3}>
          T + 0.00
        </Title>
      </div>
    </div>
  );
};

export default Badge;
