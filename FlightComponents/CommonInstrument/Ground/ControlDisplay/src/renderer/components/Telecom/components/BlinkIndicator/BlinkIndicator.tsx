import { Divider } from 'antd';
import { useState, useEffect } from 'react';

const activeColor = '#46c46d';
const disactiveColor = '#72767d';

const delay = (ms: number) => new Promise((resolve) => setTimeout(resolve, ms));
const blink = async (set: React.Dispatch<React.SetStateAction<boolean>>) => {
  set(true);
  await delay(100);
  set(false);
};

const BlinkIndicator = () => {
  const [configUpState, setConfigUpState] = useState<boolean>(false);
  const [flightDataDownState, setFlightDataDownState] =
    useState<boolean>(false);
  const [configDownState, setConfigDownState] = useState<boolean>(false);
  const [statusDownState, setStatusDownState] = useState<boolean>(false);
  const [eventDownState, setEventDownState] = useState<boolean>(false);
  const [gpsDownState, setGpsDownState] = useState<boolean>(false);

  useEffect(() => {
    window.electronAPI.configSended(() => {
      blink(setConfigUpState);
    });

    window.electronAPI.flightDataRecieved(() => {
      blink(setFlightDataDownState);
    });

    window.electronAPI.configRecieved(() => {
      blink(setConfigDownState);
    });

    window.electronAPI.statusRecieved(() => {
      blink(setStatusDownState);
    });

    window.electronAPI.telemetryRecieved(() => {
      blink(setGpsDownState);
    });

    window.electronAPI.eventRecieved(() => {
      blink(setEventDownState);
    });

    return () => {
      window.electronAPI.remove('config-sended');
      window.electronAPI.remove('flight-data-recieved');
      window.electronAPI.remove('config-recieved');
      window.electronAPI.remove('status-recieved');
      window.electronAPI.remove('teremetry-recieved');
      window.electronAPI.remove('event-recieved');
    };
  }, []);

  return (
    <>
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
    </>
  );
};

export default BlinkIndicator;
