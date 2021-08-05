<h2>{$HEADLINE}&nbsp;{$SIZELIMIT}</h2>
<input type="hidden" name="ignore">
<div class="control-panel">
  <div class="navigation">
      {$ROOT}
      {$BACK}
      {$HOME}
      <div class="refresh">{$RELOAD}</div>
  </div>
  <div class="ldap-tree valign-wrapper">{$BASE}</div>
  <div class="actions center-align">{$ACTIONS}</div>
  {$FILTER}
</div>
<hr>

<div>
    {$LIST}
</div>

<div class="card-action">
    <button class="btn-small gonicus-color" type="submit" name="ok-save">{msgPool type=okButton}</button>
    <button class="btn-small gonicus-color" type="submit" name="cancel-abort">{msgPool type=cancelButton}</button>
</div>
