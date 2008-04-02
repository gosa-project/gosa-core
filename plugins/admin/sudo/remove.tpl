<div style="font-size:18px;">
  <img alt="" src="images/button_cancel.png" align=top>&nbsp;{t}Warning{/t}
</div>
<p>
  {$info}
</p>

<p>
  {t}So - if you're sure - press 'Delete' to continue or 'Cancel' to abort.{/t}
</p>

<p class="plugbottom">

  {if $multiple}
  <input type=submit name="delete_multiple_sudos_confirmed" value="{msgPool type=delButton}">
  &nbsp;
  <input type=submit name="delete_multiple_sudo_cancel" value="{msgPool type=cancelButton}">
  {else}
  <input type=submit name="delete_sudo_confirmed" value="{msgPool type=delButton}">
  &nbsp;
  <input type=submit name="delete_cancel" value="{msgPool type=cancelButton}">
  {/if}


</p>

