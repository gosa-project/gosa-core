<table style='width:100%;'>
 <tr>
  <td style='width:50%;vertical-align:top;'><h2>Spam tagging</h2>

   <table>
    <tr>
     <td>
      {t}Rewrite header{/t}      
     </td>
     <td>
      <input type='text' name='saRewriteHeader' value='{$saRewriteHeader}' {$saRewriteHeaderACL}>
     </td>
    </tr>
    <tr>
     <td>
      {t}Required score{/t}      
     </td>
     <td>
      <select name='saRequiredScore' title='{t}Select required score to tag mail as spam{/t}'>
       {html_options options=$SpamScore selected=$saRequiredScore}
      </select>
     </td>
    </tr>
   </table>

  </td>
  <td style="vertical-align:top;border-left:1px solid #A0A0A0;"><h2>Trusted networks</h2>

   <table width='100%'>
    <tr>
     <td>
      <select name='TrustedNetworks[]' size=4 style='width:100%;' multiple {$saTrustedNetworksACL}>
       {html_options options=$TrustedNetworks}
      </select><br>
      <input type='text'	{$saTrustedNetworksACL} name='NewTrustName' value=''>&nbsp;
      <input type='submit'      {$saTrustedNetworksACL}	name='AddNewTrust'  value='{t}Add{/t}'>
      <input type='submit'      {$saTrustedNetworksACL}	name='DelTrust'     value='{t}Remove{/t}'>
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
      <input type='checkbox' {$saFlagsBACL} name='saFlagsB' value='1' {$saFlagsBCHK}> &nbsp;{t}Enable use of bayes filtering{/t}<br>
      <input type='checkbox' {$saFlagsbACL} name='saFlagsb' value='1' {$saFlagsbCHK}> &nbsp;{t}Enable bayes auto learning{/t}<br>
      <input type='checkbox' {$saFlagsCACL} name='saFlagsC' value='1' {$saFlagsCCHK}> &nbsp;{t}Enable RBL checks{/t}
     </td>
    </tr>
   </table>
  </td>
  <td style="vertical-align:bottom;border-left:1px solid #A0A0A0;">
   <table>
    <tr>
     <td>
      <input type='checkbox' {$saFlagsRACL} name='saFlagsR' value='1' {$saFlagsRCHK}> &nbsp;{t}Enable use of Razor{/t}<br>
      <input type='checkbox' {$saFlagsDACL} name='saFlagsD' value='1' {$saFlagsDCHK}> &nbsp;{t}Enable use of DDC{/t}<br>
      <input type='checkbox' {$saFlagsPACL} name='saFlagsP' value='1' {$saFlagsPCHK}> &nbsp;{t}Enable use of Pyzer{/t}
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

   <table width='100%;'>
    <tr>
     <td>
      {$divRules}<br>
      <input type='submit' name='AddRule' value='{t}Add{/t}' {$saTrustedNetworksACL}> 
     </td>
    </tr>
   </table>

  </td>
</table>
<input type='hidden' value='1' name='goSpamServer'>

{if !$servtabs}
<p class='seperator'>&nbsp;</p>
<p>
<div style="width:100%; text-align:right;">
    <input type='submit' name='SaveService' value='{t}Save{/t}'>
    &nbsp;
    <input type='submit' name='CancelService' value='{t}Cancel{/t}'>
</div>
</p>
{/if}

