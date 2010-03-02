{image path="images/empty.png" action="no_action_posted" value="do_nothing"}


<table summary="" style="width:100%;height:100%; vertical-align:top; text-align:left;" cellpadding=2>
 <tr>
  <td style="vertical-align:top; width:100%;">
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
  <td style='vertical-align:top;min-width:250px'>
   {$FILTER}
  </td>
 </tr>
</table>


<p class="plugbottom">
<button type='submit' name='add_ws_finish'>{msgPool type=addButton}</button>

&nbsp;
<button type='submit' name='add_ws_cancel'>{msgPool type=cancelButton}</button>

</p>

<input type="hidden" name="ignore">
