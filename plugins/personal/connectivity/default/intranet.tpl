<div class="intranet-wrapper">
    {render acl=$gosaIntranetACL checkbox=$multiple_support checked=$use_intranet}
    <label>
        <input type="checkbox" name="intranet" value="B" {$intranetState}>
        <span><h3>{t}Intra net account{/t}</h3></span>
    </label>
    {/render}
</div>