import { Card } from 'antd';

const AltitudeIndicator = () => {
  return (
    <Card title="ALT INDCATOR" bordered={false} style={{ margin: '8px' }}>
      <div
        style={{
          height: '250px',
          display: 'flex',
          alignItems: 'center',
          justifyContent: 'center',
        }}
      >
        <span style={{ fontSize: '2em' }}>INOP</span>
      </div>
    </Card>
  );
};

export default AltitudeIndicator;
