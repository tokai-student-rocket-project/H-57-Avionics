import { useState, useEffect } from 'react';
import wireframeThrusting from '../images/wireframe/thrusting.png';
import wireframeNormal from '../images/wireframe/normal.png';
import wireframeSeperated from '../images/wireframe/separated.png';

const getWireframe = (flightTime: number): string => {
  if (flightTime === 0) return wireframeNormal;
  if (flightTime <= 4) return wireframeThrusting;
  if (flightTime <= 10) return wireframeNormal;
  return wireframeSeperated;
};

const Wireframe = () => {
  const [flightTime, setFlightTime] = useState<number>(0);

  useEffect(() => {
    window.electronAPI.flightDataRecieved(() => {
      setFlightTime(Number(window.electronAPI.store.get('flight-time')));
    });
    return () => {
      window.electronAPI.remove('flight-data-recieved');
    };
  }, []);

  return (
    <div style={{ display: 'flex', justifyContent: 'center' }}>
      <img height={600} src={getWireframe(flightTime)} alt="wireframe" />
    </div>
  );
};

export default Wireframe;
