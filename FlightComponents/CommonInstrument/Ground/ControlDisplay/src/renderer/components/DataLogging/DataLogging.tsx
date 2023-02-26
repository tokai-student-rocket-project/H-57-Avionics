import { Button, Card, Input } from 'antd';
import React, { useState } from 'react';
import { FileFilled } from '@ant-design/icons';
import {
  green,
  red,
  emphasisMidium,
  emphasisHigh,
} from '../../utilities/colors';

const DataLogging = () => {
  const [isLogging, setIsLogging] = useState<boolean>();
  const [outputFile, setOutputFile] = useState<string>();

  const toggleLogging = () => {
    if (outputFile === undefined) return;

    setIsLogging((prevState) => !prevState);

    if (isLogging) window.electronAPI.logging.stop();
    else window.electronAPI.logging.start(`${outputFile}.csv`);
  };

  return (
    <Card bordered={false} style={{ width: '250px', height: 'auto' }}>
      <div style={{ margin: '-8px 0' }}>
        <div
          style={{
            display: 'flex',
            alignItems: 'center',
          }}
        >
          <div
            style={{
              borderRight: `1px solid ${emphasisMidium}`,
              paddingRight: '8px',
            }}
          >
            <div style={{ fontSize: '1.25em' }}>LOG</div>
          </div>
          <div style={{ marginLeft: '16px' }}>
            <Input
              placeholder="Output File"
              addonAfter=".csv"
              value={outputFile}
              onChange={(event: React.ChangeEvent<HTMLInputElement>) => {
                setOutputFile(event.target.value);
              }}
            />
            <div
              style={{
                display: 'flex',
                marginTop: '8px',
                alignItems: 'center',
              }}
            >
              <Button
                onClick={toggleLogging}
                ghost
                shape="round"
                style={{
                  background: isLogging ? emphasisMidium : 'transparent',
                  borderColor: isLogging ? emphasisHigh : 'transparent',
                }}
                icon={
                  <FileFilled
                    color={isLogging ? emphasisHigh : emphasisMidium}
                  />
                }
              />
              <div
                style={{
                  backgroundColor: isLogging ? green : red,
                  width: '18px',
                  margin: 'auto 16px',
                  height: '18px',
                  borderRadius: '50%',
                }}
              />
              <span style={{ color: isLogging ? green : red }}>
                {isLogging ? '保存中' : '停止中'}
              </span>
            </div>
          </div>
        </div>
      </div>
    </Card>
  );
};

export default DataLogging;
