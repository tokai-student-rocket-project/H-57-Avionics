/* eslint-disable jsx-a11y/no-static-element-interactions */
/* eslint-disable jsx-a11y/click-events-have-key-events */
/* eslint-disable no-nested-ternary */
import { Card, Col, Row, Statistic, Select, Button, Input, Modal } from 'antd';
import { useState, useEffect } from 'react';
import { FaGamepad, FaTimes } from 'react-icons/fa';
import {
  green,
  emphasisHigh,
  emphasisMidium,
  red,
  background16dp,
  background04dp,
} from '../../utilities/colors';

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

  const [isShowGame, setIsShowGame] = useState<boolean>(false);
  const [stones, setStones] = useState<number[]>([0, 0, 0, 0, 0, 0, 0, 0, 0]);

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

  const selectStoneIndex = (e: React.MouseEvent<HTMLDivElement>) => {
    const targetIndex = Number(e.currentTarget.dataset.num);
    setStones((originStones) => {
      const newStones = originStones.map((stone, index) =>
        index === targetIndex ? 1 : stone
      );

      let cpuIndex = 0;
      // eslint-disable-next-line no-constant-condition
      while (true) {
        cpuIndex = Math.floor(Math.random() * 10);

        if (newStones[cpuIndex] === 0) break;
        if (Math.min.apply(null, newStones) === 1) return newStones;
      }

      return newStones.map((stone, index) => (index === cpuIndex ? 2 : stone));
    });
  };

  const clearStones = () => {
    setStones([0, 0, 0, 0, 0, 0, 0, 0, 0]);
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
    <>
      <Card
        title="CONFIG"
        bordered={false}
        extra={
          <div style={{ display: 'flex', alignItems: 'center' }}>
            <FaGamepad
              cursor="pointer"
              size={22}
              color={emphasisHigh}
              onClick={() => setIsShowGame(true)}
            />
          </div>
        }
      >
        <Row>
          <Col span={12}>
            {separationAltitude === '0.0' ? (
              <Statistic
                title="指定分離高度"
                valueStyle={{ color: 'white' }}
                value="頂点分離"
              />
            ) : (
              <Statistic
                title="指定分離高度"
                valueStyle={{ color: 'white' }}
                suffix="m"
                value={separationAltitude}
              />
            )}
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
              title="想定モータ動作時間"
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
      <Modal
        open={isShowGame}
        onCancel={() => setIsShowGame(false)}
        destroyOnClose
        maskClosable
        afterClose={clearStones}
        footer={null}
        bodyStyle={{ background: background16dp, color: 'white' }}
        closeIcon={<FaTimes color="white" />}
        width={500}
      >
        <div
          style={{
            margin: '32px',
            display: 'grid',
            gridGap: '10px',
            gridTemplateColumns: '1fr 1fr 1fr',
            gridTemplateRows: '100px 100px 100px',
          }}
        >
          {stones.map((stone, index) => (
            <div
              // eslint-disable-next-line react/no-array-index-key
              key={index}
              style={{
                background: stone === 0 ? background16dp : background04dp,
                border: stone === 0 ? `1px solid ${emphasisMidium}` : 'none',
                padding: '15px',
                borderRadius: '8px',
                display: 'flex',
                justifyContent: 'center',
                alignItems: 'center',
                fontSize: '2em',
                fontWeight: 'bold',
              }}
            >
              {stone === 1 ? (
                <span
                  style={{
                    color: green,
                    transform: 'rotate(-15deg)',
                  }}
                >
                  GO!
                </span>
              ) : stone === 2 ? (
                <span
                  style={{
                    color: red,
                    transform: 'rotate(-15deg)',
                  }}
                >
                  NOGO!
                </span>
              ) : (
                <div
                  style={{
                    height: '100%',
                    width: '100%',
                    cursor: 'pointer',
                  }}
                  data-num={index}
                  onClick={selectStoneIndex}
                />
              )}
            </div>
          ))}
        </div>
      </Modal>
    </>
  );
};

export default Config;
