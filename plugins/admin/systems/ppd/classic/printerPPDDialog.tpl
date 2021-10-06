
<h3>{t}Printer driver{/t}
</h3>

{if !$path_valid}
 <p>
  <b>
  {msgPool type=invalidConfigurationAttribute param=ppdPath}</b>
 </p>
 {else}
 <table summary="" width="100%">
  <tr>
   <td>{t}Model{/t}:<i>
    {$ppdString}</i>&nbsp;
    {render acl=$acl}
     <button type='submit' name='SelectPPD'>{t}Select{/t}</button>
    {/render}
   </td>
   <td style='padding-left:10px;' class='left-border'>{t}New driver{/t}&nbsp;
    {render acl=$acl}
     <input type="file" value="" name="NewPPDFile">
    {/render}
    {render acl=$acl}
     <button type='submit' name='SubmitNewPPDFile'>{t}Upload{/t}</button>
    {/render}
   </td>
  </tr>
 </table>
 
 {if $showOptions eq 1}
  <hr>
  <h3>{t}Options{/t}
  </h3>
  {$properties}
  
 {/if}
 
{/if}
<hr>
<div class="plugin-actions">
 
 {if $path_valid}
  {render acl=$acl}
   <button type='submit' name='SavePPD'>
   {msgPool type=applyButton}</button>
  {/render}
  
 {/if}
 <button type='submit' name='ClosePPD'>
 {msgPool type=cancelButton}</button>
</div>
<input type="hidden" name="PPDDisSubmitted" value="1">