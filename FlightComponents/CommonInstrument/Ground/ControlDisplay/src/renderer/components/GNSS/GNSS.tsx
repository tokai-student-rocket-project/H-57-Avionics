import { Card } from 'antd';
import { useEffect, useState } from 'react';
import { FaSatellite } from 'react-icons/fa';
import { emphasisMidium } from '../../utilities/colors';

const DegToDms = (degrees: number): string => {
  const minutesSeconds =
    // eslint-disable-next-line prefer-template
    parseFloat('0.' + degrees.toString().split('.')[1]) * 60;
  return `${Math.trunc(degrees)}°${Math.trunc(minutesSeconds)}'${
    // eslint-disable-next-line prefer-template
    (parseFloat('0.' + minutesSeconds.toString().split('.')[1]) * 60).toFixed(2)
  }"`;
};

const GNSS = () => {
  const [latitude, setLatitude] = useState<number>(42.514179);
  const [longitude, setLongitude] = useState<number>(143.439731);

  const [sattelites, setSattelites] = useState<number>(0);
  const [epocTime, setEpocTime] = useState<string>();

  useEffect(() => {
    window.electronAPI.gnssRecieved(() => {
      setLatitude(Number(window.electronAPI.store.get('latitude')));
      setLongitude(Number(window.electronAPI.store.get('longitude')));
      setSattelites(Number(window.electronAPI.store.get('satellites')));
      setEpocTime(
        new Date(
          Number(window.electronAPI.store.get('epocTime'))
        ).toLocaleTimeString()
      );

      return () => {
        window.electronAPI.remove('gnss-recieved');
      };
    });
  }, []);

  return (
    <Card bordered={false} style={{ width: '250px', height: 'auto' }}>
      <div style={{ margin: '-8px 0' }}>
        <div style={{ display: 'flex', alignItems: 'center' }}>
          <div
            style={{
              borderRight: `1px solid ${emphasisMidium}`,
              paddingRight: '8px',
            }}
          >
            <div style={{ fontSize: '1.25em' }}>GNSS</div>
            <FaSatellite
              style={{ marginLeft: '4px' }}
              size={14}
              color={emphasisMidium}
            />
            <span
              style={{
                fontSize: '1em',
                color: 'white',
                marginLeft: '4px',
              }}
            >
              {sattelites}
            </span>
          </div>

          <div
            style={{
              fontSize: '1.2em',
              marginLeft: '16px',
            }}
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
      </div>
    </Card>
  );
};

export default GNSS;
