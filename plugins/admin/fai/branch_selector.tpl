<br> 
 <div class="contentboxh" style="border-bottom:1px solid #B0B0B0;">
    <p class="contentboxh"><img src="{$branchimage}" align="right" alt="[F]">{t}Branches{/t}</p>
   </div>
   <div class="contentboxb">
        <table summary="" style="width:100%;border-bottom:1px solid #B0B0B0;">
     <tr>
      <td>
    {t}Current release{/t}&nbsp;
    <select name="selectedBranch" onChange="document.mainform.submit();">
        {html_options options=$branches selected=$selectedBranch}
    </select>
      </td>
     </tr>
        </table>
        <table summary="" style="width:100%;">
     <tr>
      <td>
	{if $allow_create}
		<input type="image" align="middle" value="branch" name="branch_branch" src="images/branch_small.png">
		&nbsp;
		{t}Create new branch{/t}
		<br>
		<input type="image" align="middle" value="freeze" name="freeze_branch" src="images/freeze.png">
	{else}
		<img src="images/branch_small_grey.png" alt='{t}Branch{/t}' class='center'>
		&nbsp;
		{t}Create new branch{/t}
		<br>
		<img src="images/freeze_grey.png" alt='{t}Freeze{/t}' class='center'>
	{/if}
	&nbsp;
	{t}Create new locked branch{/t}

    {if $selectedBranch != "main" && $allow_remove}
    <br>
	    <input type="image" align="middle" value="delete" name="remove_branch" src="images/edittrash.png">
    &nbsp;
    {t}Delete current release{/t}
    {/if}
      </td>
     </tr>
        </table>
   </div>

