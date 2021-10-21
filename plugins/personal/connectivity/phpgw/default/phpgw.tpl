<div class="phpgw-wrapper">
    <div class="row">
        <div class="col s12">
			{render acl=$phpgwAccountACL checkbox=$multiple_support checked=$use_phpgw}
				<label>
					<p>
						<input type="checkbox" name="phpgw" value="B" {$phpgwState}>
						<span>{t}PHPGroupware account{/t}</span>
					</p>
				</label>
			{/render}
		</div>
	</div>
</div>
