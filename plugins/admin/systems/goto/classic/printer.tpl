<table summary="{t}Printer{/t}" width="100%">
 <tr>
  <td style='width:50%; ' class='right-border'>

{if $StandAlone}
   <h3>{t}General{/t}</h3>
   <table summary="{t}Generic settings{/t}">
    <tr>
     <td><LABEL for="cn" >{t}Printer name{/t}</LABEL>{$must}</td>
     <td>
{render acl=$cnACL}
      <input type='text' name="cn" id="cn" size=20 maxlength=60 value="{$cn}">
{/render}
     </td>
    </tr>
    <tr>
     <td><LABEL for="description">{t}Description{/t}</LABEL></td>
     <td>
{render acl=$descriptionACL}
      <input type='text' id="description" name="description" size=25 maxlength=80 value="{$description}">
{/render}
     </td>
    </tr>
    <tr>
      <td colspan=2>&nbsp;</td>
    </tr>
    <tr>
     <td><LABEL for="base">{t}Base{/t}</LABEL>{$must}</td>
     <td>
{render acl=$baseACL}
       {$base}
{/render}
     </td>
    </tr>
   </table>
  </td>
  <td>
{/if}
   <h3>{t}Details{/t}</h3>
   <table summary="{t}Details{/t}">
{if !$StandAlone}
      <tr> 
	 <td><LABEL for="description">{t}Description{/t}</LABEL></td> 
	 <td> 
	{render acl=$descriptionACL} 
	   <input type='text' id="description" name="description" size=25 maxlength=80 value="{$description}"> 
	{/render} 
	 </td> 
      </tr> 
{/if} 
     <tr>
       <td><LABEL for="l">{t}Printer location{/t}</LABEL></td>
       <td>
{render acl=$lACL}
        <input type='text' id="l" name="l" size=30 maxlength=80 value="{$l}">
{/render}
       </td>
     </tr>
     <tr>
       <td><LABEL for="labeledURI">{t}Printer URL{/t}</LABEL>{$must}</td>
       <td>
{render acl=$labeledURIACL}
        <input type='text' id="labeledURI" name="labeledURI" size=30 maxlength=80 value="{$labeledURI}">
{/render}
       </td>
     </tr>
{if $displayServerPath && 0}
    <tr>
     <td>{t}PPD Provider{/t}
     </td>
     <td>
      <input size=30 type='text' value='{$ppdServerPart}' name='ppdServerPart'>
     </td>
    </tr>
{/if}
   </table>
   <table summary="{t}Driver configuration{/t}">
    <tr> 
     <td>
      <br>
      {t}Driver{/t}: <i>{$driverInfo}</i>&nbsp;
{render acl=$gotoPrinterPPDACL mode=read_active}
       <button type='submit' name='EditDriver'>{t}Edit{/t}</button>

{/render}
{render acl=$gotoPrinterPPDACL}
       <button type='submit' name='RemoveDriver'>{t}Remove{/t}</button>

{/render}
     </td>
    </tr>
   </table>
  </td>
 </tr>
</table>

<hr>

<h3>{t}Permissions{/t}</h3>
<table summary="{t}Permissions{/t}" width="100%">
 <tr>
  <td style='width:50%' class='right-border'>

   <table style="width:100%" summary='{t}Permissions{/t}'>
    <tr>
     <td>
      {t}Users which are allowed to use this printer{/t}<br>
{render acl=$gotoUserPrinterACL}
      <select size="1" name="UserMember[]" title="{t}Users{/t}" style="width:100%;height:120px;"  multiple>
       {html_options options=$UserMembers values=$UserMemberKeys}
      </select><br>
{/render}
{render acl=$gotoUserPrinterACL}
      <button type='submit' name='AddUser'>{msgPool type=addButton}</button>

{/render}
{render acl=$gotoUserPrinterACL}
      <button type='submit' name='DelUser'>{msgPool type=delButton}</button>

{/render}
     </td>
    </tr>
   </table> 
 
  </td>
  <td>
   <table style="width:100%" summary='{t}Permissions{/t}'>
    <tr>
     <td>
      {t}Users which are allowed to administrate this printer{/t}<br>
{render acl=$gotoUserPrinterACL}
           <select size="1" name="AdminMember[]" title="{t}Administrators{/t}" style="width:100%;height:120px;"  multiple>
                    {html_options options=$AdminMembers values=$AdminMemberKeys}
                   </select><br>
{/render}
{render acl=$gotoUserPrinterACL}
       <button type='submit' name='AddAdminUser'>{msgPool type=addButton}</button>

{/render}
{render acl=$gotoUserPrinterACL}
       <button type='submit' name='DelAdmin'>{msgPool type=delButton}</button>

{/render}
  
     </td>
    </tr>
   </table>
   
  </td>
 </tr>
</table>


{if $netconfig ne ''}
<hr>
{$netconfig}
{/if}

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">

  <!-- // First input field on page
  if(document.mainform.cn)
	focus_field('cn');
  -->
</script>
