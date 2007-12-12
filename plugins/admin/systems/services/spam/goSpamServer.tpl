<table style='width:100%'>
 <tr>
  <td style='width:50%;vertical-align:top;'><h2>Spam tagging</h2>

   <table>
    <tr>
     <td>
      {t}Rewrite header{/t}      
     </td>
     <td>
{render acl=$saRewriteHeaderACL}
      <input type='text' name='saRewriteHeader' value='{$saRewriteHeader}'>
{/render}
     </td>
    </tr>
    <tr>
     <td>
      {t}Required score{/t}      
     </td>
     <td>
{render acl=$saRequiredScoreACL}
      <select name='saRequiredScore' title='{t}Select required score to tag mail as spam{/t}'>
       {html_options options=$SpamScore selected=$saRequiredScore}
      </select>
{/render}
     </td>
    </tr>
   </table>

  </td>
  <td style="vertical-align:top;border-left:1px solid #A0A0A0;"><h2>Trusted networks</h2>

   <table width='100%'>
    <tr>
     <td>
{render acl=$saTrustedNetworksACL}
      <select name='TrustedNetworks[]' size=4 style='width:100%;' multiple>
       {html_options options=$TrustedNetworks}
      </select><br>
{/render}
{render acl=$saTrustedNetworksACL}
      <input type='text'	name='NewTrustName' value=''>&nbsp;
{/render}
{render acl=$saTrustedNetworksACL}
      <input type='submit'      name='AddNewTrust'  value='{t}Add{/t}'>
{/render}
{render acl=$saTrustedNetworksACL}
      <input type='submit'      name='DelTrust'     value='{t}Remove{/t}'>
{/render}
     </td>
    </tr>
   </table>

  </td>
 </tr>
 <tr>
  <td colspan=2>
   <p class='seperator'>&nbsp;</p>
  </td>
 </tr>
 <tr>
  <td>
	<h2>Flags</h2>
	
   <table>
    <tr>
     <td>
{render acl=$saFlagsBACL}
      <input type='checkbox' name='saFlagsB' value='1' {$saFlagsBCHK}> &nbsp;{t}Enable use of bayes filtering{/t}<br>
{/render}
{render acl=$saFlagsbACL}
      <input type='checkbox' name='saFlagsb' value='1' {$saFlagsbCHK}> &nbsp;{t}Enable bayes auto learning{/t}<br>
{/render}
{render acl=$saFlagsCACL}
      <input type='checkbox' name='saFlagsC' value='1' {$saFlagsCCHK}> &nbsp;{t}Enable RBL checks{/t}
{/render}
     </td>
    </tr>
   </table>
  </td>
  <td style="vertical-align:bottom;border-left:1px solid #A0A0A0;">
   <table>
    <tr>
     <td>
{render acl=$saFlagsRACL}
      <input type='checkbox' name='saFlagsR' value='1' {$saFlagsRCHK}> &nbsp;{t}Enable use of Razor{/t}<br>
{/render}
{render acl=$saFlagsDACL}
      <input type='checkbox' name='saFlagsD' value='1' {$saFlagsDCHK}> &nbsp;{t}Enable use of DDC{/t}<br>
{/render}
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
   <p class='seperator'>&nbsp;</p>
  </td>
 </tr>
 <tr>
  <td colspan='2'><h2>Rules</h2>

   <table width='100%'>
    <tr>
     <td>
      {$divRules}<br>
{render acl=$saTrustedNetworksACL}
      <input type='submit' name='AddRule' value='{t}Add{/t}'> 
{/render}
     </td>
    </tr>
   </table>

  </td>
</table>
<input type='hidden' value='1' name='goSpamServer'>

<p class='seperator'>&nbsp;</p>
<div style="width:100%; text-align:right;padding-top:10px;padding-bottom:3px;">
    <input type='submit' name='SaveService' value='{t}Save{/t}'>
    &nbsp;
    <input type='submit' name='CancelService' value='{t}Cancel{/t}'>
</div>

