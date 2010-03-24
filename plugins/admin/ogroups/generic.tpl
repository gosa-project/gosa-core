<table summary="{t}Object group{/t}" style="width:100%;">
 <tr>
  <td style="width:50%">
   <input type="hidden" name="ogroupedit" value="1">
   <table summary="{t}Generic settings{/t}">
    <tr>
     <td><LABEL for="cn">{t}Group name{/t}</LABEL>{$must}</td>
     <td>
{render acl=$cnACL}
       <input type='text' name="cn" id="cn" size=25 maxlength=60 value="{$cn}" title="{t}Name of the group{/t}">
{/render}
     </td>
    </tr>
    <tr>
     <td><LABEL for="description">{t}Description{/t}</LABEL></td>
     <td>
{render acl=$descriptionACL}
	<input type='text' id="description" name="description" size=40 maxlength=80 value="{$description}" title="{t}Descriptive text for this group{/t}">
{/render}
     </td>
    </tr>
    <tr>
     <td colspan=2>&nbsp;</td>
    </tr>
    <tr>
     <td><LABEL for="base">{t}Base{/t}</LABEL>{$must}</td>
     <td>
{render acl=$baseACL}
       {$base}
{/render}
     </td>
    </tr>
   </table>

	<hr>
	<h3>{t}System trust{/t}</h3>
    {t}Trust mode{/t}&nbsp;
    {render acl=$trustmodeACL}
        <select name="trustmode" id="trustmode" size=1
            onChange="changeSelectState('trustmode', 'wslist');
                      changeSelectState('trustmode', 'add_ws');
                      changeSelectState('trustmode', 'del_ws');">
          {html_options options=$trustmodes selected=$trustmode}
        </select><br>
    {/render}
    {render acl=$trustmodeACL}
       <select style="width:100%" id="wslist" name="workstation_list[]" size=8 multiple {$trusthide}>
        {html_options values=$workstations output=$workstations}
        {if $emptyArrAccess}
            <option disabled>&nbsp;</option>
        {/if}
       </select>
    {/render}
       <br>
    {render acl=$trustmodeACL}
       <button type='submit' name='add_ws' id="add_ws">{msgPool type=addButton}</button>&nbsp;

    {/render}
    {render acl=$trustmodeACL}
       <button type='submit' name='delete_ws' id="del_ws">{msgPool type=delButton}</button>

    {/render}


  </td>
  <td style='padding-left:10px;' class='left-border'>

   <b><LABEL for="members">{t}Member objects{/t}</LABEL></b>&nbsp;({$combinedObjects})
   <br>
{render acl=$memberACL}
   <select style="width:100%; height:450px;" id="members" name="members[]" size="15" multiple>
    {$members}
   </select>
{/render}
   <br>
{render acl=$memberACL}
   <button type='submit' name='edit_membership'>{msgPool type=addButton}</button>&nbsp;

{/render}
{render acl=$memberACL}
   <button type='submit' name='delete_membership'>{msgPool type=delButton}</button>

{/render}
  </td>
 </tr>
</table>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('cn');
  -->
</script>
