/* eslint global-require: off, no-console: off, promise/always-return: off */

import path from 'path';
import { app, BrowserWindow, shell, ipcMain } from 'electron';
import { autoUpdater } from 'electron-updater';
import log from 'electron-log';
import { SerialPort } from 'serialport';
import { ReadlineParser } from '@serialport/parser-readline';
import Store from 'electron-store';
import MenuBuilder from './menu';
import { resolveHtmlPath } from './util';

class AppUpdater {
  constructor() {
    log.transports.file.level = 'info';
    autoUpdater.logger = log;
    autoUpdater.checkForUpdatesAndNotify();
  }
}

let mainWindow: BrowserWindow | null = null;

ipcMain.handle('get-serialports', async () => {
  return (await SerialPort.list()).map((serialportInfo) => serialportInfo.path);
});

let serialport: SerialPort | null = null;

const store = new Store();
ipcMain.on('get-store', async (event, val) => {
  event.returnValue = store.get(val);
});
ipcMain.on('set-store', async (event, key, val) => {
  store.set(key, val);
});

ipcMain.on('open-serialport', (_, serialportPath: string) => {
  if (serialport?.isOpen) serialport.close();

  serialport = new SerialPort({ path: serialportPath, baudRate: 115200 });
  const parser = serialport.pipe(new ReadlineParser());
  parser.on('data', (data) => {
    const dataObject = JSON.parse(data);

    if (dataObject.t === 's') {
      store.set('flightpin-state', dataObject.f === '1' ? 'OPEN' : 'CLOSE');
      store.set('shiranui3-state', dataObject.s3 === '1' ? 'ON' : 'OFF');
      store.set('buzzer-state', dataObject.b === '1' ? 'ON' : 'OFF');
      mainWindow?.webContents.send('status-updated');
    } else if (dataObject.t === 'f') {
      store.set('flight-time', dataObject.ft);
      store.set('altitude', dataObject.alt);
      store.set('acceleration-x', dataObject.ax);
      store.set('acceleration-y', dataObject.ay);
      store.set('acceleration-z', dataObject.az);
      mainWindow?.webContents.send('flight-data-updated');
    } else if (dataObject.t === 'c') {
      store.set('base-pressure', dataObject.p);
      store.set('separation-minimum', dataObject.smin);
      store.set('separation-maximum', dataObject.smax);
      mainWindow?.webContents.send('config-updated');
    } else if (dataObject.t === 'r') {
      store.set('rssi', dataObject.rssi);
      mainWindow?.webContents.send('rssi-updated');
    }
  });
});

if (process.env.NODE_ENV === 'production') {
  const sourceMapSupport = require('source-map-support');
  sourceMapSupport.install();
}

const isDebug =
  process.env.NODE_ENV === 'development' || process.env.DEBUG_PROD === 'true';

if (isDebug) {
  require('electron-debug')();
}

const installExtensions = async () => {
  const installer = require('electron-devtools-installer');
  const forceDownload = !!process.env.UPGRADE_EXTENSIONS;
  const extensions = ['REACT_DEVELOPER_TOOLS'];

  return installer
    .default(
      extensions.map((name) => installer[name]),
      forceDownload
    )
    .catch(console.log);
};

const createWindow = async () => {
  if (isDebug) {
    await installExtensions();
  }

  const RESOURCES_PATH = app.isPackaged
    ? path.join(process.resourcesPath, 'assets')
    : path.join(__dirname, '../../assets');

  const getAssetPath = (...paths: string[]): string => {
    return path.join(RESOURCES_PATH, ...paths);
  };

  mainWindow = new BrowserWindow({
    show: false,
    icon: getAssetPath('icon.png'),
    webPreferences: {
      preload: app.isPackaged
        ? path.join(__dirname, 'preload.js')
        : path.join(__dirname, '../../.erb/dll/preload.js'),
    },
  });

  mainWindow.loadURL(resolveHtmlPath('index.html'));

  mainWindow.on('ready-to-show', () => {
    if (!mainWindow) {
      throw new Error('"mainWindow" is not defined');
    }
    if (process.env.START_MINIMIZED) {
      mainWindow.minimize();
    } else {
      mainWindow.show();
    }
  });

  mainWindow.on('closed', () => {
    mainWindow = null;
  });

  const menuBuilder = new MenuBuilder(mainWindow);
  menuBuilder.buildMenu();

  // Open urls in the user's browser
  mainWindow.webContents.setWindowOpenHandler((edata) => {
    shell.openExternal(edata.url);
    return { action: 'deny' };
  });

  // Remove this if your app does not use auto updates
  // eslint-disable-next-line
  new AppUpdater();
};

/**
 * Add event listeners...
 */

app.on('window-all-closed', () => {
  // Respect the OSX convention of having the application in memory even
  // after all windows have been closed
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

app
  .whenReady()
  .then(() => {
    createWindow();
    app.on('activate', () => {
      // On macOS it's common to re-create a window in the app when the
      // dock icon is clicked and there are no other windows open.
      if (mainWindow === null) createWindow();
    });
  })
  .catch(console.log);
