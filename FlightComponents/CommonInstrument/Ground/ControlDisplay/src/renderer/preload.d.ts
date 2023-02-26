declare global {
  interface Window {
    electronAPI: {
      getSerialports(): string[];
      oepnSerialport(path: string): void;
      oepnSerialportTelemeter(path: string): void;

      statusRecieved(callback: () => void): void;
      flightDataRecieved(callback: () => void): void;
      configRecieved(callback: () => void): void;
      eventRecieved(
        callback: (
          _: never,
          id: number,
          flightTime: number,
          event: string
        ) => void
      ): void;
      rssiRecieved(callback: () => void): void;

      sendConfig(label: string, value: string): void;
      configSended(callback: () => void): void;

      gnssRecieved(callback: () => void): void;
      valveRecieved(callback: () => void): void;

      remove(channel: string): void;

      store: {
        get: (key: string) => any;
        set: (key: string, val: any) => void;
      };

      logging: {
        logUpdated(callback: () => void): void;
        start: (outputFile: string) => void;
        stop: () => void;
      };
    };
  }
}

export {};
