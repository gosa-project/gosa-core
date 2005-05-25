<div class="contentboxh">
 <p class="contentboxh"><img src="{$launchimage}" align="right" alt="[F]">{t}Filter{/t}</p>
</div>
<div class="contentboxb">
 <p class="contentboxb" style="border-top:1px solid #B0B0B0; padding:0px;">

 <table width="100%">
  <tr>
   <td width=33%>
    <table>
     <tr>
      <td><img alt="" align="middle" border=0 src="{$server_image}">&nbsp;{t}Show hosts{/t}</td>
      <td>
       <select name="host" size=1 onChange="mainform.submit()">
        {html_options options=$hostlist selected=$host}
       </select>
      </td>
     </tr>
     <tr>
      <td><img alt="" align="middle" border=0 src="{$log_image}">&nbsp;{t}Log level{/t}</td>
      <td>
       <select name="log_level" size=1 onChange="mainform.submit()">
        {html_options options=$loglevellist selected=$log_level}
       </select>
      </td>
     </tr>
    </table>
   </td>
   
   <td style="border-left:1px solid #A0A0A0">
    &nbsp;
   </td>
   
   <td width=33%>
    <table>
     <tr>
      <td><img alt="" align="middle" border=0 src="{$time_image}">&nbsp;{t}Time interval{/t}</td>
      <td>
       <select name="time" size=1 onChange="mainform.submit()">
        {html_options options=$tilist selected=$time}
       </select>
      </td>
     </tr>
     <tr>
      <td><img alt="" align="middle" border=0 src="{$search_image}">&nbsp;{t}Search for{/t}</td>
      <td><input name="regex" size=25 maxlength=60 value="{$regex}" title="{t}Enter string to search for{/t}"></td>
     </tr>
    </table>
   </td>

   <td style="border-left:1px solid #A0A0A0">
    &nbsp;
   </td>
   
   <td>
    <table width="100%">
     <tr>
      <td><img alt="" align="middle" border=0 src="{$ruleset_image}">&nbsp;{t}Ruleset{/t}</td>
      <td width="100%">
       <input name="edit_rules" type="submit" value="{t}Edit{/t}" disabled>
      </td>
     </tr>
     <tr>
      <td colspan=2 style="text-align:right;">
       <input name="search" type="submit" value="{t}Search{/t}">
      </td>
     </tr>
    </table>
   </td>

  </tr>
 </table>
</p>
</div>

<br>

{if $search_result ne ""}
 <table style="width:100%; vertical-align:top; text-align:left; border:1px solid #B0B0B0;" cellpadding=2 cellspacing=1 rules="cols">
  <tr style="background-color: #E8E8E8; height:26px; font-weight:bold">
   <td><a href=main.php?plug={$plug}&amp;sort=0>{t}Level{/t} {$mode0}</a></td>
   <td><a href=main.php?plug={$plug}&amp;sort=1>{t}Hostname{/t} {$mode1}</a></td>
   <td width=15%><a href=main.php?plug={$plug}&amp;sort=2>{t}Date{/t} {$mode2}</a></td>
   <td><a href=main.php?plug={$plug}&amp;sort=3>{t}Message{/t} {$mode3}</a></td>
  </tr>
  {$search_result}
 </table>

 <table style="width:100%; vertical-align:top; text-align:center;" cellpadding=4 cellspacing=0 border=0>
  <tr>
   <td>{$range_selector}</td>
  </tr>
 </table>

{else}
  <b>{t}Search returned no results...{/t}</b>
{/if}

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.search_for.focus();
  -->
</script>
