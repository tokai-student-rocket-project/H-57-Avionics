import { useState, useEffect } from 'react';
import { emphasisHigh } from '../../utilities/colors';

const Log = () => {
  const [latestLog, setLatestLog] = useState<string>();

  useEffect(() => {
    window.electronAPI.logging.logUpdated(() => {
      setLatestLog(window.electronAPI.store.get('latest-log'));
    });

    return () => {
      window.electronAPI.remove('log-updated');
    };
  }, []);

  return (
    <div
      style={{
        backgroundColor: 'rgb(17, 17, 17)',
        padding: '4px 64px',
        height: '32px',
      }}
    >
      <span
        style={{
          color: emphasisHigh,
          fontSize: '1em',
        }}
      >
        {latestLog}
      </span>
    </div>
  );
};

export default Log;
