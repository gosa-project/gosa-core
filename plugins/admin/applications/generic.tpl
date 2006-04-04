<table summary="" style="width:100%;">
 <tr>
  <td style="width:50%;">

   <table summary="">
     <tr>
      <td><LABEL for="cn">{t}Application name{/t}{$must}</LABEL></td>
      <td><input id="cn" name="cn" size=25 maxlength=60 value="{$cn}" title="{t}Application name{/t}" {$cnACL}></td>
    </tr>
    <tr>
     <td><LABEL for="gosaApplicationName">{t}Display name{/t}</LABEL></td>
     <td>
        <input id="gosaApplicationName" name="gosaApplicationName" size=35 maxlength=60 {$gosaApplicationNameACL} value="{$gosaApplicationName}" title="{t}Application name to be displayed (i.e. below icons){/t}">
     </td>
    </tr>
    <tr>
     <td><LABEL for="gosaApplicationExecute">{t}Execute{/t}{$must}</LABEL></td>
     <td>
        <input id="gosaApplicationExecute" name="gosaApplicationExecute" size=35 maxlength=60 {$gosaApplicationExecuteACL} value="{$gosaApplicationExecute}" title="{t}Path and/or binary name of application{/t}">
     </td>
    </tr>
    <tr>
     <td><LABEL for="description">{t}Description{/t}</LABEL></td>
     <td>
      <input id="description" name="description" size=35 maxlength=80 {$descriptionACL} value="{$description}">
     </td>
    </tr>

    <tr><td colspan=2><div style="height:15px;"></div></td></tr>
 
    <tr>
      <td><LABEL for="base">{t}Base{/t}{$must}</LABEL></td>
      <td>
       <select size="1" id="base" name="base" title="{t}Choose subtree to place application in{/t}" 
        {if $isReleaseApplikation} disabled {/if}>
        {html_options options=$bases selected=$base_select}
       </select>
      </td>
     </tr>
   </table>
  </td>
  <td style="border-left:1px solid #A0A0A0">
   &nbsp;
  </td>
  <td style="vertical-align:top;">
   <table summary="">
    <tr>
    <td>
    <LABEL for="picture_file">{t}Icon{/t}</LABEL>
    <br>
    <img alt="" src="getbin.php?rand={$rand}" border=1 style="width:48px; height:48; background-color:white; vertical-align:bottom;">
    </td>
    <td style="vertical-align:top">
    &nbsp;<br>
    <input type="hidden" name="MAX_FILE_SIZE" value="100000">
    <input name="picture_file" type="file" size="20" maxlength="255" accept="image/*.png" id="picture_file">
    <input type="submit" name="update" value="{t}Update{/t}" title="{t}Reload picture from LDAP{/t}">
    </td>
    </tr>
   </table>
  </td>
 </tr>
</table>

<p class="plugbottom" style="height:4px; margin-bottom:0px;">&nbsp;</p>

<h2><img src="images/lamp.png" alt="{t}Options{/t}" align="middle">&nbsp;{t}Options{/t}</h2>
<table summary="" style="width:100%;">
  <tr>
    <td style="width:50%;border-right:1px solid #B0B0B0; vertical-align:top">
      <input type=checkbox name="exec_for_groupmembers" value="1" {$gosaApplicationFlagsACL} {$exec_for_groupmembers}>{t}Only executable for members{/t}
      <br>
      <input type=checkbox name="overwrite_config" value="1" {$gosaApplicationFlagsACL} {$overwrite_config}>{t}Replace user configuration on startup{/t}
    </td>
    <td>
      <input type=checkbox name="place_on_desktop" value="1" {$gosaApplicationFlagsACL} {$place_on_desktop}>
      {t}Place icon on members desktop{/t}
      <br>
      <input type=checkbox name="place_in_startmenu" value="1" {$gosaApplicationFlagsACL} {$place_in_startmenu}>
      {t}Place entry in members startmenu{/t}
      <br>
      <input type=checkbox name="place_on_kicker" value="1" {$gosaApplicationFlagsACL} {$place_on_kicker}>
      {t}Place entry in members launch bar{/t}
    </td>
  </tr>
</table>

<p class="plugbottom" style="height:4px; margin-bottom:0px;">&nbsp;</p>

<table width="99%" summary="">
	<tr>
		<td>
			<h2><img src="images/fai_script.png" alt="{t}Script{/t}" align="middle">&nbsp;{t}Script{/t}</h2>
			<textarea name="gotoLogonScript" style='width:99%;height:220px;'>{$gotoLogonScript}</textarea>
			<input type="file" name="ScriptFile" value="{t}Import{/t}">
			<input type="submit" name="upLoad" value="{t}Upload{/t}">
		</td>
	</tr>
</table>

<div style="height:20px;"></div>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.cn.focus();
  -->
</script>
