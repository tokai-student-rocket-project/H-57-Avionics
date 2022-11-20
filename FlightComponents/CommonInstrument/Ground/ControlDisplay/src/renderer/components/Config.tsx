import { Card, Input, Button, Row, Col, Divider } from 'antd';
import { useState, useEffect } from 'react';
import { FaArrowLeft } from 'react-icons/fa';

const Config = () => {
  const [activeBasePrassure, setActiveBasePressure] = useState<string>('');
  const [activeSeparationMinimum, setActiveSeparationMinimum] =
    useState<string>('');
  const [activeSeparationMaximum, setActiveSeparationMaximum] =
    useState<string>('');

  useEffect(() => {
    window.electronAPI.configRecieved(() => {
      setActiveBasePressure(window.electronAPI.store.get('base-pressure'));
      setActiveSeparationMinimum(
        window.electronAPI.store.get('separation-minimum')
      );
      setActiveSeparationMaximum(
        window.electronAPI.store.get('separation-maximum')
      );
    });
    return () => {
      window.electronAPI.remove('config-recieved');
    };
  }, []);

  return (
    <Card title="CONFIG" bordered={false} style={{ margin: '16px' }}>
      <Divider>基準気圧</Divider>
      <Row gutter={8} wrap={false} align="middle">
        <Col flex="auto">
          <Input addonAfter="hPa" readOnly value={activeBasePrassure} />
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
          <Input addonAfter="sec" readOnly value={activeSeparationMinimum} />
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
          <Input addonAfter="sec" readOnly value={activeSeparationMaximum} />
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
