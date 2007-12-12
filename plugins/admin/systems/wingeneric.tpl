<table summary="" width="100%">
 <tr>
  <td style="vertical-align:top; width:50%;">
	<table summary="">
	 <tr>
	  <td><LABEL for="cn">{t}Machine name{/t}</LABEL>{$must}</td>
	  <td><input id="cn" name="cn" size=20 maxlength=60 value="{$cn}"></td>
	 </tr>
	 <tr>
          <td colspan=2>&nbsp;</td>
	 </tr>
 	 <tr>
	  <td><LABEL for="base">{t}Base{/t}</LABEL>{$must}</td>
	  <td>
	   <select size="1" id="base" name="base" title="{t}Choose subtree to place terminal in{/t}">
	    {html_options options=$bases selected=$base_select}
	   </select>
		<input type="image" name="chooseBase" src="images/folder.png" class="center" title="{t}Select a base{/t}">
	  </tr>
	</table>
  </td>
  <td style="vertical-align:top">
	<table summary="">
	 <tr>
	  <td><LABEL for="description">{t}Description{/t}</LABEL></td>
	  <td><input id="description" name="description" size=25 maxlength=80 value="{$description}"></td>
	 </tr>
	</table>
  </td>
 </tr>
</table>

<p class="plugbottom" style="margin-bottom:0px; padding:0px;">&nbsp;</p>

{$netconfig}

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('cn');
  -->
</script>
