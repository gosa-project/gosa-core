<div class="intranet-wrapper">
    {render acl=$gosaIntranetACL checkbox=$multiple_support checked=$use_intranet}
    <label class="card-title">
        <input type="checkbox" name="intranet" value="B" {$intranetState}>
        <span style="overflow: hidden;"><h2>{t}Intra net account{/t}</h2></span>
    </label>
    {/render}
</div>