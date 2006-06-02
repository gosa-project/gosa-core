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


function keyPress(DnEvents) {
  // determines whether Netscape or Internet Explorer
  k = (netscape) ? DnEvents.keyCode : window.event.keyCode;
  if (k == 13) { // enter key pressed
		if(typeof(nextfield)!='undefined') {
			if(nextfield == 'login') {
    	  return true; // submit, we finished all fields
    	} else { // we are not done yet, send focus to next box
      	eval('document.mainform.' + nextfield + '.focus()');
      	return false;
    	}
  	} else {
			if(netscape) {
				if(DnEvents.target.type == 'textarea') {
					return true;
				} else if (DnEvents.target.type != 'submit') {
					// TAB
					var thisfield = document.getElementById(DnEvents.target.id);
					for (i = 0; i < document.forms[0].elements.length; i++) {
						if(document.forms[0].elements[i].id==thisfield.id) {
							// Last form element on page?
							if(i!=document.forms[0].elements.length-1) {
								document.forms[0].elements[i+1].focus();
							}
						}
					}
					return false;
				} else {
					return true;
				}
			// Check for konqueror
			} else if(document.clientWidth) {
				// do nothing ATM
			} else {
				if(window.event.srcElement.type == 'textarea') {
					return true;
				} else if (window.event.srcElement.type != 'submit') {
					// TAB
					var thisfield = document.getElementById(window.event.srcElement.id);
					for (i = 0; i < document.forms[0].elements.length; i++) {
						if(document.forms[0].elements[i].id==thisfield.id) {
							// Last form element on page?
							if(i!=document.forms[0].elements.length-1) {
								document.forms[0].elements[i+1].focus();
							}
						}
					}
					return false;
				} else {
					return true;
				}
			}
		}
	} else if (k==9) {
		// Tab key pressed
		if(netscape) {
			if(DnEvents.target.type == 'textarea') {
				document.getElementById(DnEvents.target.id).value+="\t";
		 		return false;
			}
		// Check for konqueror
		} else if(document.clientWidth) {
			// do nothing ATM
		} else {
		 	if(window.event.srcElement.type == 'textarea') {
				document.getElementById(window.event.srcElement.id).value+="\t";
		 		return false;
		 	}
		}
	}
}

function changeState(myField) {
	if(document.getElementById(myField) != null){
	  document.getElementById(myField).disabled=(document.getElementById(myField).disabled)?false:true;
	}
}

function setHidden(str) {
	type = document.getElementById(str).style.display;
	if((type=='')||(type=='block')) {
		document.getElementById(str).style.display='none';
	}else{
		document.getElementById(str).style.display='block';
	}
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

<!-- Second field must be non-checked -->
function changeTripleSelectState_2nd_neg(firstTriggerField, secondTriggerField, myField) {
  if (
  	document.getElementById(firstTriggerField).checked == true &&
	document.getElementById(secondTriggerField).checked == false){
	  document.getElementById(myField).disabled= false;
  } else {
	  document.getElementById(myField).disabled= true;
  }
}
// work together to analyze keystrokes
if (netscape){
  document.captureEvents(Event.KEYPRESS);
	document.onkeypress= keyPress;
} else {
	document.onkeydown= keyPress;
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

  if (document.poppedLayer.style.visibility == "visible") {
	  hide (element);
	} else {
	  show (element);
	}
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

function js_check(form) {
	form.javascript.value = 'true';
}

window.onload = adjust_width;
window.onresize = adjust_width;
window.onload = adjust_height;
window.onresize = adjust_height;

// Automatic resize (height) of divlists
function adjust_height(e) {
	if (!e) e=window.event;
	if (document.getElementById("menucell") && document.getElementById("d_scrollbody")) {
		var inner_height= window.innerHeight;
		var min_height= 450;
		var px_height= min_height;
		var suggested= px_height;
	
		// document.defaultView allows access to the rendered size of elements and should be supported by modern browsers
		if(document.defaultView) {
			var menu_height=parseInt(document.defaultView.getComputedStyle(document.getElementById("menucell"),"").getPropertyValue('height'));
	
			// Minimum height for divlist should be the bottom edge of the menu
			min_height= menu_height-197;
			suggested= min_height;
			if((inner_height-230)-suggested>0) {
				suggested= inner_height-230;
			}
	
		} else if(px_height >= 450) {
				suggested= px_height-230;
		}
		document.getElementById("d_scrollbody").style.height=suggested+"px";
	}
	return true;
}

// Automatic resize (width) of divlists
function adjust_width(e) {
	if (!e) e=window.event;

	// Known to not work with IE
	if(document.defaultView && document.getElementById("t_scrolltable")) {
		// Resize the div
		var div_width=parseInt(document.defaultView.getComputedStyle(document.getElementById("t_scrolltable"),"").getPropertyValue('width'));
		var width= parseInt(window.innerWidth);

		// Resize the body cells
		var diff= width-div_width-470;
		
		// window has been upscaled
		if(div_width+diff>=600) {
			document.getElementById('d_scrollbody').style.width=div_width+diff+"px";
			document.getElementById('t_scrollbody').style.width=(div_width-19)+diff+"px";
	
			// Resize the Header cells (only the relative-width ones)
			document.getElementById('t_scrollhead').style.width=div_width+diff+"px";

		// window has been downscaled, we must reset the div to 600px
		} else if (width < 930) {
			// Reset layout (set width to 600px)
			div_width=600;
			document.getElementById('d_scrollbody').style.width=div_width+"px";
			document.getElementById('t_scrollbody').style.width=(div_width-19)+"px";
	
			// Resize the Header cells (only the relative-width ones)
			document.getElementById('t_scrollhead').style.width=div_width+"px";
		}
	} else {
		// IE

	}
}

// vim:ts=2:syntax
