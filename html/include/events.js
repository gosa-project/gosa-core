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
        if(document['loadHandler']) {
               loadHandler();
        }
}

function OnResize()
{
	adjust();
        if(document['resizeHandler']) {
	  resizeHandler();
        }
}
