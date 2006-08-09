<h2><img class="center" alt="" align="middle" src="images/penguin.png"> {t}Boot parameters{/t}</h2>
<table summary="" style="width:100%;">
 <tr>
  <td style="width:50%; vertical-align:top;">

   <table summary="" style="width:100%">
    <tr>
     <td style="width:30%"><LABEL for="gotoBootKernel">{t}Boot kernel{/t}</LABEL></td>
     <td>
	<select id="gotoBootKernel" name="gotoBootKernel" {$gotoBootKernelACL}>
	{html_options options=$gotoBootKernels  selected=$gotoBootKernel}
	<option disabled>&nbsp;</option>
	</select>
     </td>
    </tr>
    <tr>
     <td><LABEL for="customParameters">{t}Custom options{/t}</LABEL></td>
     <td><input name="customParameters" id="customParameters"  size=25 maxlength=500 {$gotoKernelParametersACL}
                value="{$customParameters}" title="{t}Enter any parameters that should be passed to the kernel as append line during bootup{/t}"></td>
    </tr>
    <tr>
     <td colspan="2" style='vertical-align:top;padding-top:3px;width:100%'><LABEL for="gotoLdapServer">{t}LDAP server{/t}</LABEL>
		{$SelectBoxLdapServer}
     </td>
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

<table summary="" style="width:100%;">
 <tr><td colspan=2><p class="plugbottom" style="margin-top:0px;"></p></td></tr>

 <tr>
  <td style="width:50%; vertical-align:top;">
   <h2>
    <img class="center" alt="" align="middle" src="images/hardware.png"> {t}Kernel modules (format: name parameters){/t}
   </h2>
    <select style="width:100%;" name="modules_list[]" size=15 multiple title="{t}Add additional modules to load on startup{/t}">
     {html_options values=$gotoModules output=$gotoModules}
	 <option disabled>&nbsp;</option>
    </select>
    <br>
    <input name="module" size=30 align=middle maxlength=30 {$gotoModulesACL}>
    <input type=submit value="{t}Add{/t}" name="add_module" {$gotoModulesACL}>&nbsp;
    <input type=submit value="{t}Delete{/t}" name="delete_module" {$gotoModulesACL}>
  </td>

  <td style="padding-left:10px;border-left:1px solid #A0A0A0;vertical-align:top">
        <h2><img class="center" alt="" src="images/fai_partitionTable.png" align="middle">&nbsp;<LABEL for="gotoShare">{t}Shares{/t}</LABEL></h2>
        <table summary="" style="width:100%">
                <tr>
                        <td>
                        <select style="width:100%;" name="gotoShare" multiple size=15 {$gotoShareACL} id="gotoShare">
        {html_options values=$gotoShareKeys output=$gotoShares}
        <option disabled>&nbsp;</option>
                                </select>
                                <br>
                        <select name="gotoShareSelection" {$gotoShareACL}>
        {html_options values=$gotoShareSelectionKeys output=$gotoShareSelections}
        <option disabled>&nbsp;</option>
                                </select>
                                <input type="text" size=15 {$gotoShareACL} name="gotoShareMountPoint" value="{t}Mountpoint{/t}">
                                <input type="submit" {$gotoShareACL} name="gotoShareAdd" value="{t}Add{/t}">
                                <input type="submit" {$gotoShareACL} name="gotoShareDel" value="{t}Remove{/t}">
                        </td>
                </tr>
        </table>
  </td>
 </tr>
</table>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
  document.mainform.gotoLdapServer.focus();
  -->
</script>
