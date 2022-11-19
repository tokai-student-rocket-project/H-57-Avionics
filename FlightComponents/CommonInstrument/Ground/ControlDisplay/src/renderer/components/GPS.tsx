import { Card } from 'antd';
import { useEffect, useState } from 'react';

const DegToDms = (degrees: number): string => {
  const minits = (degrees - Math.floor(degrees)) * 60;
  const seconds = (minits - Math.floor(minits)) * 60;
  return `${Math.floor(degrees)}° ${Math.floor(seconds)}' ${
    Math.floor(seconds * 100) / 100
  }" `;
};

const GPS = () => {
  const [latitude, setLatitude] = useState<string>('35° 0\' 0.00" N');
  const [lontitude, setLontitude] = useState<string>('135° 0\' 0.00" E');

  useEffect(() => {
    window.electronAPI.telemetryUpdated(() => {
      const latitudeDEG = window.electronAPI.store.get('latitude');
      const latitudeDMS = `${DegToDms(Math.abs(latitudeDEG))}${
        Math.sign(latitudeDEG) === -1 ? 'S' : 'N'
      }`;
      setLatitude(latitudeDMS);

      const lontitudeDEG = window.electronAPI.store.get('lontitude');
      const lontitudeDMS = `${DegToDms(Math.abs(lontitudeDEG))}${
        Math.sign(lontitudeDEG) === -1 ? 'W' : 'E'
      }`;
      setLontitude(lontitudeDMS);

      return () => {
        window.electronAPI.remove('telemetry-updated');
      };
    });
  }, []);

  return <Card title="GPS" bordered={false} style={{ margin: '16px' }} />;
};

export default GPS;
