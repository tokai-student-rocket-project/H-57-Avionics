import { Select, Card, Input } from 'antd';
import { useState } from 'react';
import {
  FaPlay,
  FaStop,
  FaCog,
  FaLevelDownAlt,
  FaCode,
  FaEraser,
} from 'react-icons/fa';

const { Option } = Select;
const { TextArea } = Input;

const SerialMonitor = () => {
  const [serialports, setSerialports] = useState<string[]>([]);
  const [serialport, setSerialport] = useState<string>('');
  const [data, setData] = useState<string>('');

  const getSerialports = async () => {
    setSerialports(await window.electronAPI.getSerialports());
  };

  const selectSerialport = (selectedSerialport: string) => {
    setSerialport(selectedSerialport);
    window.electronAPI.oepnSerialport(selectedSerialport);
  };

  const openSerialport = () => {
    if (serialport) window.electronAPI.oepnSerialport(serialport);
  };

  window.electronAPI.recievedData((_, recievedData) => {
    // setData(`${recievedData}${data}`);
  });

  return (
    <Card title="SERIAL MONITOR" bordered={false}>
      <div
        style={{
          marginBottom: '16px',
          display: 'flex',
          alignItems: 'center',
        }}
      >
        <Select
          placeholder="シリアルポートを選択"
          onClick={getSerialports}
          onSelect={selectSerialport}
          style={{ width: '100%', marginRight: '8px' }}
        >
          {serialports.map((serialportItr) => (
            <Option key={serialportItr} value={serialportItr}>
              {serialportItr}
            </Option>
          ))}
        </Select>
        <FaPlay
          size={18}
          cursor="pointer"
          color="#46c46d"
          style={{ marginRight: '8px' }}
          onClick={openSerialport}
        />
        <FaStop
          size={18}
          cursor="pointer"
          color="#ed4245"
          style={{ marginRight: '8px' }}
        />
        {/* <FaEraser size={18} cursor="pointer" style={{ marginRight: '8px' }} /> */}
        <FaLevelDownAlt
          size={18}
          cursor="pointer"
          color="#b9bbbe"
          style={{ marginRight: '8px' }}
        />
        {/* <FaCode size={18} cursor="pointer" style={{ marginRight: '8px' }} /> */}
        <FaCog
          size={18}
          cursor="pointer"
          color="#b9bbbe"
          style={{ marginRight: '8px' }}
        />
      </div>
      <TextArea readOnly style={{ height: '400px' }} value={data} />
    </Card>
  );
};

export default SerialMonitor;
