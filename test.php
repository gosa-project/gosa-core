<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<style type="text/css">

.ObjectListViewport {
	width:100%;
	height:100%;
	border-collapse:collapse;
}

.ObjectListViewport_Header_Table {
	width:100%;
	border: 0px;
	border-collapse:collapse;
}	
	
.ObjectListViewport_Entry_Table {
	width:100%;
	border: 0px;
	border-collapse:collapse;
}

.ObjectListViewport_Footer_Table {
    width:100%;
    border: 0px;
    border-collapse:collapse;
}


.ObjectListViewport_Table {
	border-collapse:collapse;
    width:100%;
	height:100%;
	border: 0px;
}

.ObjectListViewport_TD_Header {
	height:20px;
	background-color: #CCCCCC;
	vertical-align:middle;
	border: 0px;
	margin:0px;
	padding:0px;
}
.ObjectListViewport_TD_Entries {
	vertical-align:middle;
	border: 0px;
	margin:0px;
	padding:0px;
}
.ObjectListViewport_TD_Footer {
	height:20px;
	background-color: #CCCCCC;
	vertical-align:middle;
	border: 0px;
	margin:0px;
	padding:0px;
}
.ObjectListViewport_Entry_Cover{
	overflow:auto;
	height:100px;
}
.ObjectListViewport_Entry_Cell {
  	background-color: white;
    background-image: none;
    background-repeat: repeat;
    background-attachment: scroll;
    -x-background-x-position: 0%;
    -x-background-y-position: 0%;
    -moz-background-clip: -moz-initial;
    -moz-background-origin: -moz-initial;
    -moz-background-inline-policy: -moz-initial;
    border-right-width: 1px;
    border-right-style: solid;
    border-right-color: rgb(192, 192, 192);
    padding-top: 3px;
    padding-right-value: 3px;
    padding-bottom: 3px;
    padding-left-value: 3px;
    padding-left-ltr-source: physical;
    padding-left-rtl-source: physical;
    padding-right-ltr-source: physical;
    padding-right-rtl-source: physical;
    height: 22px;
}

.ObjectListViewport_Footer_Cell {

    background-color: rgb(240, 240, 240);
    background-image: none;
    background-repeat: repeat;
    background-attachment: scroll;
    -x-background-x-position: 0%;
    -x-background-y-position: 0%;
    -moz-background-clip: -moz-initial;
    -moz-background-origin: -moz-initial;
    -moz-background-inline-policy: -moz-initial;
    font-weight: bold;
    border-top-width: 1px;
    border-top-style: solid;
    border-top-color: rgb(170, 170, 170);
    padding-top: 3px;
    padding-right-value: 3px;
    padding-bottom: 3px;
    padding-left-value: 3px;
    padding-left-ltr-source: physical;
    padding-left-rtl-source: physical;
    padding-right-ltr-source: physical;
    padding-right-rtl-source: physical;
    height: 22px;
}

.ObjectListViewport_Header_Cell {
    background-color: rgb(240, 240, 240);
    background-image: none;
    background-repeat: repeat;
    background-attachment: scroll;
    -x-background-x-position: 0%;
    -x-background-y-position: 0%;
    -moz-background-clip: -moz-initial;
    -moz-background-origin: -moz-initial;
    -moz-background-inline-policy: -moz-initial;
    font-weight: bold;
    border-right-width: 1px;
    border-right-style: solid;
    border-right-color: rgb(170, 170, 170);
    border-bottom-width: 1px;
    border-bottom-style: solid;
    border-bottom-color: rgb(170, 170, 170);
    padding-top: 3px;
    padding-right-value: 3px;
    padding-bottom: 3px;
    padding-left-value: 3px;
    padding-left-ltr-source: physical;
    padding-left-rtl-source: physical;
    padding-right-ltr-source: physical;
    padding-right-rtl-source: physical;
    height: 22px;
}
</style>
</head>
<script type="text/javascript">

<!--

/* Register resize event to force redraw of all 
 *  displayed lists 
 */
