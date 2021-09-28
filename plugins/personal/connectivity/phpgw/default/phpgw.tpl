<div class="phpgw-wrapper">
	{render acl=$phpgwAccountACL checkbox=$multiple_support checked=$use_phpgw}
	<label class="card-title">
		<input type="checkbox" name="phpgw" value="B" {$phpgwState}>
        <span style="overflow: hidden;"><h2>{t}PHPGroupware account{/t}</h2></span>
    </label>
	{/render}
</div>
