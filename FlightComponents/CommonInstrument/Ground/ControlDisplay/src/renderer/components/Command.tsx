import { Button, Card, Space } from 'antd';

const Command = () => {
  return (
    <Card style={{ marginTop: '32px' }} title="COMMAND" bordered={false}>
      <Space direction="vertical" style={{ width: '100%' }}>
        <Button>初期化</Button>
        <Button>ヘルスチェック</Button>
      </Space>
    </Card>
  );
};

export default Command;
