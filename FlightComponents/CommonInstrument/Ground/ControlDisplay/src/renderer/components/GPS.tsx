import { Card, Row, Col } from 'antd';
import { useEffect, useState } from 'react';
import { FaSatellite } from 'react-icons/fa';
import {
  MapContainer,
  TileLayer,
  Marker,
  Circle,
  Tooltip,
} from 'react-leaflet';
import 'leaflet/dist/leaflet.css';
import L from 'leaflet';

L.Icon.Default.imagePath =
  'https://cdnjs.cloudflare.com/ajax/libs/leaflet/1.7.1/images/';

const DegToDms = (degrees: number): string => {
  const minits = (degrees - Math.floor(degrees)) * 60;
  const seconds = (minits - Math.floor(minits)) * 60;
  return `${Math.floor(degrees)}° ${Math.floor(seconds)}' ${
    Math.floor(seconds * 100) / 100
  }" `;
};

const GPS = () => {
  const [latitude, setLatitude] = useState<number>(42.514179);
  const [longitude, setLongitude] = useState<number>(143.439731);

  const [sattelites, setSattelites] = useState<number>(4);

  useEffect(() => {
    window.electronAPI.telemetryRecieved(() => {
      setLatitude(Number(window.electronAPI.store.get('latitude')));
      setLongitude(Number(window.electronAPI.store.get('longitude')));
      setSattelites(Number(window.electronAPI.store.get('sattelites')));

      return () => {
        window.electronAPI.remove('telemetry-recieved');
      };
    });
  }, []);

  return (
    <Card
      title="GPS"
      bordered={false}
      style={{ margin: '16px' }}
      extra={
        <div style={{ display: 'flex', alignItems: 'center' }}>
          <FaSatellite
            style={{ marginLeft: '16px' }}
            size={16}
            color="#b9bbbe"
          />
          <span
            style={{
              fontSize: '1.2em',
              color: sattelites === 0 ? '#ed4245' : 'white',
              marginLeft: '8px',
            }}
          >
            {sattelites}
          </span>
        </div>
      }
    >
      <Row style={{ margin: '24px 0px' }}>
        <Col span={12} style={{ display: 'flex', justifyContent: 'center' }}>
          <span style={{ fontSize: '1.2em' }}>{`${DegToDms(
            Math.abs(latitude)
          )}${Math.sign(latitude) === -1 ? 'S' : 'N'}`}</span>
        </Col>
        <Col span={12} style={{ display: 'flex', justifyContent: 'center' }}>
          <span style={{ fontSize: '1.2em' }}>{`${DegToDms(
            Math.abs(longitude)
          )}${Math.sign(longitude) === -1 ? 'W' : 'E'}`}</span>
        </Col>
      </Row>
      <MapContainer
        zoomControl={false}
        center={[latitude, longitude]}
        zoom={14}
      >
        <TileLayer
          attribution='&copy; <a href="https://maps.gsi.go.jp/development/ichiran.html">国土地理院</a>'
          url="https://cyberjapandata.gsi.go.jp/xyz/seamlessphoto/{z}/{x}/{y}.jpg"
        />
        <Marker position={[latitude, longitude]} />
        <Circle
          pathOptions={{ color: '#ed4245' }}
          center={[42.514179, 143.439731]}
          radius={1000}
        >
          <Tooltip>Safety Zone</Tooltip>
        </Circle>
        <Circle
          pathOptions={{ color: '#5865f2' }}
          center={[42.514179, 143.439731]}
          radius={50}
        >
          <Tooltip>Launcher</Tooltip>
        </Circle>
        <Circle
          pathOptions={{ color: '#ed4245' }}
          center={[42.51887536, 143.44089407]}
          radius={50}
        >
          <Tooltip>Hard</Tooltip>
        </Circle>
        <Circle
          pathOptions={{ color: '#46c46d' }}
          center={[42.51719357, 143.44248005]}
          radius={50}
        >
          <Tooltip>Soft</Tooltip>
        </Circle>
      </MapContainer>
    </Card>
  );
};

export default GPS;
