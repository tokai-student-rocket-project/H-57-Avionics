import { Card, Input, Button, Row, Col, Divider } from 'antd';
import { useState, useEffect } from 'react';
import { FaArrowLeft } from 'react-icons/fa';

const Config = () => {
  const [activeSeparationAltitude, setActiveSeparationAltitude] =
    useState<string>('');
  const [activeBasePrassure, setActiveBasePressure] = useState<string>('');
  const [activeBurnTime, setActiveBurnTime] = useState<string>('');
  const [activeSeparationProtection, setActiveSeparationProtection] =
    useState<string>('');
  const [activeForceSeparation, setActiveForceSeparation] =
    useState<string>('');

  const [standbySeparationAltitude, setStandbySeparationAltitude] =
    useState<string>('');
  const [standbyBasePrassure, setStandbyBasePressure] = useState<string>('');
  const [standbyBurnTime, setStandbyBurnTime] = useState<string>('');
  const [standbySeparationProtection, setStandbySeparationProtection] =
    useState<string>('');
  const [standbyForceSeparation, setStandbyForceSeparation] =
    useState<string>('');

  useEffect(() => {
    window.electronAPI.configRecieved(() => {
      setActiveSeparationAltitude(
        window.electronAPI.store.get('separation-altitude')
      );
      setActiveBasePressure(window.electronAPI.store.get('base-pressure'));
      setActiveBurnTime(window.electronAPI.store.get('burn-time'));
      setActiveSeparationProtection(
        window.electronAPI.store.get('separation-protection')
      );
      setActiveForceSeparation(
        window.electronAPI.store.get('force-separation')
      );
    });
    return () => {
      window.electronAPI.remove('config-recieved');
    };
  }, []);

  const sendConfig = (event: React.MouseEvent<HTMLInputElement>) => {
    const label = event.currentTarget.getAttribute('data-num');
    if (label === 'a')
      window.electronAPI.sendConfig(label, standbySeparationAltitude);
    if (label === 'p')
      window.electronAPI.sendConfig(label, standbyBasePrassure);
    if (label === 'b') window.electronAPI.sendConfig(label, standbyBurnTime);
    if (label === 'sp')
      window.electronAPI.sendConfig(label, standbySeparationProtection);
    if (label === 'fs')
      window.electronAPI.sendConfig(label, standbyForceSeparation);
  };

  return (
    <Card title="CONFIG" bordered={false} style={{ margin: '16px' }}>
      <Divider>指定分離高度</Divider>
      <Row gutter={8} wrap={false} align="middle">
        <Col flex="auto">
          <Input addonAfter="m" readOnly value={activeSeparationAltitude} />
        </Col>
        <Col>
          <Button type="primary" onClick={sendConfig} data-num="a">
            <FaArrowLeft />
          </Button>
        </Col>
        <Col flex="auto">
          <Input
            addonAfter="m"
            value={standbySeparationAltitude}
            onChange={(event) =>
              setStandbySeparationAltitude(event.target.value)
            }
          />
        </Col>
      </Row>
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
      <Divider>分離保護時間</Divider>
      <Row gutter={8} wrap={false} align="middle">
        <Col flex="auto">
          <Input addonAfter="sec" readOnly value={activeSeparationProtection} />
        </Col>
        <Col>
          <Button type="primary" onClick={sendConfig} data-num="sp">
            <FaArrowLeft />
          </Button>
        </Col>
        <Col flex="auto">
          <Input
            addonAfter="sec"
            value={standbySeparationProtection}
            onChange={(event) =>
              setStandbySeparationProtection(event.target.value)
            }
          />
        </Col>
      </Row>
      <Divider>強制分離時間</Divider>
      <Row gutter={8} wrap={false} align="middle">
        <Col flex="auto">
          <Input addonAfter="sec" readOnly value={activeForceSeparation} />
        </Col>
        <Col>
          <Button type="primary" onClick={sendConfig} data-num="fs">
            <FaArrowLeft />
          </Button>
        </Col>
        <Col flex="auto">
          <Input
            addonAfter="sec"
            value={standbyForceSeparation}
            onChange={(event) => setStandbyForceSeparation(event.target.value)}
          />
        </Col>
      </Row>
    </Card>
  );
};

export default Config;
