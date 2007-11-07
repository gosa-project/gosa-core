<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
	"http://www.w3.org/TR/html4/transitional.dtd">
<html>
<head>
<style type="text/css">
	
.ObjectListViewport_Headline_Table {
	background-color: #FF0000;
	width:100%;
	border-collapse:collapse;
}
.ObjectListViewport_Headline_Cell {
	border: 1px solid #000000;
}
.ObjectListViewport_Entry_Table {
	background-color: #00FF00;
	width:100%;
	border-collapse:collapse;
}
.ObjectListViewport_Entry_Cell {
	border: 1px solid #000000;
}
.ObjectListViewport_Footer_Table {
	background-color: #0000FF;
	width:100%;
	border-collapse:collapse;
}
.ObjectListViewport_Footer_Cell {
	border: 1px solid #000000;
}
</style>
<script type="text/javascript">
	function setHeight() {
		dbg = document.getElementById('debug');
		OVP = document.getElementById('ObjectListViewport');
		style = getComputedStyle(OVP,"");
		dbg.value = style.height;
		document.getElementById('ObjectListViewport_Entry_Cover').style.height= '200px' ;
	}
</script>
</head>
<body onmouseover="setHeight();" >
<input type='text' id='debug' value="">
<div id='ObjectListViewport' style='height:20%;'>
<?php
echo passthru("./test");
?>
</div>
</body>
</html>
