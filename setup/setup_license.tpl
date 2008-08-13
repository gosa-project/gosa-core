<div>
	<div class='default' style='margin:10px; border:1px solid #A0A0A0'>
		<div style='height:450px;padding:5px;overflow:auto; '>
			{$License}
		</div>
	</div>	
	<div style='width:95%; text-align: right'>
		<input {if $accepted} checked {/if} type='checkbox' name='accepted' class="center">{t}I have read the license and accept it{/t}
	</div>
</div>
<input type='hidden' name='step_license' value='1'>
