import 'antd/dist/antd.css';
import { Col, Layout, Row } from 'antd';
import Status from './components/Status';
import './App.css';
import Config from './components/Config';
import FlightData from './components/FlightData';
import Badge from './components/Badge';
import FlightMode from './components/Blank';
import Telecom from './components/Telecom';
import GPS from './components/GPS';
import Valve from './components/Valve';
import Wireframe from './components/Wireframe';

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
            <Telecom />
            <Config />
          </Col>
          <Col span={4}>
            <Wireframe />
          </Col>
          <Col span={4}>
            <Status />
            <Valve />
          </Col>
          <Col span={8}>
            <GPS />
          </Col>
        </Row>
      </Content>
    </Layout>
  );
};

export default App;
