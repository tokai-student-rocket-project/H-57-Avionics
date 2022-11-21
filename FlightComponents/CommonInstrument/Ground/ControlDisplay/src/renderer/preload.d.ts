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
        callback: (_: never, event: string, flightTime: string) => void
      ): void;
      rssiRecieved(callback: () => void): void;

      sendConfig(label: string, value: string): void;
      configSended(callback: () => void): void;

      telemetryRecieved(callback: () => void): void;

      remove(channel: string): void;

      store: {
        get: (key: string) => any;
        set: (key: string, val: any) => void;
      };
    };
  }
}

export {};
