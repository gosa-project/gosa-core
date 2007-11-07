 <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN"
            "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<style type="text/css">

.ObjectListViewport {
	width:100%;
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

.ObjectListViewport_Entry_Cover{

	border: 0px;
	margin:0px;
	padding:0px;
}

.ObjectListViewport_Footer_Table {
    width:100%;
    border: 0px;
    border-collapse:collapse;
}


.ObjectListViewport_Table {
	border-collapse:collapse;
    width:100%;
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
	background-color: #009900;
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
td {
	border-left: solid 1px;
	border-right: solid 1px;
	font-size:1em;
	height:22px;
}

</style>
<script type="text/javascript">
window.onload = setHeight;
window.onresize = setHeight;

	function setHeight() {
		dbg = document.getElementById('debug');

		OVP = getComputedStyle(document.getElementById('ObjectListViewport'),"").height;
		OVP_h = parseInt(OVP.replace(/px/,''));

		table 		  = getComputedStyle(document.getElementById('ObjectListViewport_Table'),"").height;
		table_header  = getComputedStyle(document.getElementById('ObjectListViewport_TD_Header'),"").height;
		table_entries = getComputedStyle(document.getElementById('ObjectListViewport_TD_Entries'),"").height;
		table_footer  = getComputedStyle(document.getElementById('ObjectListViewport_TD_Footer'),"").height;

		table_h 		= parseInt(table.replace(/px/,''));
		table_header_h 	= parseInt(table_header.replace(/px/,''));
		table_entries_h = parseInt(table_entries.replace(/px/,'')); 
		table_footer_h 	= parseInt(table_footer.replace(/px/,''));
			
		calc = (OVP_h  - ( table_header_h + table_footer_h ));
		dbg.value = calc; 
		document.getElementById('ObjectListViewport_Entry_Cover').style.height= calc + 'px' ;

		OVP_T = getComputedStyle(document.getElementById('ObjectListViewport_Table'),"").width;
		OVP_T_w = parseInt(OVP_T.replace(/px/,'',OVP_T));
		

		document.getElementById('ObjectListViewport_Entry_Table').style.width = (OVP_T_w - 16) + 'px';
		dbg.value  = OVP_T_w + document.getElementById('ObjectListViewport_Entry_Table').style.width; 
	}
</script>
</head>
<body>
<input type='text' id='debug' value="" style='width:100%;'>
	<table style='height:90%;width:100%;background-color:#6788FF;border: solid 5px #000000;'>
		<tr>
			<td style='width:50%;background-color:#AAA006;'>
				hallo<br>
				hallo<br>
				hallo<br>
				hallo<br>
				hallo<br>
				hallo<br>
			</td>
			<td style='height:230px;background-color:#660099;'>
				<?php
				echo passthru("./test");
				?>
			</td>
		</tr>
		<tr>
			<td style='background-color:#999777;'>
				hallo<br>
				hallo<br>
			</td>
			<td style='background-color:#AAA006;'>
				hallo<br>
				hallo<br>
			</td>
		</tr>
	</table>
<script type="text/javascript">
//	document.getElementById('ObjectListViewport_Entry_Cover').style.height= document.getElementById('ObjectListViewport').height;
	document.getElementById('ObjectListViewport_Entry_Cover').style.height= "200px";
</script>
</body>
</html>
