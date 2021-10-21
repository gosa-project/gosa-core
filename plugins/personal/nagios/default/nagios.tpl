<div class="nagios-wrapper">
    <div class="row">
        <div class="col s12 xl6">
            <h3>{t}Nagios Account{/t}</h3>

            {render acl=$NagiosAliasACL}
            <div class="input-field">
                <input type='text' id="NagiosAlias" name="NagiosAlias" size=25 maxlength=65 value="{$NagiosAlias}">
                <label for="NagiosAlias">{t}Alias{/t}{$must}</label>
            </div>
            {/render}

            {render acl=$NagiosMailACL}
            <div class="input-field">
                <input type='text' id="NagiosMail" name="NagiosMail" size=25 maxlength=65 value="{$NagiosMail}">
                <label for="NagiosMail">{t}Mail address{/t}{$must}</label>
            </div>
            {/render}

            {render acl=$HostNotificationPeriodACL}
            <div class="input-field">
                <select name="HostNotificationPeriod" id="HostNotificationPeriod" size=1>
                    {html_options options=$HostNotificationPeriodValues values=$HostNotificationPeriodValues selected=$HostNotificationPeriod}
                </select>
                <label for="HostNotificationPeriod">{t}Host notification period{/t}{$must}</label>
            </div>
            {/render}

            {render acl=$ServiceNotificationPeriodACL}
            <div class="input-field">
                <select name="ServiceNotificationPeriod" id="ServiceNotificationPeriod" size=1>
                    {html_options options=$ServiceNotificationPeriodValues values=$ServiceNotificationPeriodValues selected=$ServiceNotificationPeriod}
                </select>
                <label for="ServiceNotificationPeriod">{t}Service notification period{/t}{$must}</label>
            </div>
            {/render}

            {render acl=$ServiceNotificationOptionsACL}
            <div class="input-field">
                <select name="ServiceNotificationOptions" id="ServiceNotificationOptions" size=1>
                    {html_options options=$ServiceNotificationOptionsValues values=$ServiceNotificationOptionsValues selected=$ServiceNotificationOptions}
                </select>
                <label for="ServiceNotificationOptions">{t}Service notification options{/t}{$must}</label>
            </div>
            {/render}

            {render acl=$HostNotificationOptionsACL}
            <div class="input-field">
                <select name="HostNotificationOptions" id="HostNotificationOptions" size=1>
                    {html_options options=$HostNotificationOptionsValues values=$HostNotificationOptionsValues selected=$HostNotificationOptions}
                </select>
                <label for="HostNotificationOptions">{t}Host notification options{/t}{$must}</label>
            </div>
            {/render}

            {render acl=$NagiosPagerACL}
            <div class="input-field">
                <input type='text' id="NagiosPager" name="NagiosPager" size=25 maxlength=65 value="{$NagiosPager}">
                <label for="NagiosPager">{t}Pager{/t}</label>
            </div>
            {/render}

            {render acl=$ServiceNotificationCommandsACL}
            <div class="input-field">
                <input type='text' id="ServiceNotificationCommands" disabled name="ServiceNotificationCommands" size=25 maxlength=65 value="{$ServiceNotificationCommands}">
                <label for="ServiceNotificationCommands">{t}Service notification commands{/t}</label>
            </div>
            {/render}

            {render acl=$HostNotificationCommandsACL}
            <div class="input-field">
                <input type='text' id="HostNotificationCommands" disabled name="HostNotificationCommands" size=25 maxlength=65 value="{$HostNotificationCommands}">
                <label for="HostNotificationCommands">{t}Host notification commands{/t}</label>
            </div>
            {/render}
        </div>

        <div class="col s12 xl6">
            <h3>{t}Nagios authentication{/t}</h3>

            {render acl=$AuthorizedSystemInformationACL}
            <label>
                <p>
                    <input type="checkbox" name="AuthorizedSystemInformation" value="1" {$AuthorizedSystemInformationCHK}>
                    <span>{t}view system informations{/t} </span>
                </p>
            </label>
            {/render}

            {render acl=$AuthorizedConfigurationInformationACL}
            <label>
                <p>
                    <input type="checkbox" name="AuthorizedConfigurationInformation" value="1" {$AuthorizedConfigurationInformationCHK}>
                    <span>{t}view configuration information{/t}</span>
                </p>
            </label>
            {/render}

            {render acl=$AuthorizedSystemCommandsACL}
            <label>
                <p>
                    <input type="checkbox" name="AuthorizedSystemCommands" value="1" {$AuthorizedSystemCommandsCHK}>
                    <span>{t}trigger system commands{/t}</span>
                </p>
            </label>
            {/render}

            {render acl=$AuthorizedAllServicesACL}
            <label>
                <p>
                    <input type="checkbox" name="AuthorizedAllServices" value="1" {$AuthorizedAllServicesCHK}>
                    <span>{t}view all services{/t}</span>
                </p>
            </label>
            {/render}

            {render acl=$AuthorizedAllHostsACL}
            <label>
                <p>
                    <input type="checkbox" name="AuthorizedAllHosts" value="1" {$AuthorizedAllHostsCHK}>
                    <span>{t}view all hosts{/t}</span>
                </p>
            </label>
            {/render}

            {render acl=$AuthorizedAllServiceCommandsACL}
            <label>
                <p>
                    <input type="checkbox" name="AuthorizedAllServiceCommands" value="1" {$AuthorizedAllServiceCommandsCHK}>
                    <span>{t}trigger all service commands{/t}</span>
                </p>
            </label>
            {/render}

            {render acl=$AuthorizedAllHostCommandsACL}
            <label>
                <p>
                    <input type="checkbox" name="AuthorizedAllHostCommands" value="1" {$AuthorizedAllHostCommandsCHK}>
                    <span>{t}trigger all host commands{/t}</span>
                </p>
            </label>
            {/render}
        </div>
    </div>
</div>

<input type="hidden" name="nagiosTab" value="nagiosTab">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
    // First input field on page
    focus_field('NagiosAlias');
</script>
