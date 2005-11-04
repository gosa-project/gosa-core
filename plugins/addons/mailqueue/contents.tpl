<div style="height: 5px;">&nbsp;</div>
<div class="contentboxh">
 <p class="contentboxh"><img src="images/launch.png" alt="[F]" align="right">Filter</p>
</div>
<div class="contentboxb">
 <p class="contentboxb" style="border-top: 1px solid rgb(176, 176, 176); padding-top: 5px;">
 	<img alt="" src="images/search.png" align="middle" border="0">
		&nbsp;{t}Search for{/t}
	<input name="search_for" size="25" maxlength="60" value="{$search_for}" title="{t}Please enter a search string here.{/t}" onchange="mainform.submit()">
		&nbsp;in 
	<select size="1" name="p_server" title="{t}Select a server{/t}" onchange="mainform.submit()">
		{html_options values=$p_servers output=$p_servers selected=$p_server}
	</select>
 		&nbsp;{t}Select time periode{/t}
	<select size="1" name="p_time" onchange="mainform.submit()">
		{html_options values=$p_timeKeys output=$p_times selected=$p_time}
 	</select>
	&nbsp;
 	<input name="search" value="Search" type="submit">
 </p>
</div>
<br>
<table style="border: 1px solid rgb(176, 176, 176); width: 100%; vertical-align: top; text-align: left;"
 border="0" cellpadding="2" cellspacing="1" rules="cols">
	<tr style="background-color: rgb(232, 232, 232); height: 26px; font-weight: bold;">
		<td><a href="{$plug}&sort=MailID"		>{t}ID{/t}			{if $OrderBy == "MailID"}	{$SortType}{/if}</a></td>
		<td><a href="{$plug}&sort=Size"			>{t}Size{/t}		{if $OrderBy == "Size"}		{$SortType}{/if}</a></td>
		<td><a href="{$plug}&sort=Arrival"		>{t}Arrival{/t}		{if $OrderBy == "Arrival"}	{$SortType}{/if}</a></td>
		<td><a href="{$plug}&sort=Sender"		>{t}Sender{/t}		{if $OrderBy == "Sender"}	{$SortType}{/if}</a></td>
		<td><a href="{$plug}&sort=Recipient"	>{t}Recipient{/t}	{if $OrderBy == "Recipient"}{$SortType}{/if}</a></td>
		<td><a href="{$plug}&sort=Error"		>{t}Error{/t}		{if $OrderBy == "Error"}	{$SortType}{/if}</a></td>
	</tr>

{counter start=0 assign=i start=1}
{foreach from=$entries item=val key=key}
	
	{if ($i%2)== 0 }
		<tr style="height: 22px; background-color: rgb(236, 236, 236);">
	{else}
		<tr style="height: 22px; background-color: rgb(245, 245, 245);">
	{/if}
		<td>{$entries[$key].MailID}</td>
		<td>{$entries[$key].Size}</td>
		<td>{$entries[$key].Arrival}</td>
		<td>{$entries[$key].Sender}</td>
		<td>{$entries[$key].Recipient}</td>
		<td titel="{$entries[$key].Error}">{$entries[$key].Error}</td>
	</tr>
	{counter}
{/foreach}
</table>

 <table summary="" style="width:100%; vertical-align:top; text-align:center;" cellpadding=4 cellspacing=0 border=0>
  <tr>
   <td>{$range_selector}</td>
  </tr>
 </table>
<p class="plugbottom">
 &nbsp;
</p>


