import { useState, useEffect } from 'react';
import {
  MapContainer,
  TileLayer,
  Marker,
  Circle,
  Tooltip,
  useMap,
} from 'react-leaflet';
import 'leaflet/dist/leaflet.css';
import L from 'leaflet';

L.Icon.Default.imagePath = `${window.location.origin}/map/icon/`;

type ChangeCenterProps = {
  center: L.LatLngExpression;
};

const ChangeCenter = (props: ChangeCenterProps) => {
  const { center } = props;
  const map = useMap();

  try {
    map.setView(center);
    // eslint-disable-next-line no-empty
  } catch {}

  return null;
};

const MapView = () => {
  const [center, setCenter] = useState<L.LatLngExpression>([0, 0]);

  useEffect(() => {
    window.electronAPI.gnssRecieved(() => {
      if (Number(window.electronAPI.store.get('latitude')) === NaN) return;
      if (Number(window.electronAPI.store.get('longitude')) === NaN) return;

      setCenter([
        Number(window.electronAPI.store.get('latitude')),
        Number(window.electronAPI.store.get('longitude')),
      ]);

      return () => {
        window.electronAPI.remove('gnss-recieved');
      };
    });
  }, []);

  return (
    <MapContainer
      zoomControl={false}
      center={center}
      zoom={16}
      style={{ position: 'fixed', height: 'calc(100vh - 32px)' }}
    >
      <TileLayer
        attribution='&copy; <a href="https://maps.gsi.go.jp/development/ichiran.html">国土地理院</a>'
        url={`${window.location.origin}/map/tile/{z}/{x}/{y}.png`}
      />
      <Marker position={center} />
      <ChangeCenter center={center} />
      <Circle
        pathOptions={{ color: '#ed4245' }}
        center={[42.51435555, 143.43985]}
        radius={1000}
      >
        <Tooltip>Safety Zone</Tooltip>
      </Circle>
      <Circle
        pathOptions={{ color: '#5865f2' }}
        center={[42.51435555, 143.43985]}
        radius={100}
      >
        <Tooltip>Launcher</Tooltip>
      </Circle>
      <Circle
        pathOptions={{ color: '#FF1744' }}
        center={[42.51940917, 143.4417391]}
        radius={10}
      >
        <Tooltip>Hard</Tooltip>
      </Circle>
      <Circle
        pathOptions={{ color: '#00E676' }}
        center={[42.51633086, 143.44108353]}
        radius={10}
      >
        <Tooltip>Soft</Tooltip>
      </Circle>
    </MapContainer>
  );
};

export default MapView;
