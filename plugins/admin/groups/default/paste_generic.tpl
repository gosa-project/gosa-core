<h3>{t}Group settings{/t}</h3>

<div class="row content">
	<div class="col s6 xl5">
		<div class="input-field">
			<input type='text' id='cn' name='cn' value='{$cn}' title='{t}POSIX name of the group{/t}'>
			<label for="cn">{t}Group name{/t}</label>
		</div>
		<p>
			<label>
				<input type="checkbox" class="filled-in" name="force_gid" id="force_gid" value="1" {$used}
					title="{t}Normally IDs are auto-generated, select to specify manually{/t}"
					onclick='changeState("gidNumber")' />
				<span>{t}Force GID{/t}</span>
			</label>
			<input type='text' name='gidNumber' id='gidNumber' {$dis} value='{$gidNumber}'
				title='{t}Forced ID number{/t}'>
		</p>
		<div class="input-field">
		</div>
	</div>
</div>

<script language="JavaScript" type="text/javascript">
	focus_field('cn');
</script>