import { Card, Timeline } from 'antd';
import { useState, useEffect } from 'react';

type TimelineItem = {
  time: string;
  event: string;
};

const TimeLine = () => {
  const [events, setEvents] = useState<TimelineItem[]>([]);

  useEffect(() => {
    window.electronAPI.eventRecieved((_, event: string, flightTime: string) => {
      if (!event || !flightTime) return;

      if (event === 'LAUNCH') {
        setEvents([]);
        events.length = 0;
      }

      setEvents([...events, { time: `T + ${flightTime}`, event }]);
    });

    return () => {
      window.electronAPI.remove('event-recieved');
    };
  }, [events]);

  return (
    <Card title="TIMELINE" style={{ margin: '16px' }} bordered={false}>
      <div style={{ marginTop: '24px', height: '200px' }}>
        <Timeline mode="left" style={{ color: 'white' }}>
          {events.map((item) => {
            return (
              <Timeline.Item
                color="#5865f2"
                // dot={<FaDotCircle color="#5865f2" lightingColor="white" />}
                key={events.indexOf(item)}
                label={item.time}
              >
                <span style={{ fontSize: '1.2em' }}>{item.event}</span>
              </Timeline.Item>
            );
          })}
        </Timeline>
      </div>
    </Card>
  );
};

export default TimeLine;
