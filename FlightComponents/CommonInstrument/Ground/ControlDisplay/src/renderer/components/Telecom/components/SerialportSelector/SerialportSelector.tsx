import { Select } from 'antd';
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
    <>
      <Select
        style={{ width: '256px' }}
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
      <Select
        style={{ width: '256px' }}
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
    </>
  );
};

export default SerialportSelector;
