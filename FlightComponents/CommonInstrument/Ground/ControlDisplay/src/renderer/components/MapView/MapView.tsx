import { useState, useEffect } from 'react';
import {
  MapContainer,
  TileLayer,
  Marker,
  Circle,
  Tooltip,
  Polygon,
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
  const [center, setCenter] = useState<L.LatLngExpression>([
    42.514179, 143.439731,
  ]);

  useEffect(() => {
    window.electronAPI.gnssRecieved(() => {
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
      style={{ position: 'fixed' }}
    >
      <TileLayer
        attribution='&copy; <a href="https://maps.gsi.go.jp/development/ichiran.html">国土地理院</a>'
        url={`${window.location.origin}/map/tile/{z}/{x}/{y}.jpg`}
      />
      <Marker position={center} />
      <ChangeCenter center={center} />
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
  );
};

export default MapView;
