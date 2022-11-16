import { contextBridge, ipcRenderer, IpcRendererEvent } from 'electron';

export type Channels = 'ipc-example';

contextBridge.exposeInMainWorld('electronAPI', {
  getSerialports: () => ipcRenderer.invoke('get-serialports'),
  oepnSerialport: (path: string) => ipcRenderer.send('open-serialport', path),
  recievedData: (callback: (event: IpcRendererEvent, data: string) => void) =>
    ipcRenderer.on('recieved-data', callback),
});
