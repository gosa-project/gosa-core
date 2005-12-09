<table summary="" style="width:100%;">
 <tr>
  <td style="width:50%; vertical-align:top;">
<h2><img class="center" alt="" align="middle" src="images/penguin.png"> {t}Boot parameters{/t}</h2>

   <table summary="">
    <tr>
     <td><LABEL for="gotoLdapServer">{t}LDAP server{/t}</LABEL></td>
     <td>
      <select name="gotoLdapServer" id="gotoLdapServer" title="{t}Choose LDAP server to use for accounts and terminal management{/t}" {$gotoLdapServerACL}>
       {html_options values=$ldapservers output=$ldapservers selected=$gotoLdapServer_select}
	   <option disabled>&nbsp;</option>
      </select>
     </td>
    </tr>
    <tr>
     <td><LABEL for="gotoBootKernel">{t}Boot kernel{/t}</LABEL></td>
     <td><input id="gotoBootKernel" name="gotoBootKernel" size=20 maxlength=60 {$gotoBootKernelACL}
                value="{$gotoBootKernel}" title="{t}Enter the filename of the kernel to use{/t}"></td>
    </tr>
    <tr>
     <td><LABEL for="customParameters">{t}Custom options{/t}</LABEL></td>
     <td><input name="customParameters" id="customParameters"  size=25 maxlength=500 {$gotoKernelParametersACL}
                value="{$customParameters}" title="{t}Enter any parameters that should be passed to the kernel as append line during bootup{/t}"></td>
    </tr>
   </table>

  </td>

  <td style="border-left:1px solid #A0A0A0">
     &nbsp;
  </td>
  
  <td style="vertical-align:top;">
	<h2><img class="center" alt="" align="middle" src="images/fai_profile.png">&nbsp;{t}FAI server{/t}</h2>
		<!--<select name="FAIdebianMirror" {$FAIdebianMirrorACL} onchange="document.mainform.submit();">-->
		<select name="FAIdebianMirror" {$FAIdebianMirrorACL}>
        {html_options options=$FAIdebianMirrors output=$FAIdebianMirrors selected=$FAIdebianMirror}
        <option disabled>&nbsp;</option>
		</select>
		<!--<input type="submit" value="{t}set{/t}" name="refresh">-->
	<h2><img class="center" alt="" align="middle" src="images/fai_profile.png">&nbsp;{t}Assigned FAI classes{/t}</h2>
		{$FAIScriptlist}	

    <br>
		<select name="FAIclassesSel" {$FAIclassACL}>
        {html_options values=$FAIclassesKeys output=$FAIclasses}
        <option disabled>&nbsp;</option>
		</select>	
   		<input name="AddClass" value="{t}Add{/t}" type="submit"> 
   		<input name="DelClass" value="{t}Delete{/t}" type="submit"> 
  </td>
 </tr>
</table>

<table summary="" style="width:100%;">
 <tr><td colspan=2><p class="plugbottom" style="margin-top:0px;"></p></td></tr>

 <tr>
  <td style="width:50%; vertical-align:top; border-right:1px solid #B0B0B0">
   <h2>
    <img class="center" alt="" align="middle" src="images/hardware.png"> {t}Kernel modules (format: name parameters){/t}
   </h2>
    <select style="width:100%; height:150px;" name="modules_list[]" size=15 multiple title="{t}Add additional modules to load on startup{/t}">
     {html_options values=$gotoModules output=$gotoModules}
	 <option disabled>&nbsp;</option>
    </select>
    <br>
    <input name="module" size=30 align=middle maxlength=30 {$gotoModulesACL}>
    <input type=submit value="{t}Add{/t}" name="add_module" {$gotoModulesACL}>&nbsp;
    <input type=submit value="{t}Delete{/t}" name="delete_module" {$gotoModulesACL}>
  </td>

  <td style="vertical-align:top;">
        <h2><img class="center" alt="" src="images/fai_partitionTable.png" align="middle">&nbsp;<LABEL for="gotoShare">{t}Shares{/t}</LABEL></h2>
        <table summary="" style="width:100%">
                <tr>
                        <td>
                        <select style="width:100%;height:150px;" name="gotoShare" multiple size=4 {$gotoShareACL} id="gotoShare">
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
