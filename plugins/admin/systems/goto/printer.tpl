<table summary="" width="100%">
 <tr>
  <td style="vertical-align:top; width:50%; border-right:1px solid #B0B0B0">
{if $StandAlone }
   <h2>{t}General{/t}</h2>
   <table summary="">
    <tr>
     <td><LABEL for="cn" >{t}Printer name{/t}</LABEL>{$must}</td>
     <td>
{render acl=$cnACL}
      <input name="cn" id="cn" size=20 maxlength=60 value="{$cn}">
{/render}
     </td>
    </tr>
    <tr>
     <td><LABEL for="description">{t}Description{/t}</LABEL></td>
     <td>
{render acl=$descriptionACL}
      <input id="description" name="description" size=25 maxlength=80 value="{$description}">
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
      <select id="base" size="1" name="base" title="{t}Choose subtree to place user in{/t}">
       {html_options options=$bases selected=$base_select}
      </select>
{/render}
{render acl=$baseACL disable_picture='images/folder_gray.png'}
      <input type="image" name="chooseBase" src="images/folder.png" class="center" title="{t}Select a base{/t}">
{/render}
     </td>
    </tr>
   </table>
  </td>
  <td>
{/if}
   <h2>{t}Details{/t}</h2>
   <table summary="">
     <tr>
       <td><LABEL for="l">{t}Printer location{/t}</LABEL></td>
       <td>
{render acl=$lACL}
        <input id="l" name="l" size=25 maxlength=80 value="{$l}">
{/render}
       </td>
     </tr>
     <tr>
       <td><LABEL for="labeledURI">{t}Printer URL{/t}</LABEL>{$must}</td>
       <td>
{render acl=$labeledURIACL}
        <input id="labeledURI" name="labeledURI" size=25 maxlength=80 value="{$labeledURI}">
{/render}
       </td>
     </tr>
   </table>
   <table summary="">
    <tr> 
     <td>
      <br>
      {t}Driver{/t}: <i>{$driverInfo}</i>&nbsp;
{render acl=$gotoPrinterPPDACL mode=read_active}
       <input type="submit" name="EditDriver" value="{t}Edit{/t}">
{/render}
{render acl=$gotoPrinterPPDACL}
       <input type="submit" name="RemoveDriver" value="{t}Remove{/t}">
{/render}
     </td>
    </tr>
   </table>
  </td>
 </tr>
</table>

<p class="plugbottom" style="margin-bottom:0px; padding:0px;">&nbsp;</p>

<h2><img class="center" alt="" align="middle" src="images/closedlock.png"> {t}Permissions{/t}</h2>
<table summary="" width="100%">
 <tr>
  <td style="border-right:1px solid #B0B0B0; width:50%">
   <table style="width:100%" summary=''>
    <tr>
     <td>
      {t}Users which are allowed to use this printer{/t}<br>
{render acl=$gotoUserPrinterACL}
      <select size="1" name="UserMember[]" title="{t}Users{/t}" style="width:100%;height:120px;"  multiple>
       {html_options options=$UserMembers values=$UserMemberKeys}
      </select><br>
{/render}
{render acl=$gotoUserPrinterACL}
      <input type="submit" value="{t}Add user{/t}"  name="AddUser">
{/render}
{render acl=$gotoUserPrinterACL}
      <input type="submit" value="{t}Add group{/t}" name="AddGroup">
{/render}
{render acl=$gotoUserPrinterACL}
      <input type="submit" value="{t}Delete{/t}" name="DelUser">
{/render}
     </td>
    </tr>
   </table> 
 
  </td>
  <td>
   <table style="width:100%" summary=''>
    <tr>
     <td>
      {t}Users which are allowed to administrate this printer{/t}<br>
{render acl=$gotoUserPrinterACL}
           <select size="1" name="AdminMember[]" title="{t}Admins{/t}" style="width:100%;height:120px;"  multiple>
                    {html_options options=$AdminMembers values=$AdminMemberKeys}
                   </select><br>
{/render}
{render acl=$gotoUserPrinterACL}
       <input type="submit" value="{t}Add user{/t}"  name="AddAdminUser">
{/render}
{render acl=$gotoUserPrinterACL}
       <input type="submit" value="{t}Add group{/t}" name="AddAdminGroup">
{/render}
{render acl=$gotoUserPrinterACL}
       <input type="submit" value="{t}Delete{/t}" name="DelAdmin">
{/render}
  
     </td>
    </tr>
   </table>
   
  </td>
 </tr>
</table>


{if $netconfig ne ''}
<p class="plugbottom" style="margin-bottom:0px; padding:0px;">&nbsp;</p>
{$netconfig}
{/if}

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">

  <!-- // First input field on page
  if(document.mainform.cn)
	focus_field('cn');
  -->
</script>
