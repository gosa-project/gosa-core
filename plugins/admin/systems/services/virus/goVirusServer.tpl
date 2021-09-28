
<table style='width:100%;' summary="{t}Anti virus setting{/t}">
 <tr>
  <td colspan=2>
   <h3>{t}Generic virus filtering{/t}</h3>
  </td>
 </tr>
 <tr>
  <td>
   <table summary="{t}Database setting{/t}">
    <tr>
     <td>{t}Database user{/t}</td>
     <td>
      {render acl=$avUserACL}
       <input type='text' name='avUser' value='{$avUser}' style='width:220px;'>
      {/render}
     </td>
    </tr>
    <tr>
     <td>{t}Database mirror{/t}
     </td>
     <td>
      {render acl=$avDatabaseMirrorACL}
       <input type='text' name='avDatabaseMirror' value='{$avDatabaseMirror}' style='width:220px;'>
      {/render}
     </td>
    </tr>
    <tr>
     <td>{t}HTTP proxy URL{/t}</td>
     <td>
      {render acl=$avHttpProxyURLACL}
       <input type='text' name='avHttpProxyURL' value='{$avHttpProxyURL}' style='width:220px;'>
      {/render}
     </td>
    </tr>
    <tr>
     <td>{t}Maximum threads{/t}
     </td>
     <td>
      {render acl=$avMaxThreadsACL}
       <select name="avMaxThreads" title='{t}Select number of maximal threads{/t}' size=1>
        {html_options options=$ThreadValues selected=$avMaxThreads}
       </select>
      {/render}
     </td>
    </tr>
   </table>
  </td>
  <td class='left-border'>
   <table summary="{t}Database setting{/t}">
    <tr>
     <td>{t}Max directory recursions{/t}</td>
     <td>
      {render acl=$avMaxDirectoryRecursionsACL}
       <input type='text' name='avMaxDirectoryRecursions' value='{$avMaxDirectoryRecursions}' >
      {/render}
     </td>
    </tr>
    <tr>
     <td>{t}Checks per day{/t}
     </td>
     <td>
      {render acl=$avChecksPerDayACL}
       <input type='text' name='avChecksPerDay' value='{$avChecksPerDay}'>
      {/render}
     </td>
    </tr>
    <tr>
     <td colspan=2>
      {render acl=$avFlagsDACL}
       <input type='checkbox' name='avFlagsD' {$avFlagsDCHK} value='1'>
      {/render}{t}Enable debugging{/t}
     </td>
    </tr>
    <tr>
     <td colspan=2>
      {render acl=$avFlagsSACL}
       <input type='checkbox' name='avFlagsS' {$avFlagsSCHK} value='1'>
      {/render}{t}Enable mail scanning{/t}
     </td>
    </tr>
   </table>
  </td>
 </tr>
 <tr>
  <td colspan=2>
   <hr>
   <h3>{t}Archive scanning{/t}
   </h3>
  </td>
 </tr>
 <tr>
  <td>
   <table summary="{t}Archive setting{/t}">
    <tr>
     <td>
      {render acl=$avFlagsAACL}
       <input type='checkbox' name='avFlagsA' {$avFlagsACHK} value='1' 
        onClick=" changeState('avFlagsE') ; 				  
        changeState('avArchiveMaxFileSize') ; 				  
        changeState('avArchiveMaxRecursion') ; 				  
        changeState('avArchiveMaxCompressionRatio');">
      {/render}
      {t}Enable scanning of archives{/t}
     </td>
    </tr>
    <tr>
     <td>
      {render acl=$avFlagsEACL}
       <input type='checkbox' name='avFlagsE' {$avFlagsECHK} {$avFlagsAState} 
        value='1' id='avFlagsE'>
      {/render}{t}Block encrypted archives{/t}
     </td>
    </tr>
   </table>
  </td>
  <td style='width:50%;' class='left-border'>
   <table summary="{t}Archive setting{/t}">
    <tr>
     <td>{t}Maximum file size{/t}</td>
     <td>
      {render acl=$avArchiveMaxFileSizeACL}
       <input type='text' name='avArchiveMaxFileSize' id='avArchiveMaxFileSize' 
        value='{$avArchiveMaxFileSize}'
       {$avFlagsAState}>
      {/render}
     </td>
    </tr>
    <tr>
     <td>{t}Maximum recursion{/t}
     </td>
     <td>
      {render acl=$avArchiveMaxRecursionACL}
       <input type='text' name='avArchiveMaxRecursion' id='avArchiveMaxRecursion' 
        value='{$avArchiveMaxRecursion}'
       {$avFlagsAState}>
      {/render}
     </td>
    </tr>
    <tr>
     <td>{t}Maximum compression ratio{/t}
     </td>
     <td>
      {render acl=$avArchiveMaxCompressionRatioACL}
       <input type='text' name='avArchiveMaxCompressionRatio' id='avArchiveMaxCompressionRatio'
         value='{$avArchiveMaxCompressionRatio}'
       {$avFlagsAState}>
      {/render}
     </td>
    </tr>
   </table>
  </td>
 </tr>
</table>
<input type='hidden' name='goVirusServer' value='1'>

<hr>

<div class="plugin-actions">
 <button type='submit' name='SaveService'>{msgPool type=saveButton}</button>
 <button type='submit' name='CancelService'>{msgPool type=cancelButton}</button>
</div>
