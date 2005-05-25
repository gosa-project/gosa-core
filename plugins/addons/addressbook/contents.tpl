<table style="table-layout:fixed; width:100%; vertical-align:top; text-align:left; table-layout:auto;" cellpadding=4>
<tr>
  <td style="vertical-align:top;">
   <table style="width:100%; vertical-align:top; text-align:left; border:1px solid #B0B0B0;"  cellpadding=4 cellspacing=0 border=0 rules="cols">
    <colgroup>
        <col width="32%">
        <col width="14%">
        <col width="14%">
        <col width="14%">
        <col width="14%">
        <col width="8%">
    </colgroup>
    <tr style="background-color: #E8E8E8; height:26px;font-weight:bold;">
     <td>{t}Name{/t}</td><td>{t}Phone{/t}</td><td>{t}Fax{/t}</td>
     <td>{t}Mobile{/t}</td><td>{t}Private{/t}</td><td>{t}Contact{/t}</td>
    </tr>
    {$search_result}
   </table>

   <table style="vertical-align:top; text-align:center; width:100%;" cellpadding=4 cellspacing=0 border=0>
    <tr>
     <td>{$range_selector}</td>
    </tr>
   </table>

   {if $show_info eq 1}
    {include file=$address_info}
   {/if}

  </td>
  <td style="vertical-align:top; width:270px;">
   <div class="contentboxh">
    <p class="contentboxh"><img src="{$infoimage}" align="right" alt="[i]">{t}Information{/t}</p>
   </div>
   <div class="contentboxb">
    <p class="contentboxb">
     {t}The telephone list plugin provides list and search facilities for the people in your site. You may want to specify the asterisk [*] like in 'Go*us' to find 'Gonicus'. Use the filters below to narrow down your search.{/t}
    </p>
   </div>
   <br>
   <div class="contentboxh">
    <p class="contentboxh"><img src="{$actionimage}" align="right" alt="[a]">{t}Actions{/t}</p>
   </div>
   <div class="contentboxb">
    <p class="contentboxb">
     <img alt="" src="{$add_image}" align=middle>&nbsp;
     <a href="main.php{$plug}&amp;global=add">{t}Add entry{/t}</a><br>
     {if $internal eq 0}
     <img alt="" src="{$edit_image}" align=middle>&nbsp;
     <a href="main.php{$plug}&amp;global=edit">{t}Edit entry{/t}</a><br>
     <img alt="" src="{$delete_image}" align=middle>&nbsp;
     <a href="main.php{$plug}&amp;global=remove">{t}Remove entry{/t}</a><br>
     {/if}
    </p>
   </div>
   <br>
   <div class="contentboxh">
    <p class="contentboxh"><img src="{$launchimage}" align="right" alt="[F]">{t}Filters{/t}</p>
   </div>
   <div class="contentboxb">
    <table style="width:100%;border-top:1px solid #B0B0B0;">
     {$alphabet}
    </table>
    
<table  style="width:100%;border-top:1px solid #B0B0B0;">
	<tr>
		<td>
<input type=checkbox name="organizational" value="1" {$organizational} onClick="mainform.submit()" title="{t}Select to see regular users{/t}">{t}Show organizational entries{/t}<br>
    <input type=checkbox name="global" value="1" {$global} onClick="mainform.submit()" title="{t}Select to see users in addressbook{/t}">{t}Show addressbook entries{/t}<br>
		</td>
	</tr>
</table>

<table style="width:100%;border-top:1px solid #B0B0B0;">
	<tr>
		<td>
			<img alt="" src="{$tree_image}" align=middle>
			&nbsp;{t}Display results for department{/t}
		</td>
    	<td>
			<select name="search_base" size=1 onChange="mainform.submit()" title="{t}Choose the department the search will be based on{/t}">
		      	{html_options options=$deplist selected=$depselect}
    		</select>
		</td>
	</tr>
</table>

<table style="width:100%;border-top:1px solid #B0B0B0;">
	<tr>
		<td width="50%"><img alt="" src="{$search_image}" align=middle>
			&nbsp;{t}Search for{/t}
		</td>
	    <td>
			<input type='text' name='search_for' maxlength='20' value='{$search_for}' title='{t}Search string{/t}' onChange="mainform.submit()">
		</td>
	</tr>
</table>

<table style="width:100%;border-top:1px solid #B0B0B0;">
	<tr>
		<td width="50%">
			<img alt="" src="{$search_image}" align=middle>&nbsp;{t}Match object{/t}
		</td>
	    <td>
	      	<select name="object_type" size=1 onChange="mainform.submit()" title="{t}Choose the object that will be searched in{/t}">
    	   		{html_options options=$objlist selected=$object_type}
	      	</select>
	    </td>
	</tr>
</table>
   {$apply}
   </div>
  </td>
</tr>
</table>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.search_for.focus();
  -->
</script>
