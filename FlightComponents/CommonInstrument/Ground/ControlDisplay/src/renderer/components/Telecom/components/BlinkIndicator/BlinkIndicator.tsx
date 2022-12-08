import { Divider } from 'antd';
import { useState, useEffect } from 'react';
import Indicator from './components/Indicator';

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
      <Divider>Uplink</Divider>
      <div style={{ display: 'flex', alignItems: 'center' }}>
        <span>Config :</span>
        <Indicator isActive={configUpState} />
      </div>
      <Divider>Downlink</Divider>
      <div style={{ display: 'flex', alignItems: 'center' }}>
        <span>FlightData :</span>
        <Indicator isActive={flightDataDownState} />
        <span>Config :</span>
        <Indicator isActive={configDownState} />
        <span>Status :</span>
        <Indicator isActive={statusDownState} />
      </div>
      <div style={{ display: 'flex', alignItems: 'center' }}>
        <span>Event :</span>
        <Indicator isActive={eventDownState} />
        <span>GPS :</span>
        <Indicator isActive={gpsDownState} />
      </div>
    </>
  );
};

export default BlinkIndicator;
