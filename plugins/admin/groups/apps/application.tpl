
{if $multiple_support}

	<h2>Multiple edit</h2>

	{if $use_apps}
		<input type="checkbox" name="use_apps" value="1" checked class="center"
			onClick="toggle('div_use_apps');">
		{t}Modify application settings.{/t}
		<br>
		<div id="div_use_apps" style="visibility: visible;" >
	{else}
		<input type="checkbox" name="use_apps" value="1" class="center"
			onClick="toggle('div_use_apps');">
		{t}Modify application settings for groups.{/t}
		<br>
		<div style="visibility: hidden;" id="div_use_apps">
	{/if}	
{/if}

{if $enableReleaseManagement == true}
	<br>
	<b>{t}Release focus{/t}</b>&nbsp;
	{if $ReleaseSelectAble}

{render acl=$FAIreleaseACL}
		<select name="FAIrelease" title="{t}Select release name{/t}" onChange="javascript: document.mainform.submit();">
			{html_options output=$Releases values=$Releases selected=$FAIrelease}
		</select>
{/render}
	{else}
		{$Release}
	{/if}
	<p class="seperator">&nbsp;</p>
{
/if}
<table summary="" style="width:100%;">
 <tr>
  <td style="width:48%; vertical-align:top;">
   <b>
	<LABEL for="used_apps">{t}Used applications{/t}</LABEL>
   </b>
   {$UsedApps}
{render acl=$gosaMemberApplicationACL} 
   <input type="text" value="" size="30" name='CatName'>
{/render}
{render acl=$gosaMemberApplicationACL} 
   <input type="submit" value="{t}Add category{/t}" name='AddCat'>
{/render}
  </td>
  <td>&nbsp;
  </td>
  <td style="width:48%;vertical-align:top;">
   <b>
    <LABEL for="apps">{t}Available applications{/t}</LABEL>
   </b>
   <br>
   {$List}
{render acl=$gosaMemberApplicationACL} 
   <input type="submit" value="{t}Add{/t}" name='AddApps'>
{/render}
  </td>
 </tr>
</table>
{if $multiple_support}
	</div>
	<input type="hidden" value="1" name="group_apps_multi">
{/if}

