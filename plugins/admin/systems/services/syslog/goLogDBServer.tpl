<h2><img class="center" alt="" align="middle" src="images/rightarrow.png" /> {t}Logging database information{/t}</h2>
<table summary="">
    <tr>
     <td>{t}Logging DB user{/t}{$must}</td>
     <td>
{render acl=$goLogAdminACL}
	<input name="goLogAdmin" id="goLogAdmin" size=30 maxlength=60 value="{$goLogAdmin}">
{/render}
     </td>
    </tr>
    <tr>
     <td>{t}Password{/t}{$must}</td>
     <td>
{render acl=$goLogPasswordACL}
 	<input type=password name="goLogPassword" id="goLogPassword" size=30 maxlength=60 value="{$goLogPassword}">
{/render}
     </td>
    </tr>
   </table>

<p class='seperator'>&nbsp;</p>
<div style="width:100%; text-align:right;padding-top:10px;padding-bottom:3px;">
    <input type='submit' name='SaveService' value='{t}Save{/t}'>
    &nbsp;
    <input type='submit' name='CancelService' value='{t}Cancel{/t}'>
</div>
<input type="hidden" name="goLogDBServerPosted" value="1">
