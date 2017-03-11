module.exports = {
  searchDevices: function (successCallback, errorCallback) {
    cordova.exec(successCallback, errorCallback, 'Mpos', 'searchAccessories', [])
  },
  listenDevices: function (notificationCallback, errorCallback) {
    cordova.exec(notificationCallback, errorCallback, 'Mpos', 'listenDevices', [])
  },
  stopListeningDevices: function () {
    cordova.exec(function () {}, function () {}, 'Mpos', 'stopListeningDevices', [])
  },

  initialize: function (id, encryptionKey, successCallback, errorCallback) {
    cordova.exec(successCallback, errorCallback, 'Mpos', 'initialize', [id, encryptionKey])
  },
  openConnection: function (successCallback, errorCallback) {
    cordova.exec(successCallback, errorCallback, 'Mpos', 'openConnection', [])
  },
  closeConnection: function (successCallback, errorCallback) {
    cordova.exec(successCallback, errorCallback, 'Mpos', 'closeConnection', [])
  },

  openSession: function (successCallback, errorCallback) {
    cordova.exec(successCallback, errorCallback, 'Mpos', 'openSession', [])
  },
  closeSession: function (message, successCallback, errorCallback) {
    cordova.exec(successCallback, errorCallback, 'Mpos', 'closeSession', [message])
  },

  downloadTables: function (force, successCallback, errorCallback) {
    cordova.exec(successCallback, errorCallback, 'Mpos', 'downloadTables', [force])
  },

  display: function (message, successCallback, errorCallback) {
    cordova.exec(successCallback, errorCallback, 'Mpos', 'display', [message])
  },

  pay: function (amount, applications, magstripeMethod, successCallback, errorCallback) {
    cordova.exec(successCallback, errorCallback, 'Mpos', 'pay', [amount, applications, magstripeMethod])
  },

  finish: function (success, arc, data, successCallback, errorCallback) {
    cordova.exec(successCallback, errorCallback, 'Mpos', 'finish', [success, arc, data])
  },

  cancel: function () {
    cordova.exec(function () {}, function () {}, 'Mpos', 'cancel', [])
  }
}
