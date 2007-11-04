<h2>{t}Apache VHosts{/t}</h2>
<table summary="" width="100%">
<tr>
	<td style="width:100%;vertical-align:top;">
		{$VhostList}
		<input type="submit" name="AddVhost" value="{t}Add{/t}" {$servapacheACL}>
<!--		<input type="submit" name="EditVhost" value="{t}Edit{/t}" {$servapacheACL}>-->
<!--		<input type="submit" name="RemoveVhost" value="{t}Remove{/t}" {$servapacheACL}>-->
	</td>
</tr>
</table>
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.AddVhost.focus();
  -->
</script>
