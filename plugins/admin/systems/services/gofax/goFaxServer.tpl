<h2><img class="center" alt="" align="middle" src="images/rightarrow.png" /> {t}FAX database information{/t}</h2>
 <table summary="">
    <tr>
     <td>{t}FAX DB user{/t}{$must}</td>
     <td>
{render acl=$goFaxAdminACL}
	<input name="goFaxAdmin" size=30 maxlength=60 id="goFaxAdmin" value="{$goFaxAdmin}" >
{/render}
     </td>
    </tr>
    <tr>
     <td>{t}Password{/t}{$must}</td>
     <td>
{render acl=$goFaxPasswordACL}
	<input type=password name="goFaxPassword" id="goFaxPassword" size=30 maxlength=60 value="{$goFaxPassword}" >
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
<input type="hidden" name="goFaxServerPosted" value="1">
