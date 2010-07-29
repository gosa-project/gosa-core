{if $multiple_support}
<input type="hidden" value="1" name="group_mulitple_edit">
{/if}


<table summary="" style="width:100%;">
 <tr>
  <td style="width:50%">
   <table summary="" style="width:100%">
    <tr>
     <td><LABEL for="cn">{t}Group name{/t}</LABEL>{$must}</td>
     <td>
{if $multiple_support}
	<input type='text' id="dummy1" name="dummy1" size=25 maxlength=60 value="{t}Multiple edit{/t}" disabled>
{else}
{render acl=$cnACL}
       <input type='text' id="cn" name="cn" size=25 maxlength=60 value="{$cn}" title="{t}POSIX name of the group{/t}">
{/render}
{/if}
     </td>
    </tr>
    <tr>
     <td>
      <LABEL for="description">{t}Description{/t}</LABEL>
     </td>
     <td>
{render acl=$descriptionACL checkbox=$multiple_support checked=$use_description}
      <input type='text' id="description" name="description" size=40 maxlength=80 value="{$description}" title="{t}Descriptive text for this group{/t}">
{/render}
     </td>
    </tr>
    <tr>
     <td colspan=2> 
      <div style="height:15px;"></div> 
     </td>
    </tr>
    <tr>
     <td>
      <LABEL for="base">{t}Base{/t}</LABEL>{$must}
     </td>
     <td>
{render acl=$baseACL checkbox=$multiple_support checked=$use_base}
       {$base}
{/render}
     </td>
    </tr>
    <tr>
      <td colspan=2><hr></td>
    </tr>
    <tr>
      <td colspan=2> <div style="height:15px; width:100%;"></div> </td>
    </tr>
{if $multiple_support}

{else}
    <tr>
     <td colspan=2>
{render acl=$gidNumberACL}
      <input type=checkbox name="force_gid" value="1" title="{t}Normally IDs are auto-generated, select to specify manually{/t}" 
	{$force_gid} onclick="changeState('gidNumber')">
{/render}
	<LABEL for="gidNumber">{t}Force GID{/t}</LABEL>
      &nbsp;
{render acl=$gidNumberACL}
      <input type='text' name="gidNumber" size=9 maxlength=9 id="gidNumber" {$forceMode} value="{$gidNumber}" title="{t}Forced ID number{/t}">
{/render}
     </td>
    </tr>
{/if}

{if $multiple_support}
    <tr>
    <td colspan=2>
		{render acl=$sambaGroupTypeACL checkbox=$multiple_support checked=$use_smbgroup}
			<input class="center" type=checkbox name="smbgroup" value="1" {$smbgroup}>{t}Select to create a samba conform group{/t}
		{/render}
	</td>
	</tr>
	<tr>
	<td colspan=2>
		{render acl=$sambaGroupTypeACL checkbox=$multiple_support checked=$use_groupType}
			<select size="1" name="groupType">
				{html_options options=$groupTypes selected=$groupType}
			</select>
		{/render}
      &nbsp;
      <LABEL for="">{t}in domain{/t}</LABEL>
      &nbsp;

		{render acl=$sambaDomainNameACL checkbox=$multiple_support checked=$use_sambaDomainName}
			<select id="sambaDomainName" size="1" name="sambaDomainName">
		   		{html_options values=$sambaDomains output=$sambaDomains selected=$sambaDomainName}
		  	</select>
		{/render}
	</td>
	</tr>

{else}
    <tr>
     <td colspan=2>
{render acl=$sambaGroupTypeACL}
      <input type=checkbox name="smbgroup" value="1" {$smbgroup}  title="{t}Select to create a samba conform group{/t}">
{/render}
{render acl=$sambaGroupTypeACL}
      <select size="1" name="groupType">
       {html_options options=$groupTypes selected=$groupType}
      </select>
{/render}
      &nbsp;
      <LABEL for="">{t}in domain{/t}</LABEL>
      &nbsp;
{render acl=$sambaDomainNameACL}
      <select id="sambaDomainName" size="1" name="sambaDomainName">
       {html_options values=$sambaDomains output=$sambaDomains selected=$sambaDomainName}
      </select>
{/render}
     </td>
    </tr>
    {/if}

	{if $pickupGroup == "true"}
    <tr>
      <td colspan=2><hr></td>
    </tr>
    <tr>
      <td colspan=2> <div style="height:15px; width:100%;"></div> </td>
    </tr>
    <tr>
     <td colspan=2>
{render acl=$fonGroupACL checkbox=$multiple_support checked=$use_fon_group}
      <input class="center" type=checkbox name="fon_group" value="1" {$fon_group}>{t}Members are in a phone pickup group{/t}
{/render}
     </td>
    </tr>
	{/if}
	{if $nagios == "true"}
    <tr>
      <td colspan=2><hr></td>
    </tr>
    <tr>
      <td colspan=2> <div style="height:15px; width:100%;"></div> </td>
    </tr>
    <tr>
     <td colspan=2>
{render acl=$nagiosGroupACL checkbox=$multiple_support checked=$use_nagios_group}
      <input class="center" type=checkbox name="nagios_group" value="1" {$nagios_group}>{t}Members are in a nagios group{/t}
{/render}
     </td>
    </tr>
	{/if}
    <tr>
      <td colspan=2><hr></td>
    </tr>
    <tr>
      <td colspan=2> <div style="height:15px; width:100%;"></div> </td>
    </tr>
    <tr>
      <td colspan=2>{$trustModeDialog}</td>
    </tr>
   </table>

  </td>
  <td class='left-border'>

   &nbsp;
  </td>

  <td>

   <table summary="" style="width:100%">
    <tr>
     <td style="width:50%">
	{if $multiple_support}
        <h3>{t}Common group members{/t}</h3>
        {render acl=$memberUidACL}
            {$commonList}
        {/render}
        {render acl=$memberUidACL}
          <button type='submit' name='edit_membership'>{msgPool type=addButton}</button>
        {/render}
        
        <br>
        <h3>{t}Partial group members{/t}</h3>
        {render acl=$memberUidACL}
            {$partialList}
        {/render}
	{else}
        <h3>{t}Group members{/t}</h3>
        {render acl=$memberUidACL}
            {$memberList}
        {/render}
        {render acl=$memberUidACL}
          <button type='submit' name='edit_membership'>{msgPool type=addButton}</button>
        {/render}
	{/if}
     </td>
    </tr> 
   </table>
  </td>

 </tr>
</table>

<input type="hidden" name="groupedit" value="1">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('cn');
  -->
</script>
