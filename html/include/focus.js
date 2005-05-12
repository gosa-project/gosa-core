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

document.onkeydown= keyDown; // work together to analyze keystrokes
if (netscape){
  document.captureEvents(Event.KEYDOWN|Event.KEYUP);
}

