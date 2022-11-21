import { Card, Row, Col } from 'antd';
import { useEffect, useState } from 'react';
import { FaSatellite } from 'react-icons/fa';
import {
  MapContainer,
  TileLayer,
  Marker,
  Circle,
  Tooltip,
  Polygon,
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
        <Polygon
          pathOptions={{ color: '#46c46d' }}
          positions={[
            [42.517662551560605, 143.4408444423992],
            [42.51761489475421, 143.44100119790636],
            [42.51753538002347, 143.44112702526658],
            [42.51743114627683, 143.44120365870916],
            [42.51731410672498, 143.44121553146056],
            [42.517201656980916, 143.44115351416679],
            [42.51711196363886, 143.44101809033876],
            [42.51703999418659, 143.44082518765995],
            [42.517023878154305, 143.44063172300298],
            [42.51710607761477, 143.44047022030415],
            [42.51723478804244, 143.4403432753145],
            [42.51736014435264, 143.44028010129404],
            [42.517480795045266, 143.44029621579418],
            [42.51758020617211, 143.4403804866781],
            [42.5176462610769, 143.44051399690855],
            [42.51767377537928, 143.44067553769366],
          ]}
        >
          <Tooltip>Soft</Tooltip>
        </Polygon>
        <Polygon
          pathOptions={{ color: '#ed4245' }}
          positions={[
            [42.51465180947542, 143.43989483142963],
            [42.51494516752352, 143.4390180568032],
            [42.51546023278824, 143.43834318385652],
            [42.51613245500861, 143.43796935389503],
            [42.51687228848424, 143.4379682024319],
            [42.51756864383147, 143.43835695709456],
            [42.51810694506331, 143.43909523948682],
            [42.51838442625916, 143.44007251180798],
            [42.51835520320343, 143.44112967546081],
            [42.5180217269953, 143.44208753484284],
            [42.51743931550854, 143.4427775245589],
            [42.516708785481086, 143.443090072334],
            [42.51595932808879, 143.4429963594835],
            [42.51530543080068, 143.44252927319647],
            [42.5148379930463, 143.44177675871688],
            [42.514614911336196, 143.4408554454254],
          ]}
        >
          <Tooltip>Hard</Tooltip>
        </Polygon>
      </MapContainer>
    </Card>
  );
};

export default GPS;
