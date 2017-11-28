const { app, BrowserWindow } = require('electron');
const path = require('path');
const url = require('url');

/* require('electron-reload')(__dirname, {
  electron: require(`${__dirname}/node_modules/electron`)
}); */

let win;

app.commandLine.appendSwitch('allow-file-access');

function createWindow() {
	// alwaysOnTop: true,
	win = new BrowserWindow({
        width: 400,
        height: 600,
		webPreferences: {}
	});

	win.loadURL(url.format({
        pathname: path.join(__dirname, './index.html'),
        protocol: 'file:',
        slashes: true
    }));

	win.setMenu(null);

	win.webContents.openDevTools();

	win.on('closed', () => {
		win = null;
	});
}

app.on('ready', createWindow);

app.on('window-all-closed', () => {
	if (process.platform !== 'darwin') {
		app.quit();
	}
});

app.on('activate', () => {
	if (win === null) {
		createWindow();
	}
});

app.on('certificate-error', (event, webContents, url, error, certificate, callback) => {
	if (
		/https:\/\/192\.168\.30\.61:3000.+/g.test(url) ||
		/https:\/\/192\.168\.30\.61:8443.+/g.test(url) ||
		/wss:\/\/192\.168\.30\.61:3000.+/g.test(url) ||
		/ws:\/\/192\.168\.30\.61:8001.+/g.test(url) ||
		/ws:\/\/192\.168\.30\.61:8001.+/g.test(url)
	) {
		// Verification logic.
		event.preventDefault();
		callback(true);
	} else {
		callback(false);
	}
});
