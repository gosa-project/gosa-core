<h2>{$HEADLINE}&nbsp;{$SIZELIMIT}</h2>
<input type="hidden" name="ignore">
<div class="row control-panel">
  <div class="col s2 navigation">
      {$ROOT}
      {$BACK}
      {$HOME}
      <div class="refresh">{$RELOAD}</div>
  </div>
  <div class="col s4 ldap-tree valign-wrapper">{$BASE}</div>
  <div class="col s2 actions center-align">{$ACTIONS}</div>
  {$FILTER}
</div>
<hr>

{$LIST}
<input type="hidden" name="ignore">
