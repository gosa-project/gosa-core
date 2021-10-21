<div class="intranet-wrapper">
    <div class="row">
        <div class="col s12">
            {render acl=$gosaIntranetACL checkbox=$multiple_support checked=$use_intranet}
                <label>
                    <p>
                        <input type="checkbox" name="intranet" value="B" {$intranetState}>
                        <span>{t}Intra net account{/t}</span>
                    </p>
                </label>
            {/render}
		</div>
	</div>
</div>
