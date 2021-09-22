<b><LABEL for="netgroups">{t}Member of the following NIS Netgroups{/t}</LABEL></b>
  {render acl=$netgroupsACL}
    {$memberList}
  {/render}
<br>
{render acl=$netgroupsACL}
  <input type=submit name="edit_membership" value="{msgPool type=addButton}">
{/render}
<input type="hidden" name="nisnetgroupedit" value="1">

