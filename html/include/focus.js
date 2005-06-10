var browserType;

if (document.layers) {browserType = "nn4"}
if (document.all) {browserType = "ie"}
if (window.navigator.userAgent.toLowerCase().match("gecko")) {browserType= "gecko"}

netscape = "";
ver= navigator.appVersion; len = ver.length;

for (iln = 0; iln < len; iln++){
  if (ver.charAt(iln) == "("){
    break;
  }
}
netscape= (ver.charAt(iln+1).toUpperCase() != "C");

function keyDown(DnEvents) {
  // determines whether Netscape or Internet Explorer
  k = (netscape) ? DnEvents.which : window.event.keyCode;
  if (k == 13) { // enter key pressed
    if (nextfield == 'login'){
      return true; // submit, we finished all fields
    } else { // we're not done yet, send focus to next box
      eval('document.mainform.' + nextfield + '.focus()');
      return false;
    }
  }
}

function changeState(myField) {
  document.getElementById(myField).disabled=(document.getElementById(myField).disabled)?false:true;
}

function changeSelectState(triggerField, myField) {
  if (document.getElementById(triggerField).value != 2){
	  document.getElementById(myField).disabled= true;
  } else {
	  document.getElementById(myField).disabled= false;
  }
}

function changeSubselectState(triggerField, myField) {
  if (document.getElementById(triggerField).checked == true){
	  document.getElementById(myField).disabled= false;
  } else {
	  document.getElementById(myField).disabled= true;
  }
}

function changeTripleSelectState(firstTriggerField, secondTriggerField, myField) {
  if (
  	document.getElementById(firstTriggerField).checked == true &&
	document.getElementById(secondTriggerField).checked == true){
	  document.getElementById(myField).disabled= false;
  } else {
	  document.getElementById(myField).disabled= true;
  }
}

document.onkeydown= keyDown; // work together to analyze keystrokes
if (netscape){
  document.captureEvents(Event.KEYDOWN|Event.KEYUP);
}

function hide(element) {
  if (browserType == "gecko" )
     document.poppedLayer = document.getElementById(element);
  else if (browserType == "ie")
     document.poppedLayer = document.all[element];
  else
     document.poppedLayer = document.layers[element];
  document.poppedLayer.style.visibility = "hidden";
}

function show(element) {
  if (browserType == "gecko" )
     document.poppedLayer = document.getElementById(element);
  else if (browserType == "ie")
     document.poppedLayer = document.all[element];
  else
     document.poppedLayer = document.layers[element];
  document.poppedLayer.style.visibility = "visible";
}

function toggle(element) {
  if (browserType == "gecko" )
     document.poppedLayer = document.getElementById(element);
  else if (browserType == "ie")
     document.poppedLayer = document.all[element];
  else
     document.poppedLayer = document.layers[element];

  if (document.poppedLayer.style.visibility == "visible")
	  hide (element);
  else
	  show (element);

}

function popup(target, name) {
	var mypopup= 
		window.open(
			target,
			name,
			"width=600,height=700,location=no,toolbar=no,directories=no,menubar=no,status=no,scrollbars=yes"
		);
	mypopup.focus();
	return false;
}
