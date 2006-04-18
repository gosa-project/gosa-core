{if $enableReleaseManagement == true}
	<br>
	<b>{t}Release focus{/t}</b>&nbsp;
	{if $ReleaseSelectAble}
		<select name="Release" title="{t}Select release name{/t}" onChange="javascript: document.mainform.submit();">
			{html_options output=$Releases values=$Releases selected=$Release}
		</select>
	{else}
		{$Release}
	{/if}
	<p class="seperator">&nbsp;</p>
{/if}
<table summary="" style="width:100%;">
 <tr>
  <td style="width:48%; vertical-align:top;">
   <b><LABEL for="used_apps">{t}Used applications{/t}</LABEL></b>
  	{$UsedApps} 
	<input type="text" value="" size="30" name='CatName'>
	<input type="submit" value="{t}Add category{/t}" name='AddCat'>
  </td>
  <td>&nbsp;
  </td>
  <td style="width:48%;vertical-align:top;">
  <b><LABEL for="apps">{t}Available applications{/t}</LABEL></b>
  <br>
   {$List}
	<input type="submit" value="{t}Add{/t}" name='AddApps'>
 </td>
 </tr>
</table>

