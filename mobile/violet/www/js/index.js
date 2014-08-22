/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
var arrayBufferToInt = function (ab) {
    var a = new Int32Array(ab);
    return a[0];
};

var app = {
    // Application Constructor
    initialize: function() {
        this.bindEvents();
        detailPage.hidden = true;
    },
    // Bind Event Listeners
    //
    // Bind any events that are required on startup. Common events are:
    // 'load', 'deviceready', 'offline', and 'online'.
    bindEvents: function() {
        document.addEventListener('deviceready', this.onDeviceReady, false);
        document.addEventListener('pause', this.onPause, false);
        refreshButton.addEventListener('touchstart', this.refreshDeviceList, false);
        timeButton.addEventListener('touchstart', this.sendTime);
        deviceList.addEventListener('touchstart', this.connect, false); // assume not scrolling
        // add logic for resume, back, menu, etc.
    },
    // deviceready Event Handler
    //
    // The scope of 'this' is the event. In order to call the 'receivedEvent'
    // function, we must explicitly call 'app.receivedEvent(...);'
    onDeviceReady: function() {
        app.refreshDeviceList();
    },
    onPause: function() {
        rfduino.disconnect();
    },
    // Update DOM on a Received Event
    refreshDeviceList: function() {
        deviceList.innerHTML = ''; // empties the list
        rfduino.discover(9, app.onDiscoverDevice, app.onError);
    },
    onDiscoverDevice: function(device) {
        var listItem = document.createElement('li'),
            html = '<b>' + device.name + '</b><br/>' +
                'RSSI: ' + device.rssi + '&nbsp;|&nbsp;' +
                'Advertising: ' + device.advertising + '<br/>' +
                device.uuid;

        listItem.setAttribute('uuid', device.uuid);
        listItem.innerHTML = html;
        deviceList.appendChild(listItem); 

                              
    },
    connect: function(e) {
        var uuid = e.target.getAttribute('uuid'),
        cachedUUID = uuid;
            onConnect = function() {
                rfduino.onData(app.onData, app.onError);
                app.showDetailPage();
            };

        rfduino.connect(uuid, onConnect, app.onError);
    },
    sendTime: function() {
        rfduino.write(hour+minute+goldenHour+goldenMinute, app.writeSuccess, app.onError);
    },
    onData: function(data) {
        // rfduino.write("3", app.writeSuccess, app.onError);
        document.getElementById('sunlightMinutes').innerHTML = 'Minutes in Sunlight: ' + arrayBufferToInt(data);
    },
    disconnect: function() {
        rfduino.disconnect(app.showMainPage, app.onError);
    },
    showMainPage: function() {
        mainPage.hidden = false;
        detailPage.hidden = true;
    },
    showDetailPage: function() {
                            mainPage.hidden = true;
        detailPage.hidden = false;
    },
    onError: function(reason) {
        alert(reason); // real apps should use notification.alert
    },
    writeSuccess: function(reason){
        // alert("you've sent info" + reason);
        // can use this to debog if you are having trouble sending
    },

};