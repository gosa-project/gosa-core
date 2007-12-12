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
        <input class="center" type="image" name="branch_branch" src="images/branch_small.png">
        <a href="?plug={$plug_id}&act=branch_branch">{t}Create new branch{/t}</a>
        <br>
        <input class="center" type="image" name="freeze_branch" src="images/freeze.png">
		<a href="?plug={$plug_id}&act=freeze_branch">{t}Create new locked branch{/t}</a>
    {else}
        <img src="images/branch_small_grey.png" alt='{t}Branch{/t}' class='center'>
        {t}Create new branch{/t}
        <br>
        <img src="images/freeze_grey.png" alt='{t}Freeze{/t}' class='center'>
		{t}Create new locked branch{/t}
    {/if}

    {if $selectedBranch != "main" && $allow_remove}
    <br>
        <input class="center" type="image" name="remove_branch" src="images/edittrash.png">
	    <a href="?plug={$plug_id}&act=remove_branch">{t}Delete current release{/t}</a>
    {/if}
      </td>
     </tr>
        </table>
   </div>

