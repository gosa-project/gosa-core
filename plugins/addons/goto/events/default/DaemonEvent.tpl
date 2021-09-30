<div class="daemonevent-wrapper">
	<div class="row">
		{if $is_new}
		<div class="col s12 xl6">
			<h3>{t}Schedule{/t}</h3>
			
			{$timestamp}
		</div>

		<div class="col s12 xl6">
			<h3>{t}System list{/t}</h3>
            
			{$target_list}
		</div>

		{else}
		<div class="col s12 xl6">
			<h3>{t}Generic settings{/t}</h3>

			<div class="input-field">
				<input type="text" name="dataID" id="dataID" value="{$data.ID}">
				<label for="dataID">{t}ID{/t}</label>
			</div>

			<div class="input-field">
				<input type="text" name="dataStatus" id="dataStatus" value="{$data.STATUS}">
				<label for="dataStatus">{t}Status{/t}</label>
			</div>

			<div class="input-field">
				<input type="text" name="dataResult" id="dataResult" value="{$data.RESULT}">
				<label for="dataResult">{t}Result{/t}</label>
			</div>

			<div class="input-field">
				<input type="text" name="dataTarget" id="dataTarget" value="{$data.MACADDRESS}">
				<label for="dataTarget">{t}Target{/t}</label>
			</div>
		</div>

		<div class="col s12 xl6">
			<h3>{t}Time stamp{/t}</h3>

			{$timestamp}
		</div>
{/if}
	</div>
</div>