import { contextBridge, ipcRenderer, IpcRendererEvent } from 'electron';

export type Channels = 'ipc-example';

contextBridge.exposeInMainWorld('electronAPI', {
  getSerialports: () => ipcRenderer.invoke('get-serialports'),
  oepnSerialport: (path: string) => ipcRenderer.send('open-serialport', path),
  oepnSerialportTelemeter: (path: string) =>
    ipcRenderer.send('open-serialport-telemeter', path),

  statusRecieved: (callback: () => void) =>
    ipcRenderer.on('status-recieved', callback),
  flightDataRecieved: (callback: () => void) =>
    ipcRenderer.on('flight-data-recieved', callback),
  configRecieved: (callback: () => void) =>
    ipcRenderer.on('config-recieved', callback),
  eventRecieved: (
    callback: (
      _: IpcRendererEvent,
      id: number,
      flightTime: number,
      event: string
    ) => void
  ) => ipcRenderer.on('event-recieved', callback),
  rssiRecieved: (callback: () => void) =>
    ipcRenderer.on('rssi-recieved', callback),

  sendConfig: (label: string, value: string) =>
    ipcRenderer.send('send-config', label, value),
  configSended: (callback: () => void) =>
    ipcRenderer.on('config-sended', callback),

  gnssRecieved: (callback: () => void) =>
    ipcRenderer.on('gnss-recieved', callback),

  valveRecieved: (callback: () => void) =>
    ipcRenderer.on('valve-recieved', callback),

  remove: (channel: string) => ipcRenderer.removeAllListeners(channel),

  store: {
    get(key: string) {
      return ipcRenderer.sendSync('get-store', key);
    },
    set(property: string, val: any) {
      ipcRenderer.send('set-store', property, val);
    },
  },

  logging: {
    logUpdated: (callback: () => void) =>
      ipcRenderer.on('log-updated', callback),
    start(outputFile: string) {
      ipcRenderer.send('start-logging', outputFile);
    },
    stop() {
      ipcRenderer.send('stop-logging');
    },
  },
});
