{if $multiple_support}
<input type="hidden" value="1" name="group_mulitple_edit">
{/if}

<div class="row">
    <div class="col s6">
        <div class="card">
            <div class="card-content">
                <div class="input-field">
                    {if $multiple_support}
                    <input type='text' id="dummy1" name="dummy1" size=25 maxlength=60 value="{t}Multiple edit{/t}"
                        disabled>
                    {else}
                    {render acl=$cnACL}
                    <input type='text' id="cn" name="cn" size=25 maxlength=60 value="{$cn}"
                        title="{t}POSIX name of the group{/t}">
                    {/render}
                    {/if}
                    <label for="cn">{t}Group name{/t}{$must}</label>
                </div>

                <div class="input-field">
                    {render acl=$descriptionACL checkbox=$multiple_support checked=$use_description}
                    <input type='text' id="description" name="description" size=40 maxlength=80 value="{$description}"
                        title="{t}Descriptive text for this group{/t}">
                    {/render}
                    <label for="description">{t}Description{/t}</label>
                </div>

                <div class="row">
                    {render acl=$baseACL checkbox=$multiple_support checked=$use_base}
                    {$base}
                    {/render}
                </div>

                {if !$multiple_support}

                <label>
                    {render acl=$gidNumberACL}
                    <input type=checkbox name="force_gid" value="1"
                        title="{t}Normally IDs are auto-generated, select to specify manually{/t}" {$force_gid}
                        onclick="changeState('gidNumber')">
                    {/render}
                    <span>{t}Force GID{/t}</span>
                </label>

                <div class="input-field inline">
                    {render acl=$gidNumberACL}
                    <input type='text' name="gidNumber" size=9 maxlength=9 id="gidNumber" {$forceMode}
                        value="{$gidNumber}" title="{t}Forced ID number{/t}">
                    {/render}
                </div>

                <div class="row">
                    <div class="col s6">
                        <label>
                            {render acl=$sambaGroupTypeACL}
                            <input type=checkbox name="smbgroup" value="1" {$smbgroup}
                                title="{t}Select to create a samba conform group{/t}">
                            {/render}
                            <span></span>
                        </label>

                        <div class="input-field inline">
                            {render acl=$sambaGroupTypeACL}
                            <select size="1" name="groupType">
                                {html_options options=$groupTypes selected=$groupType}
                            </select>
                            {/render}
                            <label></label>
                        </div>
                    </div>
                    <div class="col s6">
                        <div class="input-field">
                            {render acl=$sambaDomainNameACL}
                            <select id="sambaDomainName" size="1" name="sambaDomainName">
                                {html_options values=$sambaDomains output=$sambaDomains selected=$sambaDomainName}
                            </select>
                            {/render}
                            <label>{t}in domain{/t}</label>
                        </div>
                    </div>
                </div>
                

                

                


                {else}

                <div class="input-field">
                    <label>
                        {render acl=$sambaGroupTypeACL checkbox=$multiple_support checked=$use_smbgroup}
                        <input class="center" type=checkbox name="smbgroup" value="1" {$smbgroup}>
                        {/render}
                        <span>{t}Select to create a samba conform group{/t}</span>
                    </label>
                </div>

                <div class="input-field">
                    {render acl=$sambaGroupTypeACL checkbox=$multiple_support checked=$use_groupType}
                    <select size="1" name="groupType">
                        {html_options options=$groupTypes selected=$groupType}
                    </select>
                    {/render}
                    <label>{t}in domain{/t}</label>
                </div>

                <div class="input-field">
                    {render acl=$sambaDomainNameACL checkbox=$multiple_support checked=$use_sambaDomainName}
                    <select id="sambaDomainName" size="1" name="sambaDomainName">
                        {html_options values=$sambaDomains output=$sambaDomains selected=$sambaDomainName}
                    </select>
                    {/render}
                    <label>{t}Domain name{/t}</label>
                </div>

                {/if}

                {if $pickupGroup == "true"}
                <div class="input-field">
                    <label>
                        {render acl=$fonGroupACL checkbox=$multiple_support checked=$use_fon_group}
                        <input class="center" type=checkbox name="fon_group" value="1" {$fon_group}>
                        {/render}
                        <span>{t}Members are in a phone pickup group{/t}</span>
                    </label>
                </div>
                {/if}

                {if $nagios == "true"}
                <div class="input-field">
                    {render acl=$nagiosGroupACL checkbox=$multiple_support checked=$use_nagios_group}
                    <input class="center" type=checkbox name="nagios_group" value="1" {$nagios_group}>
                    {/render}
                    <span>{t}Members are in a Nagios group{/t}</span>
                </div>
                {/if}
            </div>
        </div>

        {$trustModeDialog}
    </div>

    <div class="col s6">
        {if $restrictedByDynGroup}
        <div class="card">
            <div class="card-content">
                <span class="card-title">{t}The group members are part of a dyn-group and cannot be managed!{/t}</span>
                {if $multiple_support}
                {render acl=$memberACL}
                {$commonList}
                {/render}
                {else}
                {render acl=$memberACL}
                {$memberList}
                {/render}
                {/if}
            </div>
        </div>

        {else}

        {if $multiple_support}
        <div class="card">
            <div class="card-content">
                <span class="card-title">{t}Common group members{/t}</span>
                {render acl=$memberACL}
                {$commonList}
                {/render}
                {render acl=$memberACL}
                <button class="btn-small gonicus-color" type='submit' name='edit_membership'>{msgPool
                    type=addButton}</button>
                {/render}
            </div>
        </div>

        <div class="card">
            <div class="card-content">
                <span class="card-title">{t}Partial group members{/t}</span>
                {render acl=$memberACL}
                {$partialList}
                {/render}
            </div>
        </div>

        {else}

        <div class="card">
            <div class="card-content">
                <span class="card-title">{t}Group members{/t}</span>
                {render acl=$memberACL}
                {$memberList}
                {/render}
                {render acl=$memberACL}
                <button class="btn-small gonicus-color" type='submit' name='edit_membership'>{msgPool
                    type=addButton}</button>
                {/render}
            </div>
        </div>
        {/if}

        {/if}
    </div>
</div>

<input type="hidden" name="groupedit" value="1">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
    // First input field on page
    focus_field('cn');
</script>

<script type="text/javascript">
    document.addEventListener('DOMContentLoaded', function() {
        var elems = document.querySelectorAll('select');
        var instances = M.FormSelect.init(elems, {});
      });
</script>