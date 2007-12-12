<div style="height: 5px;">&nbsp;</div>
<div class="contentboxh">
 <p class="contentboxh"><img src="images/launch.png" alt="[F]" align="right">Filter</p>
</div>
<div class="contentboxb">
  <div >
	<table width="100%" summary="">
		<tr>
			<td>
	<img class="center" alt="{t}Search{/t}" src="images/search.png" align="middle" border="0">
	 &nbsp;{t}Search for{/t}
    <input name="search_for" size="12" maxlength="60" value="{$search_for}" title="{t}Please enter a search string here.{/t}" onchange="mainform.submit()">
 	 &nbsp;in 
	<select size="1" name="p_server" title="{t}Select a server{/t}" onchange="mainform.submit()">
	 {html_options values=$p_serverKeys output=$p_servers selected=$p_server}
	</select>
	{t}with status{/t} : 
	<select size="1" name="Stat" onchange="mainform.submit()">
	 {html_options values=$stats output=$r_stats selected=$stat}
	</select>
	 &nbsp;{t}within the last{/t}&nbsp;
	<select size="1" name="p_time" onchange="mainform.submit()">
	 {html_options values=$p_timeKeys output=$p_times selected=$p_time}
	</select>
	 &nbsp;
	<input name="search" value="{t}Search{/t}" type="submit">
			</td>
			<td style="border-left:1px solid #A0A0A0; text-align:right;">&nbsp;
				<input name="del_all"  src="images/edittrash.png"			value="{t}Remove all messages{/t}" 		type="image" 
					title="{t}Remove all messages from selected servers queue{/t}">
				<input name="hold_all"    src="images/mailq_hold.png"			value="{t}Hold all messages{/t}" 		type="image"
					title="{t}Hold all messages in selected servers queue{/t}">
				<input name="unhold_all"    src="images/mailq_unhold.png"		value="{t}Release all messages{/t}" 	type="image"
					title="{t}Release all messages in selected servers queue{/t}">
				<input name="requeue_all" src="images/mailq_requeue.png"		value="{t}Requeue all messages{/t}" 	type="image"
					title="{t}Requeue all messages in selected servers queue{/t}">
   			</td>
		</tr>
	</table>
	</div>
</div>
<br>


{if $all_ok != "true"}
<b>{t}Search returned no results{/t}...</b>
{else}

<table style="border: 1px solid rgb(176, 176, 176); width: 100%; vertical-align: top; text-align: left;" summary=""
 border="0" cellpadding="2" cellspacing="1" rules="cols">
	<tr style="background-color: rgb(232, 232, 232); height: 26px; font-weight: bold;">
		<td><a href="{$plug}&amp;sort=MailID"		>{t}ID{/t}			{if $OrderBy == "MailID"}	{$SortType}{/if}</a></td>
		<td><a href="{$plug}&amp;sort=Server"		>{t}Server{/t}		{if $OrderBy == "Server"}	{$SortType}{/if}</a></td>
		<td><a href="{$plug}&amp;sort=Size"			>{t}Size{/t}		{if $OrderBy == "Size"}		{$SortType}{/if}</a></td>
		<td><a href="{$plug}&amp;sort=Arrival"		>{t}Arrival{/t}		{if $OrderBy == "Arrival"}	{$SortType}{/if}</a></td>
		<td><a href="{$plug}&amp;sort=Sender"		>{t}Sender{/t}		{if $OrderBy == "Sender"}	{$SortType}{/if}</a></td>
		<td><a href="{$plug}&amp;sort=Recipient"	>{t}Recipient{/t}	{if $OrderBy == "Recipient"}{$SortType}{/if}</a></td>
		<td><a href="{$plug}&amp;sort=Error"		>{t}Error{/t}		{if $OrderBy == "Error"}	{$SortType}{/if}</a></td>
		<td>&nbsp;</td>
	</tr>

{counter start=0 assign=i start=1}
{foreach from=$entries item=val key=key}
			
	{if ($i%2)== 0 }
		<tr style="height: 22px; background-color: rgb(236, 236, 236);">
	{else}
		<tr style="height: 22px; background-color: rgb(245, 245, 245);">
	{/if}
		<td >
<!-- onlick="javascript:	document.mainform.header_server.value='{$entries[$key].Server}';
								document.mainform.header_id.value='{$entries[$key].MailID}';
								document.mainform.submit();"
-->
			{if $entries[$key].Active == true}
				<img class="center" src="images/mailq_active.png" border=0 alt="{t}Active{/t}">
			{/if}
			
			{$entries[$key].MailID}</td>
		<td> 
<!-- onclick="javascript: document.location.href='{$plug}&amp;act=header&amp;id={$entries[$key].MailID}&amp;server={$entries[$key].Server}'"-->
			{$entries[$key].Server}</td>
		<td>
			{$entries[$key].Size}</td>
		<td>
			{$entries[$key].Arrival}</td>
		<td>
			{$entries[$key].Sender}</td>
		<td>
			{$entries[$key].Recipient}</td>
		<td >{$entries[$key].Error}</td>
		<td style="text-align:right">
			<a href="{$plug}&amp;act=del&amp;id={$entries[$key].MailID}&amp;server={$entries[$key].Server}">
				<img class="center" src="images/edittrash.png" border=0 alt="{t}delete{/t}" title="{t}Delete this message{/t}">
			</a>
		{if $entries[$key].Hold == true}
			<a href="{$plug}&amp;act=unhold&amp;id={$entries[$key].MailID}&amp;server={$entries[$key].Server}">
				<img class="center" src="images/mailq_unhold.png" border=0 alt="{t}unhold{/t}" title="{t}Release message{/t}">
			</a>
		{else}
			<a href="{$plug}&amp;act=hold&amp;id={$entries[$key].MailID}&amp;server={$entries[$key].Server}">
				<img class="center" src="images/mailq_hold.png" border=0 alt="{t}hold{/t}" title="{t}Hold message{/t}">
			</a>
		{/if}
			<a href="{$plug}&amp;act=requeue&amp;id={$entries[$key].MailID}&amp;server={$entries[$key].Server}">
				<img class="center" src="images/mailq_requeue.png" border=0 alt="{t}requeue{/t}" title="{t}Requeue this message{/t}">
			</a>
			<a href="{$plug}&amp;act=header&amp;id={$entries[$key].MailID}&amp;server={$entries[$key].Server}">
				<img class="center" src="images/mailq_header.png" border=0 alt="{t}header{/t}" title="{t}Display header from this message{/t}">
			</a>
		</td>
	</tr>
	{counter}
{/foreach}
</table>

 <table summary="" style="width:100%; vertical-align:top; text-align:center;" cellpadding=4 cellspacing=0 border=0>
  <tr>
   <td>{$range_selector}</td>
  </tr>
 </table>
<p class="plugbottom">&nbsp;</p>

{/if}

<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('search_for');
  -->
</script>
