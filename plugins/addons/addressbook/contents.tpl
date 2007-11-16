<table summary="" style="table-layout:fixed; width:100%; vertical-align:top; text-align:left; table-layout:auto;" cellpadding="4" border="0">
<tr>
  <td style="vertical-align:top;">
   <table summary="" style="border:1px solid #B0B0B0; width:100%; vertical-align:top; text-align:left;"  cellpadding="4" cellspacing="0">
    <colgroup>
        <col width="32%">
        <col width="14%">
        <col width="14%">
        <col width="14%">
        <col width="14%">
        <col width="8%">
    </colgroup>
    <tr style="background-color: #E8E8E8; height:26px;font-weight:bold;">
     <td class="tbhead">{t}Name{/t}</td><td class="tbhead">{t}Phone{/t}</td><td class="tbhead">{t}Fax{/t}</td>
     <td class="tbhead">{t}Mobile{/t}</td><td class="tbhead">{t}Private{/t}</td><td class="tbrhead">{t}Contact{/t}</td>
    </tr>
    {$search_result}
   </table>

   <table summary="" style="vertical-align:top; text-align:center; width:100%;" cellpadding="4" cellspacing="0" border="0">
    <tr>
     <td>{$range_selector}</td>
    </tr>
   </table>

   {if $show_info eq 1}
    {include file=$address_info}
   {/if}

  </td>
  <td style="vertical-align:top; width:270px;">
   <div class="contentboxh" style="border-bottom:1px solid #B0B0B0;">
    <p class="contentboxh"><img src="{$infoimage}" align="right" alt="[i]">{t}Information{/t}</p>
   </div>
   <div class="contentboxb">
    <p class="contentboxb">
     {t}The telephone list plugin provides list and search facilities for the people in your site. You may want to specify the asterisk [*] like in 'Go*us' to find 'Gonicus'. Use the filters below to narrow down your search.{/t}
    </p>
   </div>
   <br>
   <div class="contentboxh">
    <p class="contentboxh" style="vertical-align:middle;"><img src="{$actionimage}" style="vertical-align:middle;" align="right" alt="[a]">{t}Actions{/t}</p>
   </div>
   <div class="contentboxb">
    <p class="contentboxb" style="vertical-align:middle;">
     <img class="center" alt="" src="{$add_image}">&nbsp;
     <a href="main.php{$plug}&amp;global=add" style="text-align:center;vertical-align:middle;">{t}Add entry{/t}</a><br>
     {if $internal eq 0}
     <img alt="" src="{$edit_image}" class="center">&nbsp;
     <a href="main.php{$plug}&amp;global=edit">{t}Edit entry{/t}</a><br>
     <img alt="" src="{$delete_image}" class="center">&nbsp;
     <a href="main.php{$plug}&amp;global=remove" style="vertical-align:middle;">{t}Remove entry{/t}</a><br>
     {/if}
    </p>
   </div>
   <br>
   <div class="contentboxh">
    <p class="contentboxh"><img src="{$launchimage}" align="right" alt="[F]">{t}Filters{/t}</p>
   </div>
   <div class="contentboxb">
    <table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
     {$alphabet}
    </table>
    
<table summary=""  style="width:100%;border-top:1px solid #B0B0B0;">
	<tr>
		<td>
<input type="checkbox" name="organizational" value="1" {$organizational} onClick="mainform.submit()" title="{t}Select to see regular users{/t}">{t}Show organizational entries{/t}<br>
    <input type="checkbox" name="global" value="1" {$global} onClick="mainform.submit()" title="{t}Select to see users in addressbook{/t}">{t}Show addressbook entries{/t}<br>
		</td>
	</tr>
</table>

<table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
	<tr>
		<td>
			<label for="search_base"><img class="center" alt="" src="{$tree_image}" align=middle title='{t}Display results for department{/t}'></label>
		</td>
    	<td>
			<select name="search_base" style='width:220px' onChange="mainform.submit()" title="{t}Choose the department the search will be based on{/t}">
		      	{html_options options=$deplist selected=$depselect}
				<option disabled>&nbsp;</option>
    		</select>
		</td>
	</tr>
</table>
<table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
	<tr>
		<td>
			<label for="object_type"><img class="center" alt="" src="{$obj_image}" align=middle title='{t}Match object{/t}'></label>
		</td>
	    <td>
			<select id="object_type" style='width:220px' name="object_type" onChange="mainform.submit()" title="{t}Choose the object that will be searched in{/t}">
    	   		{html_options options=$objlist selected=$object_type}
				<option disabled>&nbsp;</option>
	      	</select>
	    </td>
	</tr>
</table>
<table summary="" style="width:100%;border-top:1px solid #B0B0B0;">
	<tr>
		<td><label for="search_for"><img class="center" alt="" src="{$search_image}" style="text-align:center;" title='{t}Search for{/t}'></label>
		</td>
	    <td>
			<input id="search_for" style='width:99%' type='text' name='search_for' maxlength='20' value='{$search_for}' title='{t}Search string{/t}' onChange="mainform.submit()">
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
	focus_field('search_for');
  -->
</script>
