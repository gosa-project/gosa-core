<div id="mainlist">

  <div class="mainlist-header">
   <p>{$HEADLINE}&nbsp;{$SIZELIMIT}</p>
   <div class="mainlist-nav">
    <table summary="{$HEADLINE}">
     <tr>
      <td>{$RELOAD}</td>
      <td class="left-border">{$ACTIONS}</td>
      <td class="left-border">{$FILTER}</td>
     </tr>
    </table>
   </div>
  </div>
  {$LIST}
</div>

<div class="plugin-actions">
    <button name='saveProperties'>{msgPool type='okButton'}</button>
    <button name='cancelProperties'>{msgPool type='cancelButton'}</button>
</div>
