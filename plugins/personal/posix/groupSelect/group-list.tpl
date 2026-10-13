<div id="mainlist">

  <div class="mainlist-header">
   <p>
    {$HEADLINE}
    {if $SHOW_SIZELIMIT_WARNING}
      {t}(list is incomplete){/t} <button class="btn-small primary" style="margin-left: 10px" type='submit' name='edit_sizelimit'>{t}Configure{/t}</button>
    {/if}
   </p>
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

<div class="plugin-actions">
  <button type='submit' name='add_groups_finish'>{msgPool type=addButton}</button>
  <button type='submit' name='add_groups_cancel'>{msgPool type=cancelButton}</button>
</div>

