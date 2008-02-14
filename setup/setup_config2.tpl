<div class='default' style='margin:12px; '>  
    <p><b>{t}Samba settings{/t}</b></p>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Samba hash generator{/t}
        </div>
        <div class='step4_value'>
                <input size=40 id='smbhash' name='smbhash' type='text' value='{$samba_settings.smbhash}'  >
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Samba SID{/t}
        </div>
        <div class='step4_value'>
            {if $samba_settings.samba_sid_active == FALSE}
                <input type='checkbox' value='1' name='samba_sid_active'
                    onClick='changeState("samba_sid");'>
                <input size=20 id='samba_sid' name='samba_sid' type='text' value='{$samba_settings.samba_sid}' disabled>
            {else}
                <input type='checkbox' value='1' name='samba_sid_active' checked {if !$sambaDomain_found} disabled {/if}>
                <input size=20 id='samba_sid' name='samba_sid' type='text' value='{$samba_settings.samba_sid}'  >
            {/if}
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}RID base{/t}
        </div>
        <div class='step4_value'>
            {if $samba_settings.samba_rid_active == FALSE}
                <input type='checkbox' value='1' name='samba_rid_active'
                    onClick='changeState("samba_rid");'>
                <input size=20 id='samba_rid' name='samba_rid' type='text' value='{$samba_settings.samba_rid}' disabled>
            {else}
                <input type='checkbox' value='1' name='samba_rid_active' checked {if !$sambaDomain_found} disabled {/if}>
                <input size=20 id='samba_rid' name='samba_rid' type='text' value='{$samba_settings.samba_rid}'  >
            {/if}
        </div>
    </div>
    <div class='step4_container'>
        <div class='step4_name'>
            {t}Workstation container{/t}
        </div>
        <div class='step4_value'>
            {if $generic_settings.wws_ou_active == FALSE}
                <input type='checkbox' value='1' name='wws_ou_active'
                    onClick='changeState("wws_ou");'>
                <input size=40 id='wws_ou' name='wws_ou' type='text' value='{$generic_settings.wws_ou}' disabled>
            {else}
                <input type='checkbox' value='1' name='wws_ou_active' checked>
                <input size=40 id='wws_ou' name='wws_ou' type='text' value='{$generic_settings.wws_ou}'  >
            {/if}
        </div>
    </div>
    <div class='step4_container'>
        <div class='step4_name'>
            {t}Samba SID mapping{/t}
        </div>
        <div class='step4_value'>
            <select name="sambaidmapping" size="1" title="">
			{html_options options=$bool selected=$sambaidmapping}
            </select>
        </div>
    </div>
	<div class='step4_container'>
        <div class='step4_name'>
            {t}Timezone{/t}
        </div>
        <div class='step4_value'>
            <select name='timezone' title='{t}Please choose your preferred timezone here{/t}' style="width:100%">
        {foreach from=$timezones.TIMEZONES item=val key=key}
            {if $key == $timezone}
				{if isset($timezones.DST_ZONES.$key)}
            		<option selected value="{$key}">{$key} (DST)</option>
				{else}
            		<option selected value="{$key}">{$key} (GMT)</option>
				{/if}
            {else}
				{if isset($timezones.DST_ZONES.$key)}
            		<option value="{$key}">{$key} (DST)</option>
				{else}
            		<option value="{$key}">{$key} (GMT)</option>
				{/if}
            {/if}
        {/foreach}
            </select>
        </div>
    </div>


    <p><b>{t}Additional GOsa settings{/t}</b></p>

	<div class='step4_container'>
        <div class='step4_name'>
            {t}Enable Copy & Paste{/t}
        </div>
        <div class='step4_value'>

            <select name="enableCopyPaste" size="1" title="">
                {html_options options=$bool selected=$generic_settings.enableCopyPaste}
            </select>
        </div>
    </div>

	<div class='step4_container'>
        <div class='step4_name'>
            {t}Enable DNS extension{/t}
        </div>
        <div class='step4_value'>

            <select name="enableDNS" size="1" title="">
                {html_options options=$bool selected=$generic_settings.enableDNS}
            </select>
        </div>
    </div>

	<div class='step4_container'>
        <div class='step4_name'>
            {t}Enable DHCP extension{/t}
        </div>
        <div class='step4_value'>

            <select name="enableDHCP" size="1" title="">
                {html_options options=$bool selected=$generic_settings.enableDHCP}
            </select>
        </div>
    </div>

	<div class='step4_container'>
        <div class='step4_name'>
            {t}Enable mime type management{/t}
        </div>
        <div class='step4_value'>

            <select name="enableMimeType" size="1" title="">
                {html_options options=$bool selected=$enableMimeType}
            </select>
        </div>
    </div>

	<div class='step4_container'>
        <div class='step4_name'>
            {t}Enable FAI release management{/t}
        </div>
        <div class='step4_value'>

            <select name="enableFAI_management" size="1" title="">
                {html_options options=$bool selected=$enableFAI_management}
            </select>
        </div>
    </div>

	<div class='step4_container'>
        <div class='step4_name'>
            {t}Enable user netatalk plugin{/t}
        </div>
        <div class='step4_value'>
            <select name="use_netatalk" size="1" title="">
                {html_options options=$bool selected=$use_netatalk}
            </select>
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Government mode{/t}
        </div>
        <div class='step4_value'>
            <select name="governmentmode" size="1" title="">
			{html_options options=$bool selected=$governmentmode}
			</select>
        </div>
    </div>

    <p><b>{t}Mail settings{/t}</b></p>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Mail method{/t}
        </div>
        <div class='step4_value'>
            <select name="mail" size="1" title="" onChange="document.mainform.submit();">
				<option  value="disabled">{t}disabled{/t}</option>
				{foreach from=$mail_methods item=item}
					<option value="{$item.name}" {if $mail == $item.name} selected {/if}>{$item.name}&nbsp;({$item.uattrib})</option>
				{/foreach}
            </select>
        </div>
        <div class='step4_status'>
        </div>
    </div>

	<div class='step4_container'>
	    <div class='step4_name'>
            {t}Account identification attribute{/t}
        </div>
        <div class='step4_value'>
            <select name="mail_attrib" size="1" {if $mail == "disabled"} disabled {/if}>
                <option value="mail" {if $mail_attrib == "mail"} selected {/if} >mail</option>
                <option value="uid" {if $mail_attrib == "uid"} selected {/if} >uid</option>
            </select>
        </div>
        <div class='step4_status'>
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Vacation templates{/t}
        </div>
        <div class='step4_value'>
            {if $mail_settings.vacationdir_active == FALSE}
                <input type='checkbox' value='1' name='vacationdir_active'
                    onClick='changeState("vacationdir");'>
                <input size=40 id='vacationdir' name='vacationdir' type='text' value='{$mail_settings.vacationdir}' disabled>
            {else}
                <input type='checkbox' value='1' name='vacationdir_active' checked>
                <input size=40 id='vacationdir' name='vacationdir' type='text' value='{$mail_settings.vacationdir}'  >
            {/if}
        </div>
    </div>

    <div class='step4_container'>
        <div class='step4_name'>
            {t}Use Cyrus UNIX style{/t}
        </div>
        <div class='step4_value'>
            <select name="cyrusunixstyle" size="1" title="">
            {html_options options=$bool selected=$cyrusunixstyle}
            </select>
        </div>
    </div>

</div>

<input type='hidden' value='1' name='step6_posted'>
<div style="clear:both"></div>
