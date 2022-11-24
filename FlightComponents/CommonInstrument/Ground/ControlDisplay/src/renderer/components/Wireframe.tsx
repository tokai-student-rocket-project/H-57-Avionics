import { useState, useEffect } from 'react';
import wireframeThrusting from '../images/wireframe/thrusting.png';
import wireframeNormal from '../images/wireframe/normal.png';
import wireframeSeperated from '../images/wireframe/separated.png';

const getWireframe = (flightMode: string): string => {
  if (flightMode === 'THRUST') return wireframeThrusting;
  if (flightMode === 'PARASHUTE') return wireframeSeperated;
  return wireframeNormal;
};

const Wireframe = () => {
  const [flightMode, setFlightMode] = useState<string>('');

  useEffect(() => {
    window.electronAPI.statusRecieved(() => {
      setFlightMode(
        ['STANDBY', 'THRUST', 'CLIMB', 'DESCENT', 'PARASHUTE'][
          Number(window.electronAPI.store.get('flight-mode'))
        ]
      );
    });
    return () => {
      window.electronAPI.remove('statu-recieved');
    };
  }, []);

  return (
    <div style={{ display: 'flex', justifyContent: 'center' }}>
      <img height={600} src={getWireframe(flightMode)} alt="wireframe" />
    </div>
  );
};

export default Wireframe;
