import { Card, Select, Divider, Tooltip, Popover } from 'antd';
import { useState, useEffect } from 'react';
import { FaRss, FaCog } from 'react-icons/fa';

const { Option } = Select;

const activeColor = '#46c46d';
const disactiveColor = '#72767d';

const rssiToColor = (rssi: number): string => {
  if (rssi < -100) return '#ed4245';
  if (rssi < -70) return '#faa61a';
  return '#46c46d';
};

const delay = (ms: number) => new Promise((resolve) => setTimeout(resolve, ms));
const blinkIndicator = async (
  set: React.Dispatch<React.SetStateAction<boolean>>
) => {
  set(true);
  await delay(100);
  set(false);
};

const Telecom = () => {
  const [serialports, setSerialports] = useState<string[]>([]);

  const [configUpState, setConfigUpState] = useState<boolean>(false);
  const [flightDataDownState, setFlightDataDownState] =
    useState<boolean>(false);
  const [configDownState, setConfigDownState] = useState<boolean>(false);
  const [statusDownState, setStatusDownState] = useState<boolean>(false);
  const [eventDownState, setEventDownState] = useState<boolean>(false);
  const [gpsDownState, setGpsDownState] = useState<boolean>(false);

  const [rssi, setRssi] = useState<number>(0);

  const getSerialports = async () => {
    setSerialports(await window.electronAPI.getSerialports());
  };

  const selectSerialport = (selectedSerialport: string) => {
    window.electronAPI.oepnSerialport(selectedSerialport);
  };

  const selectSerialportTelemeter = (selectedSerialport: string) => {
    window.electronAPI.oepnSerialportTelemeter(selectedSerialport);
  };

  useEffect(() => {
    window.electronAPI.flightDataRecieved(() => {
      blinkIndicator(setFlightDataDownState);
    });

    window.electronAPI.configRecieved(() => {
      blinkIndicator(setConfigDownState);
    });

    window.electronAPI.statusRecieved(() => {
      blinkIndicator(setStatusDownState);
    });

    window.electronAPI.telemetryRecieved(() => {
      blinkIndicator(setGpsDownState);
    });

    window.electronAPI.rssiRecieved(() => {
      setRssi(window.electronAPI.store.get('rssi'));
    });

    window.electronAPI.eventRecieved(() => {
      blinkIndicator(setEventDownState);
    });

    return () => {
      window.electronAPI.remove('flight-data-recieved');
      window.electronAPI.remove('config-recieved');
      window.electronAPI.remove('status-recieved');
      window.electronAPI.remove('rssi-recieved');
      window.electronAPI.remove('teremetry-recieved');
    };
  }, []);

  const serialportSetting = (
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

  return (
    <Card
      title="TELECOM"
      bordered={false}
      style={{ margin: '16px' }}
      extra={
        <div style={{ display: 'flex', alignItems: 'center' }}>
          <Popover
            content={serialportSetting}
            placement="bottom"
            trigger="click"
          >
            <FaCog
              cursor="pointer"
              style={{ marginLeft: '16px' }}
              size={16}
              color="#b9bbbe"
            />
          </Popover>
          <Tooltip placement="bottom" title={rssi}>
            <FaRss
              style={{ marginLeft: '16px' }}
              size={16}
              cursor="pointer"
              color={rssiToColor(rssi)}
            />
          </Tooltip>
        </div>
      }
    >
      <Divider>アップリンク</Divider>
      <div style={{ display: 'flex', alignItems: 'center' }}>
        <span>Config :</span>
        <div
          style={{
            width: '12px',
            margin: 'auto 16px auto 8px',
            height: '12px',
            backgroundColor: configUpState ? activeColor : disactiveColor,
            borderRadius: '50%',
          }}
        />
      </div>
      <Divider>ダウンリンク</Divider>
      <div style={{ display: 'flex', alignItems: 'center' }}>
        <span>FlightData :</span>
        <div
          style={{
            width: '12px',
            margin: 'auto 16px auto 8px',
            height: '12px',
            backgroundColor: flightDataDownState ? activeColor : disactiveColor,
            borderRadius: '50%',
          }}
        />
        <span>Config :</span>
        <div
          style={{
            width: '12px',
            margin: 'auto 16px auto 8px',
            height: '12px',
            backgroundColor: configDownState ? activeColor : disactiveColor,
            borderRadius: '50%',
          }}
        />
        <span>Status :</span>
        <div
          style={{
            width: '12px',
            margin: 'auto 16px auto 8px',
            height: '12px',
            backgroundColor: statusDownState ? activeColor : disactiveColor,
            borderRadius: '50%',
          }}
        />
      </div>
      <div style={{ display: 'flex', alignItems: 'center' }}>
        <span>Event :</span>
        <div
          style={{
            width: '12px',
            margin: 'auto 16px auto 8px',
            height: '12px',
            backgroundColor: eventDownState ? activeColor : disactiveColor,
            borderRadius: '50%',
          }}
        />
        <span>GPS :</span>
        <div
          style={{
            width: '12px',
            margin: 'auto 16px auto 8px',
            height: '12px',
            backgroundColor: gpsDownState ? activeColor : disactiveColor,
            borderRadius: '50%',
          }}
        />
      </div>
    </Card>
  );
};

export default Telecom;
