declare global {
  interface Window {
    electronAPI: {
      getSerialports(): string[];
      oepnSerialport(path: string): void;
      statusUpdated(callback: () => void): void;
      flightDataUpdated(callback: () => void): void;

      store: {
        get: (key: string) => any;
        set: (key: string, val: any) => void;
      };
    };
  }
}

export {};
