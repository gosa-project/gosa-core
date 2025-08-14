<div class="list-head-wrapper">
  <h2>{$HEADLINE}&nbsp;{$SIZELIMIT}</h2>
  <input type="hidden" name="ignore">
  <div class="control-panel">
    <div class="navigation">
        {$ROOT}
        {$BACK}
        {$HOME}
        {$RELOAD}
    </div>
    <div class="ldap-tree valign-wrapper">{$BASE}</div>
    <div class="actions center-align">{$ACTIONS}</div>
    {$FILTER}
  </div>
</div>

<div class="list-content-wrapper">
  {$LIST}

  <input type="hidden" name="ignore">
</div>
