<div class="samba-wrapper">
    <div class="row">
        <div class="col s12 xl6">
            <h3>{t}Generic{/t}</h3>

            {render acl=$sambaHomePathACL checkbox=$multiple_support checked=$use_sambaHomePath}
            <div class="input-field">
                <input type='text' id="sambaHomePath" name="sambaHomePath" size=30 maxlength=60 value="{$sambaHomePath}">
                <label for="sambaHomePath">{t}Home directory{/t}</label>
            </div>
            {/render} 
            
            {render acl=$sambaHomeDriveACL checkbox=$multiple_support checked=$use_sambaHomeDrive}
            <div class="input-field">
                <select size="1" name="sambaHomeDrive">
                    {html_options values=$drives output=$drives selected=$sambaHomeDrive}
                </select>
            </div>
            {/render} 
            
            {render acl=$sambaDomainNameACL checkbox=$multiple_support checked=$use_sambaDomainName}
            <div class="input-field add">
                <select id="sambaDomainName" size="1" name="sambaDomainName" onChange="document.mainform.submit();">
                 {html_options values=$domains output=$domains selected=$sambaDomainName}
                </select>
                <label for="sambaDomainName">{t}Domain{/t}</label>
                <button class="btn-small" type='submit' name='display_information'>{t}Show information{/t}</button>
            </div>
            {/render}

        </div>

        <div class="col s12 xl6">
            <h3>&nbsp;</h3>
            {render acl=$sambaLogonScriptACL checkbox=$multiple_support checked=$use_sambaLogonScript}
            <div class="input-field">
                <input type='text' id="sambaLogonScript" name="sambaLogonScript" size=35 maxlength=60 value="{$sambaLogonScript}">
                <label for="">{t}Script path{/t}</label>
            </div>
            {/render} 
            
            {render acl=$sambaProfilePathACL checkbox=$multiple_support checked=$use_sambaProfilePath}
            <div class="input-field">
                <label for="">{t}Profile path{/t}</label>
                <input type='text' class="center" id="sambaProfilePath" name="sambaProfilePath" size=35 maxlength=60 value="{$sambaProfilePath}">
            </div>
            {/render}
        </div>
    </div>
    
    <hr class="divider">

    <div class="row">
        <div class="col s12 xl6">
            <h3>{t}Terminal Server{/t}</h3>

            {if $multiple_support}
            <label>
                <input class="center" type=checkbox name="use_tslogin" id="use_tslogin" value="1" {if $use_tslogin} checked {/if} onClick="changeState('tslogin')">
                <span></span>
            </label>
            <label>
                <input class="center" type=checkbox name="tslogin" id="tslogin" value="1" {$tslogin}{if !$use_tslogin} disabled {/if}>
                <span></span>
            </label> 
            {else} 
            
            {render acl=$AllowLoginOnTerminalServerACL}
            <label>
                <input class="center" type=checkbox name="tslogin" id="tslogin" value="1" {$tslogin} onclick="
                changeState('CtxWFHomeDir');
                changeState('CtxWFHomeDirDrive');
                changeState('CtxWFProfilePath');
                changeState('inherit');			
                changeTripleSelectState_2nd_neg('tslogin','inherit','CtxInitialProgram');
                    changeTripleSelectState_2nd_neg('tslogin','inherit','CtxWorkDirectory');
                    changeState('CtxMaxConnectionTimeF');
                    changeState('CtxMaxDisconnectionTimeF');
                    changeState('CtxMaxIdleTimeF');
                    changeTripleSelectState('tslogin','CtxMaxConnectionTimeF','CtxMaxConnectionTime');
                    changeTripleSelectState('tslogin','CtxMaxDisconnectionTimeF','CtxMaxDisconnectionTime');
                    changeTripleSelectState('tslogin','CtxMaxIdleTimeF','CtxMaxIdleTime');
                    changeState('connectclientdrives');
                    changeState('connectclientprinters');
                    changeState('defaultprinter');
                    changeState('shadow');
                    changeState('brokenconn');
                    changeState('reconn');
                "> 
                <span>{t}Allow login on terminal server{/t}</span>
            </label> 
            {/render} 
            {/if} 
            
            {render acl=$AllowLoginOnTerminalServerACL checkbox=$multiple_support checked=$use_CtxWFHomeDir}
            <div class="input-field">
                <input type='text' id="CtxWFHomeDir" name="CtxWFHomeDir" size=30 maxlength=60 value="{$CtxWFHomeDir}" {$tsloginstate}>
                <label for="CtxWFHomeDir">{t}Home directory{/t}</label>
            </div>
            {/render} 
            
            {render acl=$AllowLoginOnTerminalServerACL checkbox=$multiple_support checked=$use_CtxWFHomeDirDrive}
            <div class="input-field">
                <select size="1" id="CtxWFHomeDirDrive" name="CtxWFHomeDirDrive" {$tsloginstate}>
                    {html_options values=$drives output=$drives selected=$CtxWFHomeDirDrive}
                </select>
            </div>
            {/render} 
            
            {render acl=$AllowLoginOnTerminalServerACL checkbox=$multiple_support checked=$use_CtxWFProfilePath}
            <div class="input-field">
                <input type='text' id="CtxWFProfilePath" name="CtxWFProfilePath" size=35 maxlength=60 value="{$CtxWFProfilePath}" {$tsloginstate}>
                <label for="CtxWFProfilePath">{t}Profile path{/t}</label>
            </div>
            {/render}
        </div>
        
        <div class="col s12 xl6">
            <h3>&nbsp;</h3>
            {render acl=$AllowLoginOnTerminalServerACL checkbox=$multiple_support checked=$use_inherit}
            <label>
                <input class="center" type=checkbox id="inherit" name="inherit" {if $inheritstate} checked {/if} {$tsloginstate} 
                onClick="changeState('CtxInitialProgram'); changeState('CtxWorkDirectory');"> 
                <span>{t}Inherit client configuration{/t}</span>
            </label> 
            {/render}

            {render acl=$AllowLoginOnTerminalServerACL checkbox=$multiple_support checked=$use_CtxInitialProgram}
            <div class="input-field">
                <input type='text' id="CtxInitialProgram" name="CtxInitialProgram" size=35 maxlength=60 value="{$CtxInitialProgram}" {$inheritstate} {$tsloginstate}>
                <label for="CtxInitialProgram">{t}Initial program{/t}</label>
            </div>
            {/render}

            {render acl=$AllowLoginOnTerminalServerACL checkbox=$multiple_support checked=$use_CtxWorkDirectory}
            <div class="input-field">
                <input type='text' id="CtxWorkDirectory" name="CtxWorkDirectory" size=35 maxlength=60 value='{$CtxWorkDirectory}' {$inheritstate} {$tsloginstate}> 
                <label for="CtxWorkDirectory">{t}Working directory{/t}</label>
            </div>
            {/render}
            
            

        </div>
    
    </div>

    <div class="row">
        <div class="col s6 xl3">
            <h3>{t}Timeout settings (in minutes){/t}</h3>
            {if $multiple_support}
            <label>
                <input type="checkbox" name="use_CtxMaxConnectionTimeF" {if $use_CtxMaxConnectionTimeF} checked {/if} onClick="changeState('CtxMaxConnectionTimeF');" class="center"> 
                <span></span>
            </label>
            {/if} 

            <div class="check-input-wrapper">
                {render acl=$AllowLoginOnTerminalServerACL}
                <label>
                    <input id="CtxMaxConnectionTimeF" type="checkbox" class="center" name="CtxMaxConnectionTimeF" {if !$use_CtxMaxConnectionTimeF && $multiple_support} disabled {/if} value="1" {$CtxMaxConnectionTimeF} onclick="changeState('CtxMaxConnectionTime')" {$tsloginstate}>                            
                    <span>{t}Connection{/t}</span>
                </label>
                {/render}
                
                {render acl=$AllowLoginOnTerminalServerACL}
                <div class="input-field ">
                    <input name="CtxMaxConnectionTime" type="text" id="CtxMaxConnectionTime" size=5 maxlength=5 value="{$CtxMaxConnectionTime}" {if !$CtxMaxConnectionTimeF || $tsloginstate=="disabled" } disabled {/if}> 
                </div>
                {/render}
            </div>

            <div class="check-input-wrapper">
                {render acl=$AllowLoginOnTerminalServerACL checkbox=$multiple_support checked=$use_CtxMaxDisconnectionTimeF}
                <label>
                    <input id="CtxMaxDisconnectionTimeF" type=checkbox name="CtxMaxDisconnectionTimeF" value="1" {$CtxMaxDisconnectionTimeF} onclick="changeState('CtxMaxDisconnectionTime')" {$tsloginstate} class="center"> 
                    <span>{t}Disconnection{/t}</span>
                </label>
                {/render}
                
                {render acl=$AllowLoginOnTerminalServerACL}
                <div class="input-field ">
                    <input name="CtxMaxDisconnectionTime" id="CtxMaxDisconnectionTime" type="text" size=5 maxlength=5 value="{$CtxMaxDisconnectionTime}" {if $tsloginstate=="disabled" || !$CtxMaxDisconnectionTimeF} disabled {/if}> 
                </div>
                {/render}
            </div>

            <div class="check-input-wrapper">
                {render acl=$AllowLoginOnTerminalServerACL checkbox=$multiple_support checked=$use_CtxMaxIdleTimeF}
                <label>
                    <input id="CtxMaxIdleTimeF" type=checkbox name="CtxMaxIdleTimeF" value="1" {$CtxMaxIdleTimeF} onclick="changeState('CtxMaxIdleTime')" {$tsloginstate} class="center"> 
                    <span>{t}IDLE{/t}</span>
                </label>
                {/render}
                
                {render acl=$AllowLoginOnTerminalServerACL}
                <div class="input-field ">
                    <input name="CtxMaxIdleTime" id="CtxMaxIdleTime" size=5 maxlength=5 type="text" value="{$CtxMaxIdleTime}" {if !$CtxMaxIdleTimeF || $tsloginstate=="disabled" } disabled {/if}> 
                </div>
                {/render}
            </div>
        </div>

        <div class="col s6 xl3">
            <h3>{t}Client devices{/t}</h3>
            
            {render acl=$AllowLoginOnTerminalServerACL checkbox=$multiple_support checked=$use_connectclientdrives}
            <label>
                <input id="connectclientdrives" type=checkbox name="connectclientdrives" value="1" {$connectclientdrives} {$tsloginstate} class="center"> 
                <span>{t}Connect client drives at log on{/t}</span>
            </label>
            {/render}

            {render acl=$AllowLoginOnTerminalServerACL checkbox=$multiple_support checked=$use_connectclientprinters}
            <label>
                <input id="connectclientprinters" type=checkbox name="connectclientprinters" value="1" {$connectclientprinters}{$tsloginstate} class="center"> 
                <span>{t}Connect client printers at log on{/t}</span>
            </label>
            {/render}
                    
            {render acl=$AllowLoginOnTerminalServerACL checkbox=$multiple_support checked=$use_defaultprinter}
            <label>
                <input id="defaultprinter" type=checkbox name="defaultprinter" value="1" {$defaultprinter} {$tsloginstate} class="center"> 
                <span>{t}Default to main client printer{/t}</span>
            </label>
            {/render}
        </div>
    
        <div class="col s12 xl6">
            <h3>{t}Miscellaneous{/t}</h3>

            {render acl=$AllowLoginOnTerminalServerACL checkbox=$multiple_support checked=$use_shadow}
            <div class="input-field">
                <select id="shadow" size="1" name="shadow" {$tsloginstate}>
                    {html_options options=$shadow selected=$shadowmode}
                </select> 
                <label>{t}Shadowing{/t}</label>
            </div>
            {/render}

            {render acl=$AllowLoginOnTerminalServerACL checkbox=$multiple_support checked=$use_brokenconn}
            <div class="input-field">
                <select id="brokenconn" size="1" name="brokenconn" {$tsloginstate}>
                    {html_options options=$brokenconn selected=$brokenconnmode}
                </select> 
                <label>{t}On broken or timed out{/t}</label>
            </div>
            {/render}

            {render acl=$AllowLoginOnTerminalServerACL checkbox=$multiple_support checked=$use_reconn}
            <div class="input-field">
                <select id="reconn" size="1" name="reconn" {$tsloginstate}>
                    {html_options options=$reconn selected=$reconnmode}
                </select> 
                <label>{t}Reconnect if disconnected{/t}</label>
            </div>
            {/render}
        </div>
    </div>
        
    
    <hr class="divider">
            

    <div class="row">
        <div class="col s12 xl6">
            <h3>{t}Access options{/t}</h3>

            {render acl=$enforcePasswordChangeACL checkbox=$multiple_support checked=$use_enforcePasswordChange}
            <label>
                <input type='checkbox' value='1' name='flag_enforcePasswordChange' {if $flag_enforcePasswordChange} checked {/if} id='flag_enforcePasswordChange'> 
                <span>{t}Enforce password change{/t}</span>    
            </label>
            {/render}

            {render acl=$passwordNeverExpiresACL checkbox=$multiple_support checked=$use_passwordNeverExpires}
            <label>
                <input type='checkbox' value='1' name="flag_passwordNeverExpires" {if $flag_passwordNeverExpires} checked {/if} id='flag_passwordNeverExpires'> 
                <span>{t}The password never expires{/t}</span>
            </label>
            {/render}
                    
            {render acl=$noPasswordRequiredACL checkbox=$multiple_support checked=$use_noPasswordRequired}
            <label>
                <input type='checkbox' value='1' name="flag_noPasswordRequired" {if $flag_noPasswordRequired} checked {/if} id='flag_noPasswordRequired'> 
                <span>{t}Login from windows client requires no password{/t}</span>
            </label>
            {/render}

            {render acl=$temporaryDisabledACL checkbox=$multiple_support checked=$use_temporaryDisabled}
            <label>
                <input type='checkbox' value='1' name="flag_temporaryDisabled" {if $flag_temporaryDisabled} checked {/if} id='flag_temporaryDisabled'> 
                <span>{t}Lock samba account{/t}</span>
            </label>
            {/render}
                    
            {render acl=$cannotChangePasswordACL checkbox=$multiple_support checked=$use_cannotChangePassword}
            <label>
                <input type='checkbox' value='1' name='flag_cannotChangePassword' {if $flag_cannotChangePassword} checked {/if} id='flag_cannotChangePassword'> 
                <span>{t}Cannot change password{/t}</span>
            </label>
            {/render}

            <div class="check-input-wrapper">
                {render acl=$sambaKickoffTimeACL checkbox=$multiple_support checked=$use_sambaKickoffTime}
                <label>
                    <input id="flag_sambaKickoffTime" type=checkbox name="flag_sambaKickoffTime" value="1" {if $flag_sambaKickoffTime} checked {/if} class="center"> 
                    <span>{t}Account expires after{/t}</span>
                </label>
                {/render}

                {render acl=$sambaKickoffTimeACL}
                <div class="input-field inline">
                    <input type="text" id="lang" value="{$lang}" hidden="true">
                    <input type="text" id="sambaKickoffTime" name="sambaKickoffTime" class="datepicker" value="{$sambaKickoffTime}" style="width: 200px;"> 
                    <label for="sambaKickoffTime"></label>
                </div>
                {/render}
            </div>
        </div>

        <div class="col s12 xl6">
            <!-- /Samba policies -->
            
            {if $multiple_support}
            <label>
                <input type="checkbox" name="use_workstation_list" {if $use_workstation_list} checked {/if} class="center" onClick="changeState('workstation_list');">
                <span></span>
            </label>

            <div class="input-field add">
                <select {if $multiple_support && !$use_workstation_list} disabled {/if} id="workstation_list" style="width:100%;" name="workstation_list[]" size=10 multiple>
                    {foreach from=$multiple_workstations item=item key=key}
                    {if $item.UsedByAllUsers}
                    <option value="{$key}">{$item.Name} ({t}Used by all users{/t})</option>
                    {else}
                    <option style='color: #888888; background: #DDDDDD;background-color: #DDDDDD;' value="{$key}">{$item.Name} ({t}Used by some users{/t})</option>
                    {/if}
                    {/foreach}
                </select>
                <label for="workstation_list">{t}Allow connection from these workstations only{/t}</label>
                <button class="btn-small" type='submit' name='add_ws'>{msgPool type=addButton}</button>
                <button class="btn-small" type='submit' name='delete_ws'>{msgPool type=delButton}</button> 
            </div>
            {else} 
            
            {render acl=$sambaUserWorkstationsACL}
            <div class="input-field add">
                <select id="workstation_list" style="width:100%;" name="workstation_list[]" size=5 multiple>
                    {html_options values=$workstations output=$workstations}
                </select> 
                <label for="workstation_list">{t}Allow connection from these workstations only{/t}</label>
                <button class="btn-small" type='submit' name='add_ws'>{msgPool type=addButton}</button> 
                <button class="btn-small" type='submit' name='delete_ws'>{msgPool type=delButton}</button> 
            </div>
            {/render}
            {/if}
        </div>
    </div>
    
    <hr class="divider">
    
    <div class="row">
        <div class="col s12 xl6">
            {if $additional_info_PwdMustChange}
            <p>({$additional_info_PwdMustChange})</p> 
            {/if}
            
            {render acl=$sambaLogonHoursACL mode=read_active checkbox=$multiple_support checked=$use_SetSambaLogonHours} 
            <div class="headline-button-wrapper">
                <h3>{t}Samba log on times{/t}</h3>
                <button class="btn-small" type='submit' name='SetSambaLogonHours'>{t}Edit settings...{/t}</button>
            </div>
            {/render}
        </div>
    </div>
</div>

<input type="hidden" name="sambaTab" value="sambaTab">
