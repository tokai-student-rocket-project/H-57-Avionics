/* eslint global-require: off, no-console: off, promise/always-return: off */

import path from 'path';
import * as fs from 'fs';
import { app, BrowserWindow, shell, ipcMain } from 'electron';
import { autoUpdater } from 'electron-updater';
import log from 'electron-log';
import { SerialPort } from 'serialport';
import { ReadlineParser } from '@serialport/parser-readline';
import Store from 'electron-store';
import { resolveHtmlPath } from './util';

class AppUpdater {
  constructor() {
    log.transports.file.level = 'info';
    autoUpdater.logger = log;
    autoUpdater.checkForUpdatesAndNotify();
  }
}

let doLogging = false;
let outputStream: fs.WriteStream;

ipcMain.on('start-logging', async (_, outputFile) => {
  doLogging = true;
  outputStream = outputFile;

  outputStream.end();
  outputStream = fs.createWriteStream(outputFile, { flags: 'a' });
});

ipcMain.on('stop-logging', async () => {
  doLogging = false;
  outputStream.end();
});

let mainWindow: BrowserWindow | null = null;

ipcMain.handle('get-serialports', async () => {
  return (await SerialPort.list()).map((serialportInfo) => serialportInfo.path);
});

const store = new Store();
ipcMain.on('get-store', async (event, val) => {
  event.returnValue = store.get(val);
});
ipcMain.on('set-store', async (_, key, val) => {
  store.set(key, val);
});

let serialport: SerialPort | null = null;
let serialportTelemeter: SerialPort | null = null;

ipcMain.on('open-serialport', (_, serialportPath: string) => {
  if (serialport?.isOpen) serialport.close();

  serialport = new SerialPort({ path: serialportPath, baudRate: 115200 });
  const parser = serialport.pipe(new ReadlineParser());
  parser.on('data', (data) => {
    if (doLogging) {
      console.log(`${data} > ${outputStream.path}`);
      outputStream?.write(data);

      store.set('latest-log', data);
      mainWindow?.webContents.send('log-updated');
    }

    try {
      const dataObject = JSON.parse(data);

      if (dataObject.m) {
        store.set('flight-mode', dataObject.m);
        store.set('flightpin-state', dataObject.f === '1' ? 'OPEN' : 'CLOSE');
        store.set('shiranui3-state', dataObject.s3 === '1' ? 'ON' : 'OFF');
        store.set('buzzer-state', dataObject.b === '1' ? 'ON' : 'OFF');
        store.set('3.3v', dataObject.v33);
        store.set('5v', dataObject.v5);
        store.set('12v', dataObject.v12);
        mainWindow?.webContents.send('status-recieved');
      }

      if (dataObject.ft) {
        store.set('flight-time', dataObject.ft);
        store.set('altitude', dataObject.alt);
        store.set('yaw', dataObject.y);
        store.set('pitch', dataObject.p);
        store.set('roll', dataObject.r);
        store.set('acceleration', dataObject.ac);
        mainWindow?.webContents.send('flight-data-recieved');
      }

      if (dataObject.a) {
        store.set('separation-altitude', dataObject.a);
        store.set('base-pressure', dataObject.p);
        store.set('burn-time', dataObject.bt);
        store.set('separation-protection', dataObject.sp);
        store.set('force-separation', dataObject.fs);
        store.set('landing-time', dataObject.l);
        mainWindow?.webContents.send('config-recieved');
      }

      if (dataObject.e) {
        mainWindow?.webContents.send(
          'event-recieved',
          Number(dataObject.id),
          Number(dataObject.eft),
          dataObject.e
        );
      }

      if (dataObject.rssi) {
        store.set('rssi', dataObject.rssi);
        mainWindow?.webContents.send('rssi-recieved');
      }
    } catch (error) {
      console.log(error);
    }
  });
});

ipcMain.on('send-config', (_, label: string, value: string) => {
  if (!serialport) return;

  const dataObject = JSON.stringify({ t: 'c', l: label, v: value });
  serialport.write(`${dataObject}\n`);
  mainWindow?.webContents.send('config-sended');
});

ipcMain.on('open-serialport-telemeter', (_, serialportPath: string) => {
  if (serialportTelemeter?.isOpen) serialportTelemeter.close();

  serialportTelemeter = new SerialPort({
    path: serialportPath,
    baudRate: 115200,
  });
  const parser = serialportTelemeter.pipe(new ReadlineParser());
  parser.on('data', (data) => {
    if (doLogging) {
      console.log(`${data} > ${outputStream.path}`);
      outputStream?.write(data);

      store.set('latest-log', data);
      mainWindow?.webContents.send('log-updated');
    }

    try {
      const dataObject = JSON.parse(data);

      if (dataObject.lat) {
        store.set('latitude', dataObject.lat);
        store.set('longitude', dataObject.lon);
        store.set('satellites', dataObject.satellites);
        store.set('epochTime', dataObject.epochTime);
        mainWindow?.webContents.send('gnss-recieved');
      }

      if (dataObject.mainservoDeg) {
        store.set('mainservo-degrees', dataObject.mainservoDeg);
        store.set('supplyservo-degrees', dataObject.supplyservoDeg);
        mainWindow?.webContents.send('valve-recieved');
      }
    } catch (error) {
      console.log(error);
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
