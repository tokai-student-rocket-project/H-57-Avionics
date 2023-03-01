import { Select, Descriptions } from 'antd';
import { useState } from 'react';

const { Option } = Select;

const SerialportSelector = () => {
  const [serialports, setSerialports] = useState<string[]>([]);

  const getSerialports = async () => {
    setSerialports(await window.electronAPI.getSerialports());
  };

  const selectSerialport = (selectedSerialport: string) => {
    window.electronAPI.oepnSerialport(selectedSerialport);
  };

  const selectSerialportTelemeter = (selectedSerialport: string) => {
    window.electronAPI.oepnSerialportTelemeter(selectedSerialport);
  };

  return (
    <div style={{ width: '300px' }}>
      <Descriptions
        column={1}
        labelStyle={{ color: 'white' }}
        layout="vertical"
      >
        <Descriptions.Item label="共通計器">
          <Select
            style={{ width: '100%' }}
            placeholder="シリアルポートを選択"
            onClick={getSerialports}
            onSelect={selectSerialport}
          >
            {serialports.map((serialportItr) => (
              <Option key={serialportItr} value={serialportItr}>
                {serialportItr}
              </Option>
            ))}
          </Select>
        </Descriptions.Item>
        <Descriptions.Item label="テレメータ">
          <Select
            style={{ width: '100%' }}
            placeholder="シリアルポートを選択"
            onClick={getSerialports}
            onSelect={selectSerialportTelemeter}
          >
            {serialports.map((serialportItr) => (
              <Option key={serialportItr} value={serialportItr}>
                {serialportItr}
              </Option>
            ))}
          </Select>
        </Descriptions.Item>
      </Descriptions>
    </div>
  );
};

export default SerialportSelector;
