function discoverPeripherals() { 
	rfduino.discover(10, function(device) {
    	alert(JSON.stringify(device));
	}, failure);
};