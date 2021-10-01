<div id="mainlist">
 <div class="mainlist-header">
  <p>{t}Fax reports{/t}</p>

  <div class="mainlist-nav">
   <table summary="{t}Filter{/t}" style="width: 100%;"
      id="t_scrolltable" cellpadding="0" cellspacing="0">
    <tr>
     <td>{t}Date{/t}
      <select size="1" name="month" onChange="mainform.submit()">
       {html_options options=$months selected=$month_select}
      </select>
      <select size="1" name="year" onChange="mainform.submit()">
       {html_options values=$years output=$years selected=$year_select}
      </select>
     </td>
     <td>{t}Search for{/t}
      <input type='text' name="search_for" size=25 maxlength=60
        value="{$search_for}" title="{t}Enter user name to search for{/t}"
        onChange="mainform.submit()">
     </td>
     <td>
      <button type='submit' name='search'>{t}Search{/t}</button>
     </td>
    </tr>
   </table>

  </div>
 </div>
</div>

{if $search_result ne ""}
 <div class="listContainer" id="d_scrollbody" style="min-height: 475px; height: 444px;">
  <table summary="{t}Phone reports{/t}" style="width:100%;" cellpadding="0" cellspacing="0">
   <thead class="fixedListHeader listHeaderFormat">
    <tr>
     <td class='listheader'><a href="main.php{$plug}&amp;sort=0">{t}User{/t}</a> {$mode0}</td>
     <td class='listheader'><a href="main.php{$plug}&amp;sort=1">{t}Date{/t}</a> {$mode1}</td>
     <td class='listheader'><a href="main.php{$plug}&amp;sort=2">{t}Status{/t}</a> {$mode2}</td>
     <td class='listheader'><a href="main.php{$plug}&amp;sort=3">{t}Sender{/t}</a> {$mode3}</td>
     <td class='listheader'><a href="main.php{$plug}&amp;sort=4">{t}Receiver{/t}</a> {$mode4}</td>
     <td class='listheader'><a href="main.php{$plug}&amp;sort=5">{t}# pages{/t}</a> {$mode5}</td>
     <td class='listheader' style='border-right: 0pt none;'>&nbsp;</td>
    </tr>
   </thead>
   <tbody class="listScrollContent listBodyFormat" id="t_nscrollbody">
    {$search_result}
    <tr>
     <td class="list0">&nbsp;</td>
     <td class="list0">&nbsp;</td>
     <td class="list0">&nbsp;</td>
     <td class="list0">&nbsp;</td>
     <td class="list0">&nbsp;</td>
     <td class="list0">&nbsp;</td>
     <td class="list0" style='border-right: 0pt none;'>&nbsp;</td>
    </tr>
   </tbody>
  </table>

  <table style='width:100%; text-align:center;' summary="{t}Page selector{/t}">
   <tr>
    <td>{$range_selector}</td>
   </tr>
  </table>
 </div>
 <hr>

{else}
  <b>{t}Search returned no results...{/t}</b>
{/if}


<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('search_for');
  -->
</script>
