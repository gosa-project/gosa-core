<div class="contentboxh">
 <p class="contentboxh"><img src="{$launchimage}" align="right" alt="[F]">{t}Filter{/t}</p>
</div>
<div class="contentboxb">

 <table summary="" width="100%" class="contentboxb" style="border-top:1px solid #B0B0B0; padding:0px;">
  <tr>
   <td width="33%">
    <table summary="">
     <tr>
      <td><img class="center" alt="" align="middle" border=0 src="{$server_image}">&nbsp;<LABEL FOR="host">{t}Show hosts{/t}</LABEL></td>
      <td>
       <select id="host" name="host" size=1 onChange="mainform.submit()">
		       {html_options options=$hostlist selected=$host}
			   <option disabled>&nbsp; </option>
       </select>
      </td>
     </tr>
     <tr>
      <td><img class="center" alt="" align="middle" border=0 src="{$log_image}">&nbsp;<LABEL for="log_level">{t}Log level{/t}</label></td>
      <td>
       <select id="log_level" name="log_level" size=1 onChange="mainform.submit()">
        	{html_options options=$loglevellist selected=$log_level}
            <option disabled>&nbsp;</option>
       </select>
      </td>
     </tr>
    </table>
   </td>
   
   <td style="border-left:1px solid #A0A0A0">
    &nbsp;
   </td>
   
   <td width="33%">
    <table summary="">
     <tr>
      <td><img class="center" alt="" align="middle" border=0 src="{$time_image}">&nbsp;<LABEL for="time" >{t}Time interval{/t}</LABEL></td>
      <td>
       <select id="time" name="time" size=1 onChange="mainform.submit()">
        	{html_options options=$tilist selected=$time}
            <option disabled>&nbsp;</option>
       </select>
      </td>
     </tr>
     <tr>
      <td><img class="center" alt="" align="middle" border=0 src="{$search_image}">&nbsp;<LABEL for="regex">{t}Search for{/t}</LABEL></td>
      <td><input id="regex" name="regex" size=25 maxlength=60 value="{$regex}" title="{t}Enter string to search for{/t}"></td>
     </tr>
    </table>
   </td>

   <td style="border-left:1px solid #A0A0A0">
    &nbsp;
   </td>
   <td>
    <table summary="" width="100%">
  <!-- 
     <tr>
      <td><img class="center" alt="" align="middle" border=0 src="{$ruleset_image}">&nbsp;<LABEL for="edit_rules">{t}Ruleset{/t}</LABEL></td>
      <td width="100%">
       <input id="edit_rules" name="edit_rules" type="submit" value="{t}Edit{/t}" disabled>
      </td>
     </tr>
-->
     <tr>
      <td colspan=2 style="text-align:right;">
       <input name="search" type="submit" value="{t}Search{/t}">
      </td>
     </tr>
    </table>
   </td>
  </tr>
 </table>
</div>

<br>

{if $search_result ne ""}
 <table summary="" style="width:100%; vertical-align:top; text-align:left; border:1px solid #B0B0B0;" cellpadding=2 cellspacing=1 rules="cols">
  <tr style="background-color: #E8E8E8; height:26px; font-weight:bold">
   <td><a href=main.php{$plug}&amp;sort=0>{t}Level{/t} {$mode0}</a></td>
   <td><a href=main.php{$plug}&amp;sort=1>{t}Hostname{/t} {$mode1}</a></td>
   <td width=15%><a href=main.php{$plug}&amp;sort=2>{t}Date{/t} {$mode2}</a></td>
   <td><a href=main.php{$plug}&amp;sort=3>{t}Message{/t} {$mode3}</a></td>
  </tr>
  {$search_result}
 </table>

 <table summary="" style="width:100%; vertical-align:top; text-align:center;" cellpadding=4 cellspacing=0 border=0>
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
	focus_field('search_for');
  -->
</script>
