import { Card, Col, Row, Statistic, Select, Button, Input } from 'antd';
import { useState, useEffect } from 'react';

const units = { a: 'm', p: 'hPa', bt: 'sec', sp: 'sec', fs: 'sec', l: 'sec' };

const Config = () => {
  const [separationAltitude, setSeparationAltitude] = useState<string>('');
  const [basePrassure, setBasePressure] = useState<string>('');
  const [burnTime, setBurnTime] = useState<string>('');
  const [separationProtection, setSeparationProtection] = useState<string>('');
  const [forceSeparation, setForceSeparation] = useState<string>('');
  const [landingTime, setLandingTime] = useState<string>('');

  const [selectedLabel, setSelectedLabel] = useState<string>('');
  const [value, setValue] = useState<string>('');

  useEffect(() => {
    window.electronAPI.configRecieved(() => {
      setSeparationAltitude(
        window.electronAPI.store.get('separation-altitude')
      );
      setBasePressure(window.electronAPI.store.get('base-pressure'));
      setBurnTime(window.electronAPI.store.get('burn-time'));
      setSeparationProtection(
        window.electronAPI.store.get('separation-protection')
      );
      setForceSeparation(window.electronAPI.store.get('force-separation'));
      setLandingTime(window.electronAPI.store.get('landing-time'));
    });

    return () => {
      window.electronAPI.remove('config-recieved');
    };
  }, []);

  const selectLabel = (label: string) => {
    setSelectedLabel(label);
  };

  const sendConfig = () => {
    if (selectedLabel === 'a')
      window.electronAPI.sendConfig(selectedLabel, value);
    if (selectedLabel === 'p')
      window.electronAPI.sendConfig(selectedLabel, value);
    if (selectedLabel === 'bt')
      window.electronAPI.sendConfig(selectedLabel, value);
    if (selectedLabel === 'sp')
      window.electronAPI.sendConfig(selectedLabel, value);
    if (selectedLabel === 'fs')
      window.electronAPI.sendConfig(selectedLabel, value);
    if (selectedLabel === 'l')
      window.electronAPI.sendConfig(selectedLabel, value);
  };

  return (
    <Card title="CONFIG" bordered={false} style={{ margin: '8px' }}>
      <Row style={{ marginTop: '24px' }}>
        <Col span={12}>
          <Statistic
            title="指定分離高度"
            suffix="m"
            valueStyle={{ color: 'white' }}
            value={separationAltitude}
          />
        </Col>
        <Col span={12}>
          <Statistic
            title="基準気圧"
            suffix="hPa"
            valueStyle={{ color: 'white' }}
            value={basePrassure}
          />
        </Col>
      </Row>
      <Row style={{ marginTop: '12px' }}>
        <Col span={12}>
          <Statistic
            title="想定燃焼時間"
            suffix="sec"
            valueStyle={{ color: 'white' }}
            value={burnTime}
          />
        </Col>
        <Col span={12}>
          <Statistic
            title="分離保護時間"
            suffix="sec"
            valueStyle={{ color: 'white' }}
            value={separationProtection}
          />
        </Col>
      </Row>
      <Row style={{ marginTop: '12px' }}>
        <Col span={12}>
          <Statistic
            title="強制分離時間"
            suffix="sec"
            valueStyle={{ color: 'white' }}
            value={forceSeparation}
          />
        </Col>
        <Col span={12}>
          <Statistic
            title="想定着地時間"
            suffix="sec"
            valueStyle={{ color: 'white' }}
            value={landingTime}
          />
        </Col>
      </Row>
      <div style={{ display: 'flex', marginTop: '24px' }}>
        <Select
          onSelect={selectLabel}
          style={{ width: '300px' }}
          options={[
            { value: 'a', label: '指定分離高度' },
            { value: 'p', label: '基準気圧' },
            { value: 'bt', label: '想定燃焼時間' },
            { value: 'sp', label: '分離保護時間' },
            { value: 'fs', label: '強制分離時間' },
            { value: 'l', label: '想定着地時' },
          ]}
        />
        <Input
          onChange={(event) => setValue(event.target.value)}
          addonAfter={units[selectedLabel]}
          style={{ height: '32px' }}
        />
        <Button type="primary" style={{ width: '64px' }} onClick={sendConfig}>
          送信
        </Button>
      </div>
    </Card>
  );
};

export default Config;
