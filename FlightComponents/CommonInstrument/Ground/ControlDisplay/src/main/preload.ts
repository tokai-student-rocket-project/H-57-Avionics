import { contextBridge, ipcRenderer } from 'electron';

export type Channels = 'ipc-example';

contextBridge.exposeInMainWorld('electronAPI', {
  getSerialports: () => ipcRenderer.invoke('get-serialports'),
  oepnSerialport: (path: string) => ipcRenderer.send('open-serialport', path),
  statusUpdated: (callback: () => void) =>
    ipcRenderer.on('status-updated', callback),
  flightDataUpdated: (callback: () => void) =>
    ipcRenderer.on('flight-data-updated', callback),
  configUpdated: (callback: () => void) =>
    ipcRenderer.on('config-updated', callback),
  rssiUpdated: (callback: () => void) =>
    ipcRenderer.on('rssi-updated', callback),

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
