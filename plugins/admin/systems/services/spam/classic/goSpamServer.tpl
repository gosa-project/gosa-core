
<table style='width:100%' summary="{t}Spam taggin{/t}">
 <tr>
  <td style='width:50%;'>
   <h3>{t}Spam tagging{/t}</h3>
   <table summary="{t}Spam taggin{/t}">
    <tr>
     <td>{t}Rewrite header{/t}</td>
     <td>
      {render acl=$saRewriteHeaderACL}
       <input type='text' name='saRewriteHeader' value='{$saRewriteHeader}'>
      {/render}
     </td>
    </tr>
    <tr>
     <td>{t}Required score{/t}</td>
     <td>
      {render acl=$saRequiredScoreACL}
       <select name='saRequiredScore' title='{t}Select required score to tag mail as SPAM{/t}' size=1>
        {html_options options=$SpamScore selected=$saRequiredScore}
       </select>
      {/render}
     </td>
    </tr>
   </table>
  </td>
  <td class='left-border'>
  
   <h3>Trusted networks</h3>

   <table width='100%' summary="{t}Network settings{/t}">
    <tr>
     <td>
      {render acl=$saTrustedNetworksACL}
       <select name='TrustedNetworks[]' size=4 style='width:100%;' multiple>
        {html_options options=$TrustedNetworks}
       </select>
       <br>
      {/render}
      {render acl=$saTrustedNetworksACL}
       <input type='text'	name='NewTrustName' value=''>&nbsp;
      {/render}
      {render acl=$saTrustedNetworksACL}
       <button type='submit' name='AddNewTrust'>{msgPool type=addButton}</button>
      {/render}
      {render acl=$saTrustedNetworksACL}
       <button type='submit' name='DelTrust'>{t}Remove{/t}</button>
      {/render}
     </td>
    </tr>
   </table>
  </td>
 </tr>
 <tr>
  <td colspan=2>
   <hr>
  </td>
 </tr>
 <tr>
  <td>
   <h3>{t}Flags{/t}</h3>
   <table summary="{t}Flags{/t}">
    <tr>
     <td>
      {render acl=$saFlagsBACL}
       <input type='checkbox' name='saFlagsB' value='1' {$saFlagsBCHK}> &nbsp;{t}Enable use of Bayes filtering{/t}
      {/render}
      <br>
      {render acl=$saFlagsbACL}
       <input type='checkbox' name='saFlagsb' value='1' {$saFlagsbCHK}> &nbsp;{t}Enable Bayes auto learning{/t}
      {/render}
      <br>
      {render acl=$saFlagsCACL}
       <input type='checkbox' name='saFlagsC' value='1' {$saFlagsCCHK}> &nbsp;{t}Enable RBL checks{/t}
      {/render}
     </td>
    </tr>
   </table>
  </td>
  <td class='left-border'>
   <table summary="{t}Flags{/t}">
    <tr>
     <td>
      {render acl=$saFlagsRACL}
       <input type='checkbox' name='saFlagsR' value='1' {$saFlagsRCHK}> &nbsp;{t}Enable use of Razor{/t}
      {/render}
      <br>
      {render acl=$saFlagsDACL}
       <input type='checkbox' name='saFlagsD' value='1' {$saFlagsDCHK}> &nbsp;{t}Enable use of DDC{/t}
      {/render}
      <br>
      {render acl=$saFlagsPACL}
       <input type='checkbox' name='saFlagsP' value='1' {$saFlagsPCHK}> &nbsp;{t}Enable use of Pyzor{/t}
      {/render}
     </td>
    </tr>
   </table>
  </td>
 </tr>
 <tr>
  <td colspan=2>
   <hr>
  </td>
 </tr>
 <tr>
  <td colspan='2'>
   <h3>{t}Rules{/t}</h3>

   <table width='100%' summary="{t}Rules{/t}">
    <tr>
     <td>
      {$ruleList}
      <br>
      {render acl=$saTrustedNetworksACL}
       <button type='submit' name='AddRule'>{msgPool type=addButton}</button>
      {/render}
     </td>
    </tr>
   </table>

  </td>
 </tr>
</table>

<input type='hidden' value='1' name='goSpamServer'>

<hr>

<div class="plugin-actions">
  <button type='submit' name='SaveService'>{msgPool type=saveButton}</button>
  <button type='submit' name='CancelService'>{msgPool type=cancelButton}</button>
</div>
