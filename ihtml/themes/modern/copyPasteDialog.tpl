<h3>{t}Copy & paste wizard{/t}</h3>

<b>{$message}</b>
<br>
<br>
{if $Complete == false}
	{t}Some values need to be unique in the complete directory while some combinations make no sense. GOsa shows the relevant attributes. Please maintain the values below to fullfill the policies.{/t}
	<br>
{t}Remember that some properties like taken snapshots will not be copied!{/t}&nbsp;
{t}Or if you copy or cut an entry within GOsa and delete the source object, you may get errors while pasting this object again!{/t}

	<hr>
	<br>
	{$AttributesToFix}
	{if $SubDialog == false}
	<br>

	<div style='text-align:right;width:100%;'>
		<button type='submit' name='PerformCopyPaste'>{t}Save{/t}</button>
	    {if $type == "modified"}
		    <button type='submit' name='abort_current_cut-copy_operation'>{t}Cancel{/t}</button>
    	{/if}
		<button type='submit' name='abort_all_cut-copy_operations'>{t}Cancel all{/t}</button>
	</div>
	{/if}
{else}
	<hr>
	<h3>{t}Operation complete{/t}</h3>
	<div style='text-align:right;width:100%;'>
		<button type='submit' name='Back'>{t}Finish{/t}</button>
	</div>
{/if}
