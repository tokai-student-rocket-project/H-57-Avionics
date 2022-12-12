import { Card } from 'antd';
import { useState, useEffect } from 'react';
import back from '../../images/back.png';

const inRange = (min: number, max: number, value: number) =>
  min <= value && value < max;

const AltitudeIndicator = () => {
  const [pitchPosition, setPitchPosition] = useState<number>(0);
  const [rollPosition, setRollPosition] = useState<number>(0);
  const [isPositive, setIsPositive] = useState<boolean>();

  const setPosition = (pitch: number, roll: number) => {
    const nextPitchPosition = ((pitch + 90) / 180) * 100;
    const nextRollPosition = ((roll + 90) / 180) * 100;

    if (nextPitchPosition < 0) setPitchPosition(nextPitchPosition * -1);
    else if (nextPitchPosition > 100) setPitchPosition(200 - nextPitchPosition);
    else setPitchPosition(nextPitchPosition);

    if (nextRollPosition < 0) setRollPosition(nextRollPosition * -1);
    else if (nextRollPosition > 100) setRollPosition(200 - nextRollPosition);
    else setRollPosition(nextRollPosition);

    setIsPositive(inRange(-90, 90, pitch) && inRange(-90, 90, roll));
  };

  useEffect(() => {
    window.electronAPI.flightDataRecieved(() => {
      setPosition(
        Number(window.electronAPI.store.get('pitch')),
        Number(window.electronAPI.store.get('roll'))
      );
    });

    return () => {
      window.electronAPI.remove('flight-data-recieved');
    };
  }, []);

  return (
    <Card bordered={false} style={{ margin: '8px' }}>
      <div
        style={{
          display: 'flex',
          marginTop: '24px',
          position: 'relative',
          alignItems: 'center',
          justifyContent: 'center',
        }}
      >
        <img height={250} src={back} alt="back" />
        <div
          style={{
            position: 'absolute',
            width: '200px',
            height: '200px',
          }}
        >
          <div
            style={{
              backgroundColor: isPositive ? 'white' : 'black',
              width: '8px',
              position: 'absolute',
              height: '8px',
              borderRadius: '50%',
              marginLeft: `${pitchPosition}%`,
              marginTop: `${rollPosition}%`,
            }}
          />
        </div>
      </div>
    </Card>
  );
};

export default AltitudeIndicator;
