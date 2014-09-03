function checkDateString() {
  navigator.globalization.dateToString(
    new Date(),
    function (date) {alert('date: ' + date.value + '\n');},
    function () {alert('Error getting dateString\n')});
}