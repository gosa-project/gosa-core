<div class="netgroup-wrapper">
  <div class="row">
    <h2>{t}Member of the following NIS Netgroups{/t}</h2>

    <hr class="divider">

    {render acl=$netgroupsACL}
    {$memberList}
    {/render}

    {render acl=$netgroupsACL}
    <div class="input-field">
      <button class="btn-small" type=submit name="edit_membership">{msgPool type=addButton}</button>
    </div>
    {/render}
  </div>
</div>
<input type="hidden" name="nisnetgroupedit" value="1">

