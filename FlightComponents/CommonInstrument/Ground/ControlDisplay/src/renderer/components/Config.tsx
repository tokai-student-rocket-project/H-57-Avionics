import { Card, Input, Button, Row, Col, Divider } from 'antd';
import { FaArrowLeft } from 'react-icons/fa';

const Config = () => {
  return (
    <Card title="CONFIG" bordered={false} style={{ marginTop: '32px' }}>
      <Divider>基準気圧</Divider>
      <Row gutter={8} wrap={false} align="middle">
        <Col flex="auto">
          <Input addonAfter="hPa" readOnly />
        </Col>
        <Col>
          <Button type="primary">
            <FaArrowLeft />
          </Button>
        </Col>
        <Col flex="auto">
          <Input addonAfter="hPa" />
        </Col>
      </Row>
      <Divider>最短分離時間</Divider>
      <Row gutter={8} wrap={false} align="middle">
        <Col flex="auto">
          <Input addonAfter="sec" readOnly />
        </Col>
        <Col>
          <Button type="primary">
            <FaArrowLeft />
          </Button>
        </Col>
        <Col flex="auto">
          <Input addonAfter="sec" />
        </Col>
      </Row>
      <Divider>最長分離時間</Divider>
      <Row gutter={8} wrap={false} align="middle">
        <Col flex="auto">
          <Input addonAfter="sec" readOnly />
        </Col>
        <Col>
          <Button type="primary">
            <FaArrowLeft />
          </Button>
        </Col>
        <Col flex="auto">
          <Input addonAfter="sec" />
        </Col>
      </Row>
    </Card>
  );
};

export default Config;
