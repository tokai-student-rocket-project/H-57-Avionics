import { Steps, Card, notification } from 'antd';
import { useState, useEffect } from 'react';

type EventItem = {
  id: number;
  flightTime: number;
  event: string;
};

const Progress = () => {
  const [notificationApi, contextHolder] = notification.useNotification();
  const [latestEvent, setLatestEvent] = useState<EventItem>();
  const [stepProgress, setStepProgress] = useState<number>(-1);

  const [launchTime, setLaunchTime] = useState<number>();
  const [burnoutTime, setBurnoutTime] = useState<number>();
  const [apogeeTime, setApogeeTime] = useState<number>();
  const [separationTime, setSeparationTime] = useState<number>();
  const [landTime, setLandTime] = useState<number>();

  useEffect(() => {
    window.electronAPI.eventRecieved((_, id, flightTime, event) => {
      if (id === latestEvent?.id) return;

      const eventText = [
        'INITIALIZE',
        'START',
        'LAUNCH',
        'BURNOUT',
        'APOGEE',
        'SEPARATE',
        'FORCE-SEPARATE',
        'LAND',
        'RESET',
        'CONFIG-UPDATE',
        'EMERGENCY-SEPARATE',
      ][Number(event)];

      setLatestEvent({ id, flightTime, event: eventText });

      if (eventText === 'RESET') {
        setStepProgress(0);
        setLaunchTime(undefined);
        setBurnoutTime(undefined);
        setApogeeTime(undefined);
        setSeparationTime(undefined);
        setLandTime(undefined);
      }
      if (eventText === 'LAUNCH') {
        setStepProgress(1);
        setLaunchTime(flightTime);
      }
      if (eventText === 'BURNOUT') {
        setStepProgress(2);
        setBurnoutTime(flightTime);
      }
      if (eventText === 'APOGEE') {
        setStepProgress(3);
        setApogeeTime(flightTime);
      }
      if (
        eventText === 'SEPARATE' ||
        eventText === 'FORCE-SEPARATE' ||
        eventText === 'EMERGENCY-SEPARATE'
      ) {
        setStepProgress(4);
        setSeparationTime(flightTime);
      }
      if (eventText === 'LAND') {
        setStepProgress(5);
        setLandTime(flightTime);
      }

      notificationApi.open({
        message: `${
          flightTime < 0 ? '' : `[T+${flightTime.toFixed(2)}]`
        } ${eventText}`,
        placement: 'bottomLeft',
      });
    });

    return () => {
      window.electronAPI.remove('event-recieved');
    };
  }, [latestEvent?.id, notificationApi]);

  return (
    <Card bordered={false}>
      {contextHolder}
      <Steps
        progressDot
        current={stepProgress}
        items={[
          {
            title: 'LNCH',
            description: launchTime ? `${launchTime?.toFixed(1)} sec` : '',
          },
          {
            title: 'BOUT',
            description: burnoutTime ? `${burnoutTime?.toFixed(1)} sec` : '',
          },
          {
            title: 'APOG',
            description: apogeeTime ? `${apogeeTime?.toFixed(1)} sec` : '',
          },
          {
            title: 'SEPA',
            description: separationTime
              ? `${separationTime?.toFixed(1)} sec`
              : '',
          },
          {
            title: 'LAND',
            description: landTime ? `${landTime?.toFixed(1)} sec` : '',
          },
        ]}
      />
    </Card>
  );
};

export default Progress;
