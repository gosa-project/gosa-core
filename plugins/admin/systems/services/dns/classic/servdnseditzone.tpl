
<h3>{t}Generic{/t}</h3>

<table summary="{t}DNS zone{/t}" width="100%">
 <tr>
  <td style='width:50%;' class='right-border'>
   <table summary="{t}Generic settings{/t}">
    <tr>
     <td>{t}Zone name{/t}
      {$must}
     </td>
     <td>
      {render acl=$zoneNameACL}
       <input type="text" name="zoneName" value="{$zoneName}" {if $NotNew || $Zone_is_used} disabled {/if}>
      {/render}
     </td>
    </tr>
    <tr>
     <td>{t}Network address{/t}
      {$must}
     </td>
     <td>
      {render acl=$ReverseZoneACL}
       <input type="text" name="ReverseZone" value="{$ReverseZone}" {if $NotNew || $Zone_is_used} disabled {/if}>
      {/render}
     </td>
    </tr>
    <tr>
     <td>{t}Net mask{/t}
     </td>
     <td>
      {render acl=$NetworkClassACL}
       <select name="NetworkClass" {if $NotNew || $Zone_is_used} disabled{/if} size=1>
        {html_options options=$NetworkClasses selected=$NetworkClass}
       </select>
      {/render}
     </td>
    </tr>
    
    {if $Zone_is_used}
     <tr>
      <td colspan="2"><i>{t}Zone is in use, network settings can't be modified.{/t}</i>
      </td>
     </tr>
    {/if}
   </table>
  </td>
  <td>
   <table summary="{t}Zone records{/t}">
    <tr>
     <td>{t}Zone records{/t}
      <br>
      {if $AllowZoneEdit == false}<i>{t}Can't be edited because the zone wasn't saved right now.{/t}</i>{/if}
     </td>
     <td>
      {render acl=$zoneEditorACL mode=read_active}
       <button type='submit' name='EditZoneEntries' {if $AllowZoneEdit == false}disabled {/if}>{t}Edit{/t}</button>
      {/render}
     </td>
    </tr>
   </table>
  </td>
 </tr>
</table>

<hr>

<h3>{t}SOA record{/t}</h3>

<table summary="{t}Zone settings{/t}" width="100%">
 <tr>
  <td style='width:50%;' class='right-border'>
   <table summary="{t}SOA record{/t}">
    <tr>
     <td>
      {t}Primary DNS server for this zone{/t}
      {$must}
     </td>
     <td>
      {render acl=$sOAprimaryACL}
       <input type="text" name="sOAprimary" value="{$sOAprimary}">
      {/render}
     </td>
    </tr>
    <tr>
     <td>{t}Mail address{/t} {$must}
     </td>
     <td>
      {render acl=$sOAmailACL}
       <input type="text" name="sOAmail" value="{$sOAmail}">
      {/render}
     </td>
    </tr>
    <tr>
     <td>{t}Serial number (automatically incremented){/t}
      {$must}
     </td>
     <td>
      {render acl=$sOAserialACL}
       <input type="text" name="sOAserial" value="{$sOAserial}">
      {/render}
     </td>
    </tr>
   </table>
  </td>
  <td>
   <table summary="{t}SOA record{/t}">
    <tr>
     <td>{t}Refresh{/t}
      {$must}
     </td>
     <td>
      {render acl=$sOArefreshACL}
       <input type="text" name="sOArefresh" value="{$sOArefresh}">
      {/render}
     </td>
    </tr>
    <tr>
     <td>{t}Retry{/t}
      {$must}
     </td>
     <td>
      {render acl=$sOAretryACL}
       <input type="text" name="sOAretry" value="{$sOAretry}">
      {/render}
     </td>
    </tr>
    <tr>
     <td>{t}Expire{/t}
      {$must}
     </td>
     <td>
      {render acl=$sOAexpireACL}
       <input type="text" name="sOAexpire" value="{$sOAexpire}">
      {/render}
     </td>
    </tr>
    <tr>
     <td>{t}TTL{/t}
      {$must}
     </td>
     <td>
      {render acl=$sOAttlACL}
       <input type="text" name="sOAttl" value="{$sOAttl}">
      {/render}
     </td>
    </tr>
   </table>
  </td>
 </tr>
</table>
<hr>
<br>
<table summary="{t}Mx record{/t}" width="100%">
 <tr>
  <td style='width:50%;' class='right-border'>
   <h3>{t}MX records{/t}</h3>
   <table width="100%" summary="{t}MX records{/t}">
    <tr>
     <td>
      {render acl=$mXRecordACL}
       {$Mxrecords}
      {/render}
      {render acl=$mXRecordACL}
       <input type="text" 		name="StrMXRecord" value="">
      {/render}
      {render acl=$mXRecordACL}
       <button type='submit' name='AddMXRecord'>{msgPool type=addButton}</button>
      {/render}
     </td>
    </tr>
   </table>
  </td>
  <td>
   <h3>{t}Global zone records{/t}
   </h3>
   {render acl=$zoneRecordsACL}
    {$records}
   {/render}
  </td>
 </tr>
</table>

<hr>
<div class="plugin-actions">
  <button type='submit' name='SaveZoneChanges'>{msgPool type=saveButton}</button>
  <button type='submit' name='CancelZoneChanges'>{msgPool type=cancelButton}</button>
</div>
<script language="JavaScript" type="text/javascript">
 <!-- // First input field on page	
  focus_field('zoneName');  
 -->
</script>
