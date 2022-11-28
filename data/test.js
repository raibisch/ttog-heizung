var data = [];
var t = new Date();
var tempK = 20;
var tempWW= 21;


var oReq = new XMLHttpRequest();
oReq.addEventListener("load", reqListener);
//oReq.open("GET", "/fetch");
//oReq.send();

function addZero(i)
{
  if (i < 10) {
    i = "0" + i;
  }
  return i;
}

function myTime()
{
  var d = new Date();
  var h = addZero(d.getHours());
  var m = addZero(d.getMinutes());
  var s = addZero(d.getSeconds());
  document.getElementById('DATE').innerHTML = h + ":" + m + ":" + s;
}

function reqListener ()
{
  var myResponse = this.responseText;
  //console.log(this.responseText);
  var myArray = myResponse.split(",");
  //console.log(myArray);
  console.log(history);
  tempK = parseFloat(myArray[0]);
  tempWW  = parseFloat(myArray[1]);
  document.getElementById('TEMPK').innerHTML = myArray[0];
  document.getElementById('TEMPWW').innerHTML = myArray[1];
  document.getElementById('TEMPAU').innerHTML = myArray[3];
  myTime();
}

setInterval(function()
{
  var dateNow = new Date();  // current time
  //var y = Math.random();
  oReq.open("GET", "/fetch");
  oReq.send();
  //data.push([dateNow, tempK, tempWW]);
  //g.updateOptions( { 'file': data } );
}, 2000);


function openAlert()
{
    window.alert("Hello World");
}
