<br>
<p class="seperator">
{t}You are now going to create a branch or locked branch. This action clones the existing release to another release name. Branches can be modified as usual. FAI classes of locked branches cannot be modified, just additional classes are possible. Branch names should be alphanumeric, excluding the reserved names 'scripts', 'hooks', 'partitions', 'variables', 'templates' and 'fai'.{/t}
<br>
<br>
</p>
<p class="seperator">
<br>
<b>{t}Please enter a name for the branch{/t}</b> &nbsp;
<input type="text"	 name="BranchName" value="{$BranchName}" style="width:120px;">
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
	document.mainform.UseTextInputName.select();
	document.mainform.UseTextInputName.focus();
	-->
</script>





