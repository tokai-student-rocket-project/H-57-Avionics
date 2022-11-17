import 'antd/dist/antd.css';
import { Col, Layout, Row } from 'antd';
import SerialMonitor from './components/SerialMonitor';
import Status from './components/Status';
import './App.css';
import Config from './components/Config';
import wireframe from './wireframe.png';

const { Content } = Layout;

const App = () => {
  return (
    <Content style={{ margin: '32px' }}>
      <Row gutter={32}>
        <Col span={8}>
          <Config />
        </Col>
        <Col span={4}>
          <div style={{ display: 'flex', justifyContent: 'center' }}>
            <img height={800} src={wireframe} alt="wireframe" />
          </div>
        </Col>
        <Col span={4}>
          <Status />
        </Col>
        <Col span={8}>
          <SerialMonitor />
        </Col>
      </Row>
    </Content>
  );
};

export default App;
