<h2>{t}Apache2 VHosts{/t}</h2>
<table summary="" width="100%">
<tr>
	<td style="width:100%;vertical-align:top;">
		{$VhostList}
		<input type="submit" name="AddVhost" value="{t}Add{/t}" {$servapache2ACL}>
<!--		<input type="submit" name="EditVhost" value="{t}Edit{/t}" {$servapache2ACL}>-->
<!--		<input type="submit" name="RemoveVhost" value="{t}Remove{/t}" {$servapache2ACL}>-->
	</td>
</tr>
</table>
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.AddVhost.focus();
  -->
</script>
