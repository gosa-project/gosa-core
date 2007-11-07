<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
	"http://www.w3.org/TR/html4/transitional.dtd">
<html>
<head>
<style type="text/css">
	
.ObjectListViewport_Headline_Table {
	background-color: #FF0000;
	width:100%;
	height:20px;
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
	height:20px;
	width:100%;
	border-collapse:collapse;
}
.ObjectListViewport_Footer_Cell {
	border: 1px solid #000000;
}
</style>
<script type="text/javascript">
	function setHeight() {
	//	document.getElementById("tolletable").style.height=document.defaultView.getComputedStyle(document.getElementById("tolletable"),"").getPropertyValue("height"))+"px";
		alert(document.getElementById("tolletable").style.height());
	}
</script>
</head>
<body>

<table onmouseover="setHeight();" id="tolletable" style='height:50%;background-color: #00FF00;'><tr><td></td></tr></table>

<table style='height:30px; width:50%; background-color:#FF0000;'>
<tr>
<td>
<?php
echo passthru("./test");
?>
</td>
</tr>
</table>
</div>
</body>
</html>
