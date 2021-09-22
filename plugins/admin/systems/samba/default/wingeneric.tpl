<table summary="{t}Windows workstation{/t}" width="100%">
 <tr>
  <td style='width:50%;'>

	<table summary="{t}Generic settings{/t}">
	 <tr>
	  <td><LABEL for="cn">{t}Machine name{/t}</LABEL>{$must}</td>
	  <td>
{render acl=$cnACL}
			<input type='text' id="cn" name="cn" size=20 maxlength=60 value="{$cn}">
{/render}
	  </td>
	 </tr>
	 <tr>
          <td colspan=2>&nbsp;</td>
	 </tr>
 	 <tr>
	  <td><LABEL for="base">{t}Base{/t}</LABEL>{$must}</td>
	  <td>
{render acl=$baseACL}
{$base}
{/render}
	   </td>
	  </tr>
	</table>
  </td>
  <td>

	  <LABEL for="description">{t}Description{/t}</LABEL>
{render acl=$descriptionACL}
		<input type='text' id="description" name="description" size=25 maxlength=80 value="{$description}">
{/render}
  </td>
 </tr>
</table>

<hr>

{$netconfig}

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  	// First input field on page
	focus_field('cn');
</script>
