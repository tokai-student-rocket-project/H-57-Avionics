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
    callback: (_: IpcRendererEvent, event: string, flightTime: string) => void
  ) => ipcRenderer.on('event-recieved', callback),
  rssiRecieved: (callback: () => void) =>
    ipcRenderer.on('rssi-recieved', callback),

  telemetryRecieved: (callback: () => void) =>
    ipcRenderer.on('telemetry-recieved', callback),

  remove: (channel: string) => ipcRenderer.removeAllListeners(channel),

  store: {
    get(key: string) {
      return ipcRenderer.sendSync('get-store', key);
    },
    set(property: string, val: any) {
      ipcRenderer.send('set-store', property, val);
    },
  },
});
