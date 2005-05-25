<div style="font-size:20px;">
  <img alt="" src="images/encrypted.png" align=top>&nbsp;{t}Locking conflict detected{/t}
</div>

<p>
  <b>{t}Warning{/t}:</b> {$message}
</p>
<p>
  {t}If this is lock detection is false, the other person has obviously closed the webbrowser during the edit operation. You may delete the lockfile in this case by pressing the <i>Remove</i> button.{/t}
</p>

<p class="plugbottom">
  <input type=submit name="delete_lock" value="{t}Remove{/t}">
  &nbsp;
  <input type=submit name="cancel_lock" value="{t}Cancel{/t}">
</p>
      
<input type="hidden" name="dn" value="{$dn}">

