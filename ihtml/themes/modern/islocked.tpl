<div style="font-size:20px;">
  {image path="images/encrypted.png"}&nbsp;{t}Locking conflict detected{/t}

</div>

<p>
  <b>{t}Warning{/t}:</b> {$message}
</p>
<p>
  {t}If this lock detection is false, the other person has obviously closed the webbrowser during the edit operation. You may want to take over the lock by pressing the 'Edit anyway' button.{/t}
</p>

<p class="plugbottom">
  <button type="submit" name="delete_lock">{$action}</button>
	{if $allow_readonly}
  &nbsp;
  <button type="submit" name="open_readonly">{t}Read only{/t}</button>
	{/if}
  &nbsp;
  <button type="submit" name="cancel_lock">{t}Cancel{/t}</button>
</p>
      
<input type="hidden" name="dn" value="{$dn}">

