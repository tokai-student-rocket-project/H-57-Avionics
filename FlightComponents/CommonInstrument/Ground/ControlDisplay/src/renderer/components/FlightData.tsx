import { Col, Row, Statistic } from 'antd';

const FlightData = () => {
  return (
    <div
      style={{
        backgroundColor: '#202225',
        padding: '16px 64px',
        borderBottomColor: '#5865f2',
        clipPath: 'polygon(0% 100%, 0% 0%, 88% 0%, 100% 100%)',
        borderTopWidth: '0px',
        borderLeftWidth: '0px',
        borderRightWidth: '0px',
        borderBottomWidth: '4px',
        borderStyle: 'solid',
      }}
    >
      <Row gutter={16}>
        <Col span={12}>
          <Statistic title="Altitude" value={0.0} suffix="m" />
        </Col>
        <Col span={12}>
          <Statistic title="Speed" value={0.0} suffix="m/s" />
        </Col>
      </Row>
    </div>
  );
};

export default FlightData;
