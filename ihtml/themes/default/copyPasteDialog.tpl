<h3>{t}Copy & paste wizard{/t}</h3>

<div class="row content">
	<b>{$message}</b>
	<br>
	<br>
	{if $Complete == false}
	{t}Some values need to be unique in the complete directory while some combinations make no sense. Please edit the
	values below to fulfill the policies.{/t}
	<br>
	{t}Remember that some properties like taken snapshots will not be copied!{/t}&nbsp;
	{t}Or if you copy or cut an entry within GOsa and delete the source object, you may get errors while pasting this
	object again!{/t}

	<hr>
	<br>
	{$AttributesToFix}
	{if $SubDialog == false}
	<br>

	<div class="card-action">
		<button class="btn-small primary" type="submit" name="PerformCopyPaste" id="PerformCopyPaste">
			{t}Save{/t}
		</button>
		{if $type == "modified"}
		<button class="btn-small primary" type='submit' name='abort_current_cut-copy_operation'
			id="abort_current_cut-copy_operation">
			{t}Cancel{/t}
		</button>
		{/if}
		<button class="btn-small primary" type='submit' name='abort_all_cut-copy_operations'
			id="abort_all_cut-copy_operations">
			{t}Cancel all{/t}
		</button>
	</div>
	<div style='text-align:right;width:100%;'>
	</div>
	{/if}
	{else}
	<hr>
	<h3>{t}Operation complete{/t}</h3>
	<div class="card-action">
		<button class="btn-small primary" type='submit' name='Back' id="Back">{t}Finish{/t}</button>
	</div>
	{/if}
</div>