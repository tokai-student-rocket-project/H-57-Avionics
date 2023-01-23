import { Card } from 'antd';
import { useEffect, useState } from 'react';
import { FaSatellite } from 'react-icons/fa';

const DegToDms = (degrees: number): string => {
  const minits = (degrees - Math.floor(degrees)) * 60;
  const seconds = (minits - Math.floor(minits)) * 60;
  return `${Math.floor(degrees)}° ${Math.floor(seconds)}' ${
    Math.floor(seconds * 100) / 100
  }" `;
};

const GNSS = () => {
  const [latitude, setLatitude] = useState<number>(42.514179);
  const [longitude, setLongitude] = useState<number>(143.439731);

  const [sattelites, setSattelites] = useState<number>(4);

  useEffect(() => {
    window.electronAPI.gnssRecieved(() => {
      setLatitude(Number(window.electronAPI.store.get('latitude')));
      setLongitude(Number(window.electronAPI.store.get('longitude')));
      setSattelites(Number(window.electronAPI.store.get('satellites')));

      return () => {
        window.electronAPI.remove('gnss-recieved');
      };
    });
  }, []);

  return (
    <Card
      bordered={false}
      style={{ margin: '8px', width: '225px', height: 'auto' }}
    >
      <div style={{ margin: '8px -16px -16px -16px', display: 'flex' }}>
        <div>
          <div style={{ fontSize: '1.25em' }}>GNSS</div>
          <div style={{ display: 'flex', alignItems: 'center' }}>
            <FaSatellite
              style={{ marginLeft: '16px' }}
              size={14}
              color="#b9bbbe"
            />
            <span
              style={{
                fontSize: '1em',
                color: sattelites === 0 ? '#ed4245' : 'white',
                marginLeft: '4px',
              }}
            >
              {sattelites}
            </span>
          </div>
        </div>
        <div
          style={{ textAlign: 'center', fontSize: '1.2em', marginLeft: '16px' }}
        >
          <div>
            {`${DegToDms(Math.abs(latitude))}${
              Math.sign(latitude) === -1 ? 'S' : 'N'
            }`}
          </div>
          <div>
            {`${DegToDms(Math.abs(longitude))}${
              Math.sign(longitude) === -1 ? 'W' : 'E'
            }`}
          </div>
        </div>
      </div>
    </Card>
  );
};

export default GNSS;
