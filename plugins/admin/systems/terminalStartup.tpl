<h2><img class="center" alt="" align="middle" src="images/penguin.png"> {t}Boot parameters{/t}</h2>
<table summary="" style="width:100%;">
 <tr>
  <td style="width:50%; vertical-align:top;">

   <table summary="" style="width:100%">
    <tr>
     <td style="width:30%"><LABEL for="gotoBootKernel">{t}Boot kernel{/t}</LABEL></td>
     <td>
{render acl=$gotoBootKernelACL}
	<select id="gotoBootKernel" name="gotoBootKernel">
	{html_options options=$gotoBootKernels  selected=$gotoBootKernel}
	<option disabled>&nbsp;</option>
	</select>
{/render}
     </td>
    </tr>
    <tr>
     <td><LABEL for="customParameters">{t}Custom options{/t}</LABEL></td>
     <td>
{render acl=$gotoKernelParametersACL}
      <input name="customParameters" id="customParameters"  size=25 maxlength=500
                value="{$customParameters}" title="{t}Enter any parameters that should be passed to the kernel as append line during bootup{/t}"></td>
{/render}
    </tr>
    <tr>
     <td colspan="2" style='vertical-align:top;padding-top:3px;width:100%'><LABEL for="gotoLdapServer">{t}LDAP server{/t}</LABEL>
{render acl=$gotoLdapServerACL}
(<input type='checkbox' name='gotoLdap_inherit' {if $gotoLdap_inherit} checked {/if} value="1"
      onClick="document.mainform.submit();" class='center'>
&nbsp;{t}inherit from group{/t})
{if !$JS}
      <input type='image' src="images/list_reload.png" alt='{t}Reload{/t}' class='center'>
{/if}
{/render}
{render acl=$gotoLdapServerACL_inherit}
        {$gotoLdapServers}
{/render}
{render acl=$gotoLdapServerACL_inherit}
      <select name='ldap_server_to_add' id='ldap_server_to_add'>
        {html_options options=$gotoLdapServerList}
    </select>
{/render}
{render acl=$gotoLdapServerACL_inherit}
      <input type='submit' name='add_ldap_server' value="{t}Add{/t}" id='add_ldap_server'>
{/render}
     </td>
    </tr>
   </table>

  </td>

  <td style="border-left:1px solid #A0A0A0">
     &nbsp;
  </td>
  
  <td style="vertical-align:top;">
{render acl=$gotoKernelParametersACL}
    <input type="radio" name="bootmode" value="G" title="{t}Select if terminal supports graphical startup with progress bar{/t}" {$graphicalbootup}>
{/render}
    {t}use graphical bootup{/t}
    <br>
{render acl=$gotoKernelParametersACL}
    <input type="radio" name="bootmode" value="T" title="{t}Select if terminal should boot in text mode{/t}" {$textbootup}>
{/render}
    {t}use standard linux textual bootup{/t}
    <br>
{render acl=$gotoKernelParametersACL}
    <input type="radio" name="bootmode" value="D" title="{t}Select to get more verbose output during startup{/t}" {$debugbootup}>
{/render}
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
{render acl=$gotoModulesACL}
    <select style="width:100%;" name="modules_list[]" size=15 multiple title="{t}Add additional modules to load on startup{/t}">
     {html_options values=$gotoModules output=$gotoModules}
	 <option disabled>&nbsp;</option>
    </select>
{/render}
    <br>
{render acl=$gotoModulesACL}
    <input name="module" size=30 align=middle maxlength=30>
{/render}
{render acl=$gotoModulesACL}
    <input type=submit value="{t}Add{/t}" name="add_module">&nbsp;
{/render}
{render acl=$gotoModulesACL}
    <input type=submit value="{t}Delete{/t}" name="delete_module">
{/render}
  </td>

  <td style="padding-left:10px;border-left:1px solid #A0A0A0;vertical-align:top">
        <h2><img class="center" alt="" src="images/fai_partitionTable.png" align="middle">&nbsp;<LABEL for="gotoShare">{t}Shares{/t}</LABEL></h2>
        <table summary="" style="width:100%">
                <tr>
                        <td>
{render acl=$gotoShareACL}
                        <select style="width:100%;" name="gotoShare" multiple size=15 id="gotoShare">
        			{html_options values=$gotoShareKeys output=$gotoShares}
			        <option disabled>&nbsp;</option>
                        </select>
{/render}
                                <br>
{render acl=$gotoShareACL}
        	                <select name="gotoShareSelection">
				        {html_options values=$gotoShareSelectionKeys output=$gotoShareSelections}
				        <option disabled>&nbsp;</option>
                                </select>
{/render}
{render acl=$gotoShareACL}
                                <input type="text" size=15 name="gotoShareMountPoint" value="{t}Mountpoint{/t}">
{/render}
{render acl=$gotoShareACL}
                                <input type="submit" name="gotoShareAdd" value="{t}Add{/t}">
{/render}
{render acl=$gotoShareACL}
                                <input type="submit" name="gotoShareDel" value="{t}Remove{/t}">
{/render}
                        </td>
                </tr>
        </table>
  </td>
 </tr>
</table>
<input type='hidden' name='TerminalStarttabPosted' value="1">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('gotoLdapServer');
  -->
</script>
