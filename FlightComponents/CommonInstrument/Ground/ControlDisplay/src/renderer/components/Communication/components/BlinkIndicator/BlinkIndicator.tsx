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
  const [gnssDownState, setGnssDownState] = useState<boolean>(false);
  const [valveDownState, setValveDownState] = useState<boolean>(false);

  useEffect(() => {
    window.electronAPI.configSended(() => {
      blink(setConfigUpState);
    });

    return () => {
      window.electronAPI.remove('config-sended');
    };
  }, []);

  useEffect(() => {
    window.electronAPI.flightDataRecieved(() => {
      blink(setFlightDataDownState);
    });

    return () => {
      window.electronAPI.remove('flight-data-recieved');
    };
  }, []);

  useEffect(() => {
    window.electronAPI.configRecieved(() => {
      blink(setConfigDownState);
    });

    return () => {
      window.electronAPI.remove('config-recieved');
    };
  }, []);

  useEffect(() => {
    window.electronAPI.statusRecieved(() => {
      blink(setStatusDownState);
    });

    return () => {
      window.electronAPI.remove('status-recieved');
    };
  }, []);

  useEffect(() => {
    window.electronAPI.gnssRecieved(() => {
      blink(setGnssDownState);
    });
    return () => {
      window.electronAPI.remove('gnss-recieved');
    };
  }, []);

  useEffect(() => {
    window.electronAPI.valveRecieved(() => {
      blink(setValveDownState);
    });

    return () => {
      window.electronAPI.remove('valve-recieved');
    };
  }, []);

  useEffect(() => {
    window.electronAPI.eventRecieved(() => {
      blink(setEventDownState);
    });

    return () => {
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
        <span>GNSS :</span>
        <Indicator isActive={gnssDownState} />
        <span>Valve :</span>
        <Indicator isActive={valveDownState} />
      </div>
    </>
  );
};

export default BlinkIndicator;
