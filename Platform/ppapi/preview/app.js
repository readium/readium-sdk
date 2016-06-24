// Requirements
const { app, BrowserWindow } = require('electron');
const path = require('path');

// Initialize readium plugin
let readiumPluginPath = path.join(__dirname, '..', 'out', 'Default', 'lib', 'libreadium.so');
console.log(readiumPluginPath);
app.commandLine.appendSwitch("register-pepper-plugins", readiumPluginPath + ";application/x-ppapi-readium");


// Keep a global reference of the window object, if you don't, the window will
// be closed automatically when the javascript object is GCed.
var mainWindow = null;

// Quit when all windows are closed.
app.on("window-all-closed", () => {
  app.quit();
});

app.on("ready", function() {
  mainWindow = new BrowserWindow({
    width: 960,
    height: 540,
    minWidth: 640,
    minHeight: 440,
    plugin:true,
    resizable: true,
    title: "Readium PPAPI Test",
    webPreferences: {
      plugins: true
    }
  });

  // Hide menu
  mainWindow.setMenu(null);
  mainWindow.openDevTools();
  mainWindow.loadURL('file://' + __dirname + '/index.html');
  mainWindow.show();
});