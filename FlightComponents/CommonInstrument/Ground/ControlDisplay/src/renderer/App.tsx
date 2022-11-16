import 'antd/dist/antd.css';
import { Col, Layout, Row } from 'antd';
import SerialMonitor from './components/SerialMonitor';
import Status from './components/Status';
import './App.css';

const { Content } = Layout;

const App = () => {
  return (
    <Content style={{ margin: '32px' }}>
      <Row gutter={32}>
        <Col span={8}>
          <Status />
        </Col>
        <Col span={16}>
          <SerialMonitor />
        </Col>
      </Row>
    </Content>
  );
};

export default App;
