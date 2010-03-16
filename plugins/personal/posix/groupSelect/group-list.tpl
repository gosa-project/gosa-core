<table summary="{t}Group selection{/t}"
  style="width:100%;height:100%" cellpadding=2>
 <tr>
  <td style="width:100%;">
   <div class="contentboxh">
    <p class="contentboxh">&nbsp;{$HEADLINE}&nbsp;{$SIZELIMIT}</p>
   </div>
   
   <div class="contentboxb">
    <div style='background:white;padding:3px;'>
     <table><tr>
       <td>{$ROOT}&nbsp;</td><td>{$BACK}&nbsp;</td><td>{$HOME}&nbsp;</td><td>{$RELOAD}&nbsp;</td><td>{$SEPARATOR}&nbsp;</td><td>{t}Base{/t} {$BASE}&nbsp;</td><td>{$SEPARATOR}&nbsp;</td><td>{image path="images/rocket.png"}
       </td><td> {$ACTIONS}</td>
     </tr></table>
    </div>
   </div>
   
   <div style='height:4px;'>
   </div>
   {$LIST}
  </td>
  <td style='min-width:250px'>
   {$FILTER}
  </td>
 </tr>
</table>

<input type="hidden" value="1" name="PosixGroupDialogPosted">

<br>
<div class="plugin-actions">
<button type='submit' name='add_groups_finish'>{msgPool type=addButton}</button>
<button type='submit' name='add_groups_cancel'>{msgPool type=cancelButton}</button>
</div>


<input type="hidden" name="ignore">
