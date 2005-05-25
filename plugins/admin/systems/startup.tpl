<h2><img alt="" align="middle" src="images/penguin.png"> {t}Boot parameters{/t}</h2>
<table style="width:100%;">
 <tr>
  <td style="width:50%; vertical-align:top;">

   <table>
    <tr>
     <td>{t}LDAP server{/t}</td>
     <td>
      <select name="gotoLdapServer" title="{t}Choose LDAP server to use for accounts and terminal management{/t}" {$gotoLdapServerACL}>
       {html_options values=$ldapservers output=$ldapservers selected=$gotoLdapServer_select}
      </select>
     </td>
    </tr>
    <tr>
     <td>{t}Boot kernel{/t}</td>
     <td><input name="gotoBootKernel" size=20 maxlength=60 {$gotoBootKernelACL}
                value="{$gotoBootKernel}" title="{t}Enter the filename of the kernel to use{/t}"></td>
    </tr>
    <tr>
     <td>{t}Custom options{/t}</td>
     <td><input name="customParameters" size=25 maxlength=500 {$gotoKernelParametersACL}
                value="{$customParameters}" title="{t}Enter any parameters that should be passed to the kernel as append line during bootup{/t}"></td>
    </tr>
   </table>

  </td>

  <td style="border-left:1px solid #A0A0A0">
     &nbsp;
  </td>
  
  <td style="vertical-align:top;">
    <input type="radio" name="bootmode" value="G" title="{t}Select if terminal supports graphical startup with progress bar{/t}" {$graphicalbootup} {$gotoKernelParametersACL}>
    {t}use graphical bootup{/t}
    <br>
    <input type="radio" name="bootmode" value="T" title="{t}Select if terminal should boot in text mode{/t}" {$textbootup} {$gotoKernelParametersACL}>
    {t}use standard linux textual bootup{/t}
    <br>
    <input type="radio" name="bootmode" value="D" title="{t}Select to get more verbose output during startup{/t}" {$debugbootup} {$gotoKernelParametersACL}>
    {t}use debug mode for startup{/t}
  </td>
 </tr>
</table>

<table style="width:100%;">
 <tr><td colspan=2><p class="plugbottom" style="margin-top:0px;"></p></td></tr>

 <tr>
  <td style="width:50%; vertical-align:top;">
   <h2>
    <img alt="" align="middle" src="images/hardware.png"> {t}Kernel modules (format: name parameters){/t}
   </h2>
    <select style="width:100%; height:100px;" name="modules_list[]" size=15 multiple title="{t}Add additional modules to load on startup{/t}">
     {html_options values=$gotoModules output=$gotoModules}
    </select>
    <br>
    <input name="module" size=30 align=middle maxlength=30 {$gotoModulesACL}>
    <input type=submit value="{t}Add{/t}" name="add_module" {$gotoModulesACL}>&nbsp;
    <input type=submit value="{t}Delete{/t}" name="delete_module" {$gotoModulesACL}>
  </td>

  <td style="vertical-align:top;">
   <h2>
    <img alt="" align="middle" src="images/zip.png"> {t}Automount devices (format: complete autofs entry){/t}
   </h2>
   <select style="width:100%; height:100px;" name="autofs_list[]" size=15 multiple title="{t}Add additional automount entries{/t}">
    {html_options values=$gotoAutoFs output=$gotoAutoFs}
   </select>
   <br>
   <input name="autofs" size=30 align=middle maxlength=80 {$gotoAutoFs}>
   <input type=submit value="{t}Add{/t}" name="add_autofs" {$gotoAutoFs}>&nbsp;
   <input type=submit value="{t}Delete{/t}" name="delete_autofs" {$gotoAutoFs}>
  </td>
 </tr>
</table>

<table style="width:100%;">
 <tr><td colspan=2><p class="plugbottom" style="margin-top:0px;"></p></td></tr>

 <tr>
  <td style="width:50%; vertical-align:top;">
   <h2>
    <img alt="" align="middle" src="images/folder.png"> {t}Additional fstab entries{/t}
   </h2>
   <select style="width:100%; height:100px;" name="fstab_list[]" size=15 multiple title="{t}Add additional filesystems to be mounted during startup{/t}">
    {html_options values=$gotoFilesystem output=$gotoFilesystem}
   </select>
   <br>
   <input name="fstab" size=80 align=middle maxlength=200 {$gotoFilesystemACL}>
   <input type=submit value="{t}Add{/t}" name="add_fs" {$gotoFilesystemACL}>&nbsp;
   <input type=submit value="{t}Delete{/t}" name="delete_fs" {$gotoFilesystemACL}>
  </td>
 </tr>
</table>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.gotoLdapServer.focus();
  -->
</script>
