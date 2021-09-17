<div id="mainlist">

  <div class="mainlist-header">
   <p>{$HEADLINE}&nbsp;{$SIZELIMIT}</p>
   <div class="mainlist-nav">
    <table summary="{$HEADLINE}">
     <tr>
      <td>{$ROOT}</td>
      <td>{$BACK}</td>
      <td>{$HOME}</td>
      <td>{$RELOAD}</td>
      <td class="left-border">{t}Base{/t} {$BASE}</td>
      <td class="left-border">{$ACTIONS}</td>
      <td class="left-border">{$FILTER}</td>
     </tr>
    </table>
   </div>
  </div>

  {$LIST}
</div>
<input type="hidden" value="1" name="PosixGroupDialogPosted">

<div class="plugin-actions">
  <button type='submit' name='mailAddressSelect_save'>{msgPool type=addButton}</button>
  <button type='submit' name='mailAddressSelect_cancel'>{msgPool type=cancelButton}</button>
</div>

