<div class="intranet-wrapper">
    <div class="row">
        <div class="col s12">
            {render acl=$gosaIntranetACL checkbox=$multiple_support checked=$use_intranet}
                <label>
                    <input type="checkbox" name="intranet" value="B" {$intranetState}>
                    <span style="overflow: hidden;">{t}Intra net account{/t}</span>
                </label>
            {/render}
		</div>
	</div>
</div>
