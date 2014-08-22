// onSuccess Callback
// This method accepts a Position object, which contains the
// current GPS coordinates
//

var latitude;
var longitude;
var month;
var day;
var year;
var zenith = 90.83333333333333;
var locationFlag = 0;
var daylightSavingsIsTrue;

document.addEventListener("deviceready", onDeviceReady, false);

function onDeviceReady() {
    navigator.geolocation.getCurrentPosition(onSuccess, onError);
};

var onSuccess = function(position) {
	latitude = position.coords.latitude;
	longitude = position.coords.longitude;
	var times = SunCalc.getTimes(new Date(), latitude, longitude);
	goldenHour = times.goldenHour.getHours() - 12;
	goldenMinute = times.goldenHour.getMinutes();
	hour = new Date().getHours();
	minute = new Date().getMinutes();
	var AMPM = "AM";

	if (minute < 10) {
		minute = '0' + parseInt(minute);
	}

	if (goldenMinute < 10) {
		goldenMinute = '0' + parseInt(goldenMinute);
	}

	if (hour > 11) {
		hour = hour - 12;
		AMPM = "PM";
	}

	if (hour < 10) {
		hour = '0' + parseInt(hour);
	}

	if (goldenHour < 10) {
		goldenHour = '0' + parseInt(goldenHour);
	}

	document.getElementById('currentTime').innerHTML = hour + ':' + minute + ' ' + AMPM;
	document.getElementById('sunsetTime').innerHTML = 'Golden Hour Begins: ' + goldenHour + ':' + goldenMinute;
	// getDateString();
};

function onError(error) {
    alert('code: '    + error.code    + '\n' +
          'message: ' + error.message + '\n');
};

function getDateString() {
  navigator.globalization.dateToString(
    new Date(),
    function (date) {
    	dateArray = date.value.split("/");
    	month = parseInt(dateArray[0]);
    	day = parseInt(dateArray[1]);
    	year = parseInt(dateArray[2]);
    	checkDayLightSavings();
    },
    function () {alert('Error getting dateString\n')},
    {formatLength:'short', selector:'date'});
};

function checkDayLightSavings() {
  navigator.globalization.isDayLightSavingsTime(
    new Date(),
    function (date) {
    	daylightSavingsIsTrue = date.dst;
    	getSunsetTime()
    },
    function () {alert('Error getting names\n');}
  );
};

function getSunsetTime() {
    var D2R = Math.PI / 180;
    var R2D = 180 / Math.PI;
	var N1 = Math.floor(275 * month / 9);
	var N2 = Math.floor((month + 9) / 12);
	var N3 = (1 + Math.floor((year - 4 * Math.floor(year / 4) + 2) / 3));

	var N = N1 - (N2 * N3) + day - 30;
	var lngHour = longitude / 15;
	var t = N + ((18 - lngHour) / 24);
	var M = (0.9856 * t) - 3.289;


	var L = M + (1.916 * Math.sin(M * D2R)) + (0.020 * Math.sin(2 * M * D2R)) + 282.634;
	if (L < 0) {
		L = L + 360;
	} else if (L >= 360) {
		L = L - 360;
	}

	var RA = R2D * Math.atan(0.91764 * Math.tan(L * D2R));
	if (RA < 0) {
		RA = RA + 360;
	} else if (RA >= 360) {
		RA = RA - 360;
	}
	var Lquadrant  = (Math.floor( L/90)) * 90;
	var RAquadrant = (Math.floor(RA/90)) * 90;
	RA = RA + (Lquadrant - RAquadrant);
	RA = RA / 15;

	var sinDec = 0.39782 * Math.sin(L * D2R);
	var cosDec = Math.cos(Math.asin(sinDec));

	var cosH = (Math.cos(zenith * D2R) - (sinDec * Math.sin(latitude * D2R))) / (cosDec * Math.cos(latitude * D2R));
	if (cosH < -1) {
		alert("The sun never sets on this location today");
	}
	var H = R2D * Math.acos(cosH);
	H = H / 15;
	var T = H + RA - (0.06571 * t) - 6.622
	var UT = T - lngHour;

	if (UT < 0) {
		UT = UT + 24;
	} else if (UT >= 24) {
		UT = UT - 24;
	}

	if (daylightSavingsIsTrue) {
		UT = UT + 1;
	}
	var rightNow = new Date();
	var jan1 = new Date(rightNow.getFullYear(), 0, 1, 0, 0, 0, 0);
	var temp = jan1.toGMTString();
	var jan2 = new Date(temp.substring(0, temp.lastIndexOf(" ")-1));
	var std_time_offset = (jan1 - jan2) / (1000 * 60 * 60);
	var localT = UT + std_time_offset;

	if (localT < 0) {
		localT = localT + 24;
	} else if (localT >= 24) {
		localT = localT - 24;
	}
	document.getElementById('sunsetTime').innerHTML = 'Sunset Time: ' + localT;
}






