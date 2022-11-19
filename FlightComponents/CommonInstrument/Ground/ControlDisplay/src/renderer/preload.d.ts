declare global {
  interface Window {
    electronAPI: {
      getSerialports(): string[];
      oepnSerialport(path: string): void;
      oepnSerialportTelemeter(path: string): void;

      statusUpdated(callback: () => void): void;
      flightDataUpdated(callback: () => void): void;
      configUpdated(callback: () => void): void;
      rssiUpdated(callback: () => void): void;

      telemetryUpdated(callback: () => void): void;

      remove(channel: string): void;

      store: {
        get: (key: string) => any;
        set: (key: string, val: any) => void;
      };
    };
  }
}

export {};
