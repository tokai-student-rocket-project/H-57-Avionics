import { Card } from 'antd';

const AltitudeIndicator = () => {
  return (
    <Card title="ALTITUDE INDCATOR" bordered={false} style={{ margin: '8px' }}>
      <div
        style={{
          height: '250px',
          display: 'flex',
          alignItems: 'center',
          justifyContent: 'center',
        }}
      >
        <span style={{ fontSize: '2em' }}>COMING SOON</span>
      </div>
    </Card>
  );
};

export default AltitudeIndicator;
