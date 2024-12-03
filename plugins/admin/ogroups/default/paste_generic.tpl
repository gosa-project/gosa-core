<h3>{t}Objectgroup settings{/t}</h3>

<div class="row content">
	<div class="col s6 xl5">
		<div class="input-field">
			<input type='text' id='cn' name='cn' value='{$cn}' title='{t}Please enter the new object group name{/t}'> 
			<label for="cn">{t}Group name{/t}{$must}</label>
		</div>
		<p>
			<label>
				<input type='checkbox' class="filled-in" name="copyMembers" id="copyMembers" value="1" 
					{if $copyMembers} checked {/if} />
				<span>{t}Copy members{/t}</span>
			</label>
		</p>
		{t}Warning: systems can only inherit from a single object group!{/t}
	</div>
</div>

<script language="JavaScript" type="text/javascript">
	focus_field('cn');
</script>
