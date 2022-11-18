import { contextBridge, ipcRenderer, IpcRendererEvent } from 'electron';

export type Channels = 'ipc-example';

contextBridge.exposeInMainWorld('electronAPI', {
  getSerialports: () => ipcRenderer.invoke('get-serialports'),
  oepnSerialport: (path: string) => ipcRenderer.send('open-serialport', path),
  statusUpdated: (callback: () => void) =>
    ipcRenderer.on('status-updated', callback),
  flightDataUpdated: (callback: () => void) =>
    ipcRenderer.on('flight-data-updated', callback),

  store: {
    get(key: string) {
      return ipcRenderer.sendSync('get-store', key);
    },
    set(property: string, val: any) {
      ipcRenderer.send('set-store', property, val);
    },
  },
});
