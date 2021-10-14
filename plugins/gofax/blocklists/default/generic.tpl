<div class="blocklist-wrapper">
  <div class="row">
    <h3>{t}Generic{/t}</h3>

    <div class="col s12 xl6">
      {render acl=$cnACL}
      <div class="input-field">
        <input type="text" name="cn" id="cn" size=25 maxlength=60 value="{$cn}" title="{t}Name of blacklist{/t}">
        <label for="cn">{t}List name{/t}{$must}</label>
      </div>
      {/render}

      <div class="input-field ldap-tree">
        {render acl=$baseACL}
          {$base}
        {/render}
        <label for="base">{t}Base{/t}{$must}</label>
      </div>
    </div>

    <div class="col s12 xl6">
      {render acl=$typeACL}
      <div class="input-field">
        <select size="1" id="type" name="type" title="{t}Select whether to filter incoming or outgoing calls{/t}">
          {html_options options=$types selected=$type}
        </select>
        <label for="type">{t}Type{/t}</label>
      </div>
      {/render}

      {render acl=$descriptionACL}
      <div class="input-field">
        <input type="text" name="description" id="description" size=25 maxlength=80 value="{$description}" title="{t}Descriptive text for this blacklist{/t}">
        <label for="description">{t}Description{/t}</label>
      </div>
      {/render}
    </div>
  </div>

  <hr class="divider">

  <div class="row">
    <h3>{t}Blocked numbers{/t}</h3>

    <div class="col s12 xl6">
      {render acl=$goFaxBlocklistACL}
      <div class="input-field">
        <select style="width:100%; height:200px;" name="numbers[]" size=15 multiple>
          {html_options values=$goFaxBlocklist output=$goFaxBlocklist}
        </select>
      </div>
      {/render}

      {render acl=$goFaxBlocklistACL}
      <div class="input-field add">
        <input type="text" id="number" name="number" size=25 maxlength=60 >
        <button class="btn-small" type="submit" name="add_number">{msgPool type=addButton}</button>&nbsp;
        <button class="btn-small" type="submit" name="delete_number">{msgPool type=delButton}</button>
      </div>
      {/render}
    </div>

    <div class="col s12 xl6">
      <div class="blocklist-info">
        <h3>{t}Information{/t}</h3>

        <p>{t}Numbers can also contain wild cards.{/t}</p>
      </div>
    </div>
  </div>
</div>

<input type="hidden" name="blocklist_posted" value="1">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  // First input field on page
	focus_field('n');
</script>
