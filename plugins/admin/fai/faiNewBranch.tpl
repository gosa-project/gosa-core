{if $iframe == false}

<br>
<p class="seperator">
{t}You are now going to create a branch or locked branch. This action clones the existing release to another release name. Branches can be modified as usual. FAI classes of locked branches cannot be modified, just additional classes are possible. Branch names should be alphanumeric, excluding the reserved names 'scripts', 'hooks', 'partitions', 'variables', 'templates' and 'fai'.{/t}
<br>
<br>
</p>
<p class="seperator">
<br>
<b>{t}Please enter a name for the branch{/t}</b> &nbsp;
<input type="text" id="BranchName" name="BranchName" value="{$BranchName}" style="width:120px;">
<br>
<br>
</p>

<div align="right">
	<p>
		<input type="submit" value="{t}Apply{/t}" 	name="UseBranchName">		
		<input type="submit" value="{t}Cancel{/t}" 	name="CancelBranchName">		
	</p>
</div>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
	<!--
	document.mainform.BranchName.select();
	document.mainform.BranchName.focus();
	-->
</script>
{else}

<h2>{t}Processing requested operation, the status of this operation will be displayed in an iframe below.{/t}</h2>

<div>
<iframe src="?plug={$plugID}&PerformBranch=1" width="100%" name="status" style="height:450px;background-color:#FFFFFF;">

	<p>{t}Your browser doesn't support iframes, please use this link to perform the requested operation.{/t}
		<br>
		<a href='?plug={$plugID}&PerformBranch=1' target="_blank" title="{t}Perform requested operation.{/t}"><b>{t}Initiate operation{/t}</b></a>
	</p>	
</iframe>
</div>
<p class="seperator">&nbsp;</p>
<div style="text-align: right;width:99%;">
	<p>
		<input type="submit" name="CancelBranchName" value="{t}Back{/t}">
	</p>
</div>
{/if}