window.onload = updateObjectListViewportSize;
window.onresize = updateObjectListViewportSize;

	function updateObjectListViewportSize() {

		var dbg = document.getElementById('debug');
		dbg.value ="";

//	var all = getElementsByStyleClass('ObjectListViewport');
//	for (var i = 0; i < all.length; ++i){
//		alert(all[i].id);
//		document.getElementById(all[i].id).style.display= "block";
//		document.getElementById(all[i].id).style.height= "50px";
//	}
		
		/* Somehow IE do not need to be resized 
         *  So skip the following part if the client uses ie
         */
		if(document.all && !window.opera){
			var all = getElementsByStyleClass('ObjectListViewport_Entry_Cover');
			for (var i = 0; i < all.length; ++i){
				document.getElementById(all[i].id).style.height= "100%";
			}
			return;
		}
	
		var all = getElementsByStyleClass('ObjectListViewport');
		for (var i = 0; i < all.length; ++i){

			var id_pref = all[i].id.replace(/[^0-9]*/,"");

			/* Set Viewport to min height, to ensure 
			 *  that resize will work correctly in konqueror 
			 */
			document.getElementById('ObjectListViewport_Entry_Cover' + id_pref).style.height= "50px";

			/* Get values of displayed header and footer heights to be 
			 *  able to recalculate the Viewport 
			 */
			var viewport	= getObjectHeight('ObjectListViewport_Table' + id_pref);
			var header  	= getObjectHeight('ObjectListViewport_TD_Header' + id_pref);
			var footer  	= getObjectHeight('ObjectListViewport_TD_Footer' + id_pref);

			/* Calculate the new visible entry part height. */
			var calc = (viewport  - ( header + footer ));
			document.getElementById('ObjectListViewport_Entry_Cover' + id_pref).style.height = calc;

			/* Reduce width of entry list, we need some space to 
			 *  display the scrollbar without breaking the layout
			 */
			dbg.value += " " + calc;
			var viewport_w = getObjectWidth('ObjectListViewport_Table' + id_pref);
			document.getElementById('ObjectListViewport_Entry_Table' + id_pref).style.width = (viewport_w - 16) + 'px';
		}
	}

	
	/* Return all elements of a specific class */	
	function getElementsByStyleClass (className) {
		var all = document.all ? document.all :	document.getElementsByTagName('*');
		var elements = new Array();
		for (var e = 0; e < all.length; e++)
			if (all[e].className == className)
				elements[elements.length] = all[e];
		return elements;
	}


	/* Return integer value of style attribute width for specified ID  */
	function getObjectWidth(obj){
		obj = getComputedStyle(document.getElementById(obj),"").width;
		return(parseInt(obj.replace(/px/,'')));	
	}

	/* Return integer value of style attribute height for specified ID  */
	function getObjectHeight(obj){
		obj = getComputedStyle(document.getElementById(obj),"").height;
		return(parseInt(obj.replace(/px/,'')));
	}	

-->
</script>
<body>
<input type='button' onclick='updateObjectListViewportSize();'>
<?php
$a1 = shell_exec("./test");
$a2 = shell_exec("./test");
$a3 = shell_exec("./test");
$a4 = shell_exec("./test");
$a5 = shell_exec("./test");
$a6 = shell_exec("./test");
?>
<input type='text' id='debug' value="" style='width:100%;'>
    <table style='height:90%;width:90%;background-color: rgb(170, 170, 170);'
        cellspacing=1 cellpadding=0>
        <tr>
            <td style='width:30%;height:30%;'>
                <?php echo $a1; ?>
            </td>
            <td>
                <?php echo $a2; ?>
            </td>
        </tr>
        <tr>
            <td>
                <?php echo $a3; ?>
            </td>
            <td>
                <?php echo $a4; ?>
            </td>
        </tr>
        <tr>
            <td>
                <?php echo $a5; ?>
            </td>
            <td>
                <?php echo $a6; ?>
            </td>
        </tr>
    </table>

</body>
</html>
