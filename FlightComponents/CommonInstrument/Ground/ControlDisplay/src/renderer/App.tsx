import 'antd/dist/antd.css';
import { Col, Layout, Row } from 'antd';
import SerialMonitor from './components/SerialMonitor';
import Status from './components/Status';
import './App.css';
import Config from './components/Config';
import wireframe from './wireframe.png';
import Command from './components/Command';
import FlightData from './components/FlightData';
import Badge from './components/Badge';
import FlightMode from './components/FlightMode';

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

      <Content style={{ margin: '32px' }}>
        <Row gutter={32}>
          <Col span={8}>
            <Config />
          </Col>
          <Col span={4}>
            <div style={{ display: 'flex', justifyContent: 'center' }}>
              <img height={600} src={wireframe} alt="wireframe" />
            </div>
          </Col>
          <Col span={4}>
            <Status />
            <Command />
          </Col>
          <Col span={8}>
            <SerialMonitor />
          </Col>
        </Row>
      </Content>
    </Layout>
  );
};

export default App;
