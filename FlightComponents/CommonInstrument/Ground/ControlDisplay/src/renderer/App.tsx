import 'antd/dist/antd.css';
import { Layout } from 'antd';
import SerialMonitor from './components/SerialMonitor';
import './App.css';

const { Header, Footer, Sider, Content } = Layout;

const App = () => {
  return <SerialMonitor />;
};

export default App;
