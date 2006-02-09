<br>
<p class="seperator">
{t}This is a new system which currently has no system type defined. Please choose a system type for this entry, the depending dialog will be shown if you press 'save'. If this system should be added to an objectGroup, specify the objectgroup name and this system will be added automatically.{/t}
<br>
<br>
</p>
<p class="seperator">
<br>
<b>{t}Please select a system type and if prefered select a object group too.{/t}</b>
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
      {t}Membership in following objectGroups{/t}&nbsp;
	  <select name="ObjectGroup" title="{t}Choose a object group{/t}" style="width:120px;">
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
	<input type="submit" name="SystemTypeChoosen" value="{t}Use{/t}">
	<input type="submit" name="SystemTypeAborted" value="{t}Cancel{/t}">
</p></div>
