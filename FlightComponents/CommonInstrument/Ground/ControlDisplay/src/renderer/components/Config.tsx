import { Card, Input, Button, Row, Col, Divider } from 'antd';
import { useState, useEffect } from 'react';
import { FaArrowLeft } from 'react-icons/fa';

const Config = () => {
  const [activeBasePrassure, setActiveBasePressure] = useState<string>('');
  const [activeBurnTime, setActiveBurnTime] = useState<string>('');
  const [activeSeparationMinimum, setActiveSeparationMinimum] =
    useState<string>('');
  const [activeSeparationMaximum, setActiveSeparationMaximum] =
    useState<string>('');

  const [standbyBasePrassure, setStandbyBasePressure] = useState<string>('');
  const [standbyBurnTime, setStandbyBurnTime] = useState<string>('');
  const [standbySeparationMinimum, setStandbySeparationMinimum] =
    useState<string>('');
  const [standbySeparationMaximum, setStandbySeparationMaximum] =
    useState<string>('');

  useEffect(() => {
    window.electronAPI.configRecieved(() => {
      setActiveBasePressure(window.electronAPI.store.get('base-pressure'));
      setActiveBurnTime(window.electronAPI.store.get('burn-time'));
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

  const sendConfig = (event: React.MouseEvent<HTMLInputElement>) => {
    const label = event.currentTarget.getAttribute('data-num');
    if (label === 'p')
      window.electronAPI.sendConfig(label, standbyBasePrassure);
    if (label === 'b') window.electronAPI.sendConfig(label, standbyBurnTime);
    if (label === 'smin')
      window.electronAPI.sendConfig(label, standbySeparationMinimum);
    if (label === 'smax')
      window.electronAPI.sendConfig(label, standbySeparationMaximum);
  };

  return (
    <Card title="CONFIG" bordered={false} style={{ margin: '16px' }}>
      <Divider>基準気圧</Divider>
      <Row gutter={8} wrap={false} align="middle">
        <Col flex="auto">
          <Input addonAfter="hPa" readOnly value={activeBasePrassure} />
        </Col>
        <Col>
          <Button type="primary" onClick={sendConfig} data-num="p">
            <FaArrowLeft />
          </Button>
        </Col>
        <Col flex="auto">
          <Input
            addonAfter="hPa"
            value={standbyBasePrassure}
            onChange={(event) => setStandbyBasePressure(event.target.value)}
          />
        </Col>
      </Row>
      <Divider>想定燃焼時間</Divider>
      <Row gutter={8} wrap={false} align="middle">
        <Col flex="auto">
          <Input addonAfter="sec" readOnly value={activeBurnTime} />
        </Col>
        <Col>
          <Button type="primary" onClick={sendConfig} data-num="b">
            <FaArrowLeft />
          </Button>
        </Col>
        <Col flex="auto">
          <Input
            addonAfter="sec"
            value={standbyBurnTime}
            onChange={(event) => setStandbyBurnTime(event.target.value)}
          />
        </Col>
      </Row>
      <Divider>最短分離時間</Divider>
      <Row gutter={8} wrap={false} align="middle">
        <Col flex="auto">
          <Input addonAfter="sec" readOnly value={activeSeparationMinimum} />
        </Col>
        <Col>
          <Button type="primary" onClick={sendConfig} data-num="smin">
            <FaArrowLeft />
          </Button>
        </Col>
        <Col flex="auto">
          <Input
            addonAfter="sec"
            value={standbySeparationMinimum}
            onChange={(event) =>
              setStandbySeparationMinimum(event.target.value)
            }
          />
        </Col>
      </Row>
      <Divider>最長分離時間</Divider>
      <Row gutter={8} wrap={false} align="middle">
        <Col flex="auto">
          <Input addonAfter="sec" readOnly value={activeSeparationMaximum} />
        </Col>
        <Col>
          <Button type="primary" onClick={sendConfig} data-num="smax">
            <FaArrowLeft />
          </Button>
        </Col>
        <Col flex="auto">
          <Input
            addonAfter="sec"
            value={standbySeparationMaximum}
            onChange={(event) =>
              setStandbySeparationMaximum(event.target.value)
            }
          />
        </Col>
      </Row>
    </Card>
  );
};

export default Config;
