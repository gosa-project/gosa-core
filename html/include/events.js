window.onload 		= OnLoad;
window.onresize 	= OnResize;
window.onkeypress	= OnKeyPress;

function OnKeyPress()
{
	keyPress();
}

function OnLoad()
{
	adjust();
	loadHandler();
}

function OnResize()
{
	adjust();
	resizeHandler();
}
