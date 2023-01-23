import { useState, useEffect } from 'react';
import { FaRegDotCircle } from 'react-icons/fa';
import ballTop from '../../images/ball/ball-top.png';
import ballBottom from '../../images/ball/ball-bottom.png';

const inRange = (min: number, max: number, value: number) =>
  min <= value && value < max;

const AltitudeIndicator = () => {
  const [pitchPosition, setPitchPosition] = useState<number>(0);
  const [rollPosition, setRollPosition] = useState<number>(0);
  const [isPositive, setIsPositive] = useState<boolean>();

  const setPosition = (pitch: number, roll: number) => {
    const nextRollPosition = 50 * Math.cos(roll * (Math.PI / 180));
    const nextPitchPosition = 50 * Math.cos(pitch * (Math.PI / 180));

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
    <div
      style={{
        background: 'rgb(18, 18, 18, 0.75)',
        margin: '8px',
        padding: '25px',
        borderRadius: '50px',
      }}
    >
      <div
        style={{
          display: 'flex',
          position: 'relative',
          alignItems: 'center',
          justifyContent: 'center',
        }}
      >
        <img height={200} src={isPositive ? ballTop : ballBottom} alt="back" />
        <div
          style={{
            position: 'absolute',
            width: '200px',
            height: '200px',
          }}
        >
          <FaRegDotCircle
            size={24}
            style={{
              color: isPositive ? 'white' : 'black',
              position: 'absolute',
              marginLeft: `${pitchPosition}%`,
              marginTop: `${rollPosition}%`,
            }}
          />
        </div>
      </div>
    </div>
  );
};

export default AltitudeIndicator;
