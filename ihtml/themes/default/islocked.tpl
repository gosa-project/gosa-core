<div class='plugin'>
 <div class='inline-warning'>
  {image path="images/encrypted.png" align="top"}
  <div class='inline-warning-text'>{t}Locking conflict detected{/t}</div>
 </div>

 <hr>
 
 <p>
   {$message}
 </p>

 <p>
   {t}If this lock detection is false, the other person has obviously closed the webbrowser during the edit operation. You may want to take over the lock by pressing the 'Edit anyway' button.{/t}
 </p>

 <hr>
 
 <div class='plugin-actions'>
   <button type="submit" name="delete_lock">{$action}</button>
 	{if $allow_readonly}
   &nbsp;
   <button type="submit" name="open_readonly">{t}Read only{/t}</button>
 	{/if}
   &nbsp;
   <button type="submit" name="cancel_lock">{t}Cancel{/t}</button>
 </div>
       
 <input type="hidden" name="dn" value="{$dn}">
</div>
