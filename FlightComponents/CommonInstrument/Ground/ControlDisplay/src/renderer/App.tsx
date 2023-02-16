import 'antd/dist/antd.css';
import { Col, Layout, Row, notification } from 'antd';
import { useState, useEffect } from 'react';
import Status from './components/Status';
import './App.css';
import Config from './components/Config/Config';
import FlightData from './components/FlightData';
import Badge from './components/Badge';
import FlightMode from './components/FlightMode';
import Telecom from './components/Telecom';
import GNSS from './components/GNSS';
import Valve from './components/Valve';
import AltitudeIndicator from './components/AltitudeIndicator';
import MapView from './components/MapView';
import Electric from './components/Electric';
import Mission from './components/Mission';
import Progress from './components/Progress';
import Emergency from './components/Emergency';

const { Header, Content } = Layout;

type EventItem = {
  id: number;
  flightTime: number;
  event: string;
};

const App = () => {
  return (
    <div style={{ position: 'relative', width: '100%', height: '100vh' }}>
      <MapView />
      <div
        style={{
          position: 'absolute',
          height: '100%',
          width: '100%',
          zIndex: '999',
        }}
      >
        <Layout
          style={{
            backgroundColor: 'transparent',
            height: '100%',
          }}
        >
          <Header
            style={{
              backgroundColor: 'transparent',
              padding: '0',
              height: 'auto',
            }}
          >
            <Row>
              <Col span={9}>
                <FlightData />
              </Col>
              <Col span={6}>
                <Badge />
              </Col>
              <Col span={9}>
                <FlightMode />
              </Col>
            </Row>
          </Header>

          <Content
            style={{
              padding: '0 8px 16px 8px',
            }}
          >
            <Row style={{ height: '100%' }}>
              <Col span={7}>
                <Row>
                  <Col span={24}>
                    <Telecom />
                  </Col>
                </Row>
                <Row>
                  <Col span={24}>
                    <Config />
                  </Col>
                </Row>
              </Col>
              <Col span={10}>
                <div
                  style={{
                    height: '100%',
                    display: 'grid',
                    gridTemplateRows: 'auto 1fr auto auto',
                    placeItems: 'center',
                  }}
                >
                  <Progress />
                  <div />
                  <div
                    style={{
                      display: 'flex',
                      width: '100%',
                      justifyContent: 'center',
                      alignItems: 'flex-end',
                    }}
                  >
                    <AltitudeIndicator />
                    <div>
                      <GNSS />
                      <Emergency />
                    </div>
                  </div>
                </div>
              </Col>
              <Col span={7}>
                <Row>
                  <Mission />
                </Row>
                <Row>
                  <Col span={12}>
                    <Electric />
                    <Status />
                  </Col>
                  <Col span={12}>
                    <Valve />
                  </Col>
                </Row>
              </Col>
            </Row>
          </Content>
        </Layout>
      </div>
    </div>
  );
};

export default App;
