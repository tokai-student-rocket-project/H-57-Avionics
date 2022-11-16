declare global {
  interface Window {
    electronAPI: {
      getSerialports(): string[];
      oepnSerialport(path: string): void;
      recievedData(callback: (event: never, data: string) => void): void;
    };
  }
}

export {};
