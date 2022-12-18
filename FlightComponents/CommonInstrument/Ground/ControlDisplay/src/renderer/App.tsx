import 'antd/dist/antd.css';
import { Col, Layout, Row } from 'antd';
import Status from './components/Status';
import './App.css';
import Config from './components/Config/Config';
import FlightData from './components/FlightData';
import Badge from './components/Badge';
import FlightMode from './components/FlightMode';
import Telecom from './components/Telecom';
import GPS from './components/GPS';
import Valve from './components/Valve';
import AltitudeIndicator from './components/AltitudeIndicator';
import Electric from './components/Electric';

const { Header, Content } = Layout;

const App = () => {
  return (
    <Layout style={{ backgroundColor: 'transparent', height: '100%' }}>
      <Header
        style={{ backgroundColor: 'transparent', padding: '0', height: 'auto' }}
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

      <Content>
        <Row>
          <Col span={8}>
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
          <Col span={8}>
            <Row>
              <Col span={24}>
                <AltitudeIndicator />
              </Col>
            </Row>
            <Row>
              <Col span={24}>
                <GPS />
              </Col>
            </Row>
          </Col>
          <Col span={4}>
            <Electric />
            <Status />
          </Col>
          <Col span={4}>
            <Valve />
          </Col>
        </Row>
      </Content>
    </Layout>
  );
};

export default App;
