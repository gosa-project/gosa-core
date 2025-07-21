<hr>
<div>
    <span class="black-text">
        <h5>{$HEADLINE}&nbsp;{$SIZELIMIT}</h5>
    </span>
</div>
<hr>
<div class="row mgb-off valign-wrapper">
    <div class="col s2">
        <div class="row mgb-off">
            <div class="col s3">{$ROOT}</div>
            <div class="col s3">{$BACK}</div>
            <div class="col s3">{$HOME}</div>
            <div class="col s3">{$RELOAD}</div>
        </div>
    </div>
    <div class="col s4 valign-wrapper">{$BASE}</div>
    <div class="col s2 center-align">{$ACTIONS}</div>
    {$FILTER}
</div>
<hr>

{$LIST}

<div class="card-action">
    <button class="btn-small gonicus-color" type="submit" name="ok-save">{msgPool type=okButton}</button>
    <button class="btn-small gonicus-color" type="submit" name="cancel-abort">{msgPool type=cancelButton}</button>
</div>