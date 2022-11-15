import { Select, Card, Input } from 'antd';

const { TextArea } = Input;

const SerialMonitor = () => {
  return (
    <Card title="SERIAL MONITOR" bordered={false} style={{ margin: '32px' }}>
      <Select placeholder="シリアルポートを選択" style={{ margin: '4px' }} />
      <TextArea style={{ margin: '4px', height: '400px' }} />
    </Card>
  );
};

export default SerialMonitor;
