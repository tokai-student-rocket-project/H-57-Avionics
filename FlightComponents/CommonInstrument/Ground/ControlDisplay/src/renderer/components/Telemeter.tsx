import { Card, Col, Divider, Row, Select } from 'antd';
import { useEffect, useState } from 'react';

const { Option } = Select;

const activeColor = '#46c46d';
const disactiveColor = '#72767d';

const DegToDms = (degrees: number): string => {
  const minits = (degrees - Math.floor(degrees)) * 60;
  const seconds = (minits - Math.floor(minits)) * 60;
  return `${Math.floor(degrees)}° ${Math.floor(seconds)}' ${
    Math.floor(seconds * 100) / 100
  }" `;
};

const delay = (ms: number) => new Promise((resolve) => setTimeout(resolve, ms));
const blinkIndicator = async (
  set: React.Dispatch<React.SetStateAction<boolean>>
) => {
  set(true);
  await delay(100);
  set(false);
};

const Telemeter = () => {
  const [downState, setDownState] = useState<boolean>(false);

  const [latitude, setLatitude] = useState<string>('35° 0\' 0.00" N');
  const [lontitude, setLontitude] = useState<string>('135° 0\' 0.00" E');
  const [mainServoDegrees, setMainServoDegrees] = useState<string>('0');
  const [supplyServoDegrees, setSupplyServoDegrees] = useState<string>('0');

  useEffect(() => {
    window.electronAPI.telemetryUpdated(() => {
      const latitudeDEG = window.electronAPI.store.get('latitude');
      const latitudeDMS = `${DegToDms(Math.abs(latitudeDEG))}${
        Math.sign(latitudeDEG) === -1 ? 'S' : 'N'
      }`;
      setLatitude(latitudeDMS);

      const lontitudeDEG = window.electronAPI.store.get('lontitude');
      const lontitudeDMS = `${DegToDms(Math.abs(lontitudeDEG))}${
        Math.sign(lontitudeDEG) === -1 ? 'W' : 'E'
      }`;
      setLontitude(lontitudeDMS);

      setMainServoDegrees(window.electronAPI.store.get('mainservo-degrees'));
      setSupplyServoDegrees(
        window.electronAPI.store.get('supplyservo-degrees')
      );

      blinkIndicator(setDownState);

      return () => {
        window.electronAPI.remove('telemetry-updated');
      };
    });
  }, []);

  return (
    <Card title="TELEMETER" bordered={false} style={{ margin: '16px' }}>
      <Row gutter={16}>
        <Col span={12}>
          <Divider>GPS</Divider>
          <div
            style={{
              fontSize: '1.2em',
              display: 'flex',
              margin: '0px 8px',
              justifyContent: 'flex-end',
            }}
          >
            {latitude}
          </div>
          <div
            style={{
              fontSize: '1.2em',
              display: 'flex',
              margin: '0px 8px',
              justifyContent: 'flex-end',
            }}
          >
            {lontitude}
          </div>
        </Col>
        <Col span={12}>
          <Divider>Valve</Divider>
        </Col>
      </Row>
    </Card>
  );
};

export default Telemeter;
