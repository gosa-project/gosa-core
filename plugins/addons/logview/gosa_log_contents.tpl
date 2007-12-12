<div class="contentboxh">
 <p class="contentboxh"><img src="{$launchimage}" align="right" alt="[F]">{t}Filter{/t}</p>
</div>
<div class="contentboxb">

 <table summary="" width="100%" class="contentboxb" style="border-top:1px solid #B0B0B0; padding:0px;" border=0>
  <tr>
   <td width="33%">
    <table summary="">
     <tr>
      <td><img class="center" alt="" align="middle" border=0 src="{$server_image}">&nbsp;<LABEL FOR="server">{t}Show hosts{/t}</LABEL></td>
      <td>
       <select id="server" name="server" size=1 onChange="mainform.submit()">
		       {html_options options=$servers selected=$server}
			   <option disabled>&nbsp; </option>
       </select>
      </td>
     </tr>
     <tr>
      <td><img class="center" alt="" align="middle" border=0 src="{$log_image}">&nbsp;<LABEL for="action">{t}Action{/t}</label></td>
      <td>
       <select id="action" name="action" size=1 onChange="mainform.submit()">
        	{html_options options=$actions selected=$action}
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
        	{html_options options=$date_select selected=$time}
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
 <table style="width:100%;background-color: #B0B0B0;" cellspacing=1 cellpadding=2>
  <tr style="background-color: #E8E8E8; height:26px; font-weight:bold">
   <td><a href="main.php{$plug}&amp;sort=0&amp;direction={$direction}">{t}Action{/t} {$mode0}</a></td>
   <td><a href="main.php{$plug}&amp;sort=1&amp;direction={$direction}">{t}Date{/t} {$mode1}</a></td>
   <td><a href="main.php{$plug}&amp;sort=2&amp;direction={$direction}">{t}User{/t} {$mode2}</a></td>
   <td><a href="main.php{$plug}&amp;sort=3&amp;direction={$direction}">{t}Object type{/t} {$mode3}</a></td>
   <td><a href="main.php{$plug}&amp;sort=4&amp;direction={$direction}">{t}Object{/t} {$mode4}</a></td>
   <td><a href="main.php{$plug}&amp;sort=5&amp;direction={$direction}">{t}Message{/t} {$mode5}</a></td>
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
<input type='hidden' name='gosa_log_contents_posted' value='1'>
<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  focus_field('regex');
  -->
</script>
