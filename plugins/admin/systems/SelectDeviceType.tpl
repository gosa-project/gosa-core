<br>
<p class="seperator">
{t}This is a new system which currently has no system type defined. Please choose a system type for this entry and press the 'continue' button. If this system should be added to a group of system settings, specify the preferred object group below.{/t}
<br>
<br>
</p>
<p class="seperator">
<br>
<b>{t}Please select a system type and an optional bundle of predefined settings to be inherited.{/t}</b>
<br>
<br>
</p>
<br>
<table summary="" style='width:100%'>
 <tr>
  <td style='width:49%'>
   <table summary="">
    <tr>
     <td> 
      {t}System type{/t}&nbsp;
	  <select name="SystemType" title="{t}Choose a system type{/t}" style="width:120px;">
       {html_options values=$SystemTypeKeys output=$SystemTypes selected=$SystemType}
      </select>
     </td>
    </tr>
   </table>
  </td>
  <td>
   <table summary="">
    <tr>
     <td> 
      {t}Choose an object group as template{/t}&nbsp;
	  <select name="ObjectGroup" title="{t}Choose an object group{/t}" style="width:120px;">
       {html_options values=$ObjectGroupKeys output=$ObjectGroups selected=$ObjectGroup}
      </select>
     </td>
    </tr>
   </table>
  </td>
 </tr>
</table>
<p class="seperator">&nbsp;</p>
<div style="align: right;" align="right"><p>
	<input type="submit" name="SystemTypeChoosen" value="{t}Continue{/t}">
	<input type="submit" name="SystemTypeAborted" value="{t}Cancel{/t}">
</p></div>
