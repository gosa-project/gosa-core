<body style='background-color:white;background-image:none'>
<div style='background-color:#CDF6BD;width:100%;padding-top:10px;border-bottom:1px solid #909090'>
<form action='helpviewer.php' name='mainform' method='post' enctype='multipart/form-data'>
  <table width="99%">
   <tr>
    <td width="33%">
     <b>{t}GOsa help viewer{/t}</b>
	</td>
    <td width="33%" style="text-align:center">
		<a href="?pg={$backward}">
			<img src='images/back.png' align="middle" alt="&lt; {t}previous{/t}" border="0">
		</a>
		&nbsp;&nbsp;
		<a href="?pg={$index}">
			{t}Index{/t}
		</a>
		&nbsp;&nbsp;
		<a href="?pg={$forward}"> 
			<img src='images/forward.png' align="middle" alt="{t}next{/t}" border="0">
		</a>
	</td>
	<td style="text-align:right">
	 <input name="search_string" size="15" value="{$search_string}" maxlength="50">&nbsp;<input type=submit name="search" value="{t}Search{/t}">
	</td>
   </tr>
  </table>
</form>
</div>
<div style="height: 645px;
			   width: 100%;
			   overflow: auto;
			   margin: 0 ;
			   background-color: #F1F1F1;">
{$help_contents}
</div>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
<!-- // First input field on page
document.mainform.search_string.focus();
-->
</script>

</body>
</html>
