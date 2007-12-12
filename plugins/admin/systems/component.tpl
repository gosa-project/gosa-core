<table summary="" width="100%">
 <tr>
  <td style="vertical-align:top; width:50%; border-right:1px solid #A0A0A0">
	<table summary="">
	 <tr>
	  <td><LABEL for="cn">{t}Device name{/t}</LABEL>{$must}</td>
	  <td>
{render acl=$cnACL}
	   <input name="cn" id="cn" size=20 maxlength=60 value="{$cn}">
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
	    <select id="base" size="1" name="base" title="{t}Choose subtree to place group in{/t}">
	     {html_options options=$bases selected=$base_select}
	    </select>
{/render}
{render acl=$baseACL disable_picture='images/folder_gray.png'}
        <input type="image" name="chooseBase" src="images/folder.png" class="center" title="{t}Select a base{/t}">
{/render}

          </td>
	  </tr>
	</table>
  </td>
  <td style="vertical-align:top">
	<table summary="">
	 <tr>
	  <td><LABEL for="description">{t}Description{/t}</LABEL></td>
	  <td>	
{render acl=$descriptionACL}
	   <input id="description" name="description" size=25 maxlength=80 value="{$description}">
{/render}
          </td>
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
