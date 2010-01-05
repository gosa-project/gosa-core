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
}

function OnResize()
{
	adjust();
	resizeHandler();
}
