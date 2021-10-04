<div class="netgroup-wrapper">
  <h2>{t}Member of the following NIS Netgroups{/t}</h2>

  <div class="row">
		<div class="col s12">
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
</div>
