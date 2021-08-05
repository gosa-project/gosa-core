<div class="row">
   <div class="col s6">
      <div class="card">
         <div class="card-content">
            <span class="card-title">{t}Generic{/t}</span>
            {if !$multiple_support}
            <div class="input-field col">
               {if !$isModifyableMail && $initially_was_account && !$is_template}
               <input type='text' disabled size=30 value="{$mail}">
               {else}
               {if $domainSelectionEnabled}
               {render acl=$mailACL}
               <input type='text' id="mail" name="mail" size=20 maxlength=65 value="{$mail}">
               {/render}
               @<select name='MailDomain' size=1>
                  {html_options values=$MailDomains output=$MailDomains selected=$MailDomain}
               </select>
               {else}
               {render acl=$mailACL}
               <input type='text' id="mail" name="mail" size=35 maxlength=65 value="{$mail}">
               {/render}
               {/if}
               {/if}
               <label for="mail">{t}Primary address{/t}{$must}</label>
            </div>

            <div class="input-field col">
               {if !$isModifyableServer && $initially_was_account && !$is_template}
               <input type='text' disabled size=30 value="{$gosaMailServer}">
               {else}
               {render acl=$gosaMailServerACL}
               <select size="1" id="gosaMailServer" name="gosaMailServer"
                  title="{t}Specify the mail server where the user will be hosted on{/t}">
                  {html_options values=$MailServers output=$MailServers selected=$gosaMailServer}
                  <option disabled>&nbsp;</option>
               </select>
               <label>{t}Server{/t}</label>
               {/render}
               {/if}
            </div>
            {/if}

            {if $quotaEnabled}
            <div class="row">
               <div class="col s12">
                  <div class="col s6">{t}Quota usage{/t}</div>
                  <div class="col s6">{$quotaUsage}</div>
               </div>
            </div>
            {/if}

            <div class="input-field col">
               {render acl=$gosaMailQuotaACL checkbox=$multiple_support checked=$use_gosaMailQuota}
               <input type='text' id="gosaMailQuota" name="gosaMailQuota" size="6" align="middle" maxlength="60"
                  value="{$gosaMailQuota}">
               <label for="gosaMailQuota">{t}Quota size{/t}</label>
               <span class="helper-text">MB</span>
               {/render}
            </div>
         </div>
      </div>
   </div>

   <div class="col s6">
      {if !$multiple_support}
      <div class="card">
         <div class="card-content">
            <span class="card-title">{t}Alternative addresses{/t}</span>
            <div class="input-field">
               {render acl=$gosaMailAlternateAddressACL}
               <select id="alternates_list" style="width:100%;height:100px;" name="alternates_list[]" size="15" multiple
                  title="{t}List of alternative mail addresses{/t}">
                  {html_options values=$gosaMailAlternateAddress output=$gosaMailAlternateAddress}
                  <option disabled>&nbsp;</option>
               </select>
               {/render}
            </div>

            <div class="input-field">
               {render acl=$gosaMailAlternateAddressACL}
               <input type='text' name="alternate_address" size="30" align="middle" maxlength="65" value="">
               {/render}
            </div>
            {render acl=$gosaMailAlternateAddressACL}
            <button class="btn-small gonicus-color" type='submit' name='add_alternate'>{msgPool type=addButton}</button>
            {/render}
            {render acl=$gosaMailAlternateAddressACL}
            <button class="btn-small gonicus-color" type='submit' name='delete_alternate'>{msgPool
               type=delButton}</button>
            {/render}
         </div>
      </div>
      {/if}
   </div>

   <div class="col s12">
      {render acl=$gosaMailDeliveryModeCACL}
      <div class="input-field">
         <label>
            <input class="center" type=checkbox id="own_script" name="own_script" value="1" {$own_script} onClick="
               changeState('sieveManagement');
               changeState('drop_own_mails');
               changeState('use_vacation');
               changeState('use_spam_filter');
               changeState('use_mailsize_limit');
               changeState('import_vacation');
               changeState('vacation_template');
               changeState('only_local');
               changeState('gosaVacationMessage');
               changeState('gosaSpamSortLevel');
               changeState('gosaSpamMailbox');
               changeState('gosaMailMaxSize');
               changeStates();">
            <span>{t}Use custom sieve script{/t} <b>({t}disables all Mail options!{/t})</b></span>
            {if $allowSieveManagement}
            {render acl=$sieveManagementACL}
            <button class="btn-small gonicus-color" type='submit' name='sieveManagement' id="sieveManagement" {if
               $own_script=="" } disabled{/if}>
               {t}Sieve Management{/t}
            </button>
            {/render}
            {/if}
         </label>
      </div>
      {/render}
   </div>

   <div class="col s6">
      <div class="card">
         <div class="card-content">
            {render acl=$gosaMailDeliveryModeIACL checkbox=$multiple_support checked=$use_drop_own_mails}
            <p>
               <label>
                  <input {if $own_script !="" } disabled {/if} class="center" id='drop_own_mails' type=checkbox
                     name="drop_own_mails" value="1" {$drop_own_mails}
                     title="{t}Select if you want to forward mails without getting own copies of them{/t}" />
                  <span>{t}No delivery to own mailbox{/t}</span>
               </label>
            </p>

            {/render}

            {render acl=$gosaMailDeliveryModeVACL checkbox=$multiple_support checked=$use_use_vacation}
            <p>
               <label>
                  <input type=checkbox name="use_vacation" value="1" {$use_vacation} id="use_vacation" {if $own_script
                     !="" } disabled {/if}
                     title="{t}Select to automatically response with the vacation message defined below{/t}"
                     class="center" onclick="changeStates()">
                  <span>{t}Activate vacation message{/t}</span>
               </label>
               {/render}
            </p>

            {if $rangeEnabled}
            <div class="row">
               <div class="input-field col s3">
                  {render acl=$gosaVacationMessageACL}
                  <input type="text" id="gosaVacationStart" name="gosaVacationStart" class="datepicker"
                     value="{$gosaVacationStart}">
                  <label for="gosaVacationStart">{t}from{/t}</label>
                  {/render}
               </div>
               <div class="input-field col s3">
                  {render acl=$gosaVacationMessageACL}
                  <input type="text" id="gosaVacationStop" name="gosaVacationStop" class="datepicker"
                     value="{$gosaVacationStop}">
                  <label for="gosaVacationStop">{t}till{/t}</label>
                  {/render}
               </div>
            </div>
            {/if}

            {render acl=$gosaVacationMessageACL checkbox=$multiple_support checked=$use_gosaVacationMessage}
            <textarea {if $own_script !="" } disabled {/if} id="gosaVacationMessage" class="materialize-textare"
               name="gosaVacationMessage">
               {$gosaVacationMessage}
            </textarea>
            <b><label for="gosaVacationMessage">{t}Vacation message{/t}</label></b>
            {/render}

            {if $show_templates eq "true"}
            {render acl=$gosaVacationMessageACL}
            <select id='vacation_template' name="vacation_template" {if $own_script !="" } disabled {/if} size=1>
               {html_options options=$vacationtemplates selected=$template}
               <option disabled>&nbsp;</option>
            </select>
            {/render}
            {render acl=$gosaVacationMessageACL}
            <button type='submit' name='import_vacation' id="import_vacation" {if $own_script !="" } disabled {/if}>
               {t}Import{/t}
            </button>
            {/render}
            {/if}

         </div>
      </div>
   </div>

   <div class="col s6">
      <div class="card">
         <div class="card-content">
            <label>
               {render acl=$gosaMailDeliveryModeSACL checkbox=$multiple_support checked=$use_use_spam_filter}
               <input {if $own_script !="" } disabled {/if} id='use_spam_filter' type=checkbox name="use_spam_filter"
                  value="1" {$use_spam_filter}
                  title="{t}Select if you want to filter this mails through Spamassassin{/t}" class="center">
               <span></span>
               {/render}
            </label>

            <div class="input-field">
               {render acl=$gosaSpamSortLevelACL checkbox=$multiple_support checked=$use_gosaSpamSortLevel}
               <select {if $own_script !="" } disabled {/if} id="gosaSpamSortLevel" size="1" name="gosaSpamSortLevel"
                  title="{t}Choose SPAM level - smaller values are more sensitive{/t}">
                  {html_options values=$spamlevel output=$spamlevel selected=$gosaSpamSortLevel}
               </select>
               <label for="gosaSpamSortLevel">{t}Move mails tagged with SPAM level greater than{/t}</label>
               {/render}
            </div>

            <div class="input-field">
               {render acl=$gosaSpamMailboxACL checkbox=$multiple_support checked=$use_gosaSpamMailbox}
               <select {if $own_script !="" } disabled {/if} size="1" id="gosaSpamMailbox" name="gosaSpamMailbox">
                  {html_options values=$spambox output=$spambox selected=$gosaSpamMailbox}
                  <option disabled>&nbsp;</option>
               </select>
               <label for="gosaSpamMailbox">{t}to folder{/t}</label>
               {/render}
            </div>

            <div class="row valign-wrapper">
               <div class="col s6">
                  <label>
                     {render acl=$gosaMailDeliveryModeRACL checkbox=$multiple_support checked=$use_use_mailsize_limit}
                     <input {if $own_script !="" } disabled {/if} id='use_mailsize_limit' type=checkbox
                        name="use_mailsize_limit" value="1" {$use_mailsize_limit} class="center">
                     {/render}
                     <span>{t}Reject mails bigger than{/t}</span>
                  </label>
               </div>
               <div class="col s6">
                  <div class="input-field">
                     {render acl=$gosaMailMaxSizeACL checkbox=$multiple_support checked=$use_gosaMailMaxSize}
                     <input {if $own_script !="" } disabled {/if} id="gosaMailMaxSize" name="gosaMailMaxSize" size="6"
                        align="middle" type='text' maxlength="30" value="{$gosaMailMaxSize}" class="center">
                     <span class="helper-text">{t}MB{/t}</span>
                     {/render}
                  </div>
               </div>

            </div>

         </div>
      </div>
   </div>
</div>
</div>


<table summary="{t}Spam filter configuration{/t}">
   <tr style="padding-bottom:0px;">
      <td style='width:50%; '>


         <div>
            <div style='float:left'>

            </div>
            <div style='padding-left: 25px;'>
            </div>
         </div>

         <div class='clear'></div>

         <div>
            <div style='float:left'>

            </div>
            <div style='padding-left: 25px;'>

            </div>
         </div>

         <div class='clear'></div>


      <td class='left-border' rowspan="2">&nbsp;
      </td>
      <td>


         <div>
            <div style='float:left'>

            </div>
         </div>

         <div class='clear'></div>

         <div>

         </div>

         <div class='clear'></div>

      </td>
   </tr>
   <tr>
      <td style='width:45%'>

         <p style="margin-bottom:0px;">
         </p>

         <br>

         {if $show_templates eq "true"}
         {render acl=$gosaVacationMessageACL}
         <select id='vacation_template' name="vacation_template" {if $own_script !="" } disabled {/if} size=1>
            {html_options options=$vacationtemplates selected=$template}
            <option disabled>&nbsp;</option>
         </select>
         {/render}
         {render acl=$gosaVacationMessageACL}
         <button type='submit' name='import_vacation' id="import_vacation" {if $own_script !="" } disabled
            {/if}>{t}Import{/t}</button>

         {/render}
         {/if}
      </td>
      <td>
         <p style="margin-bottom:0px;">
            <b><label for="forwarder_list">{t}Forward messages to{/t}</label></b>
         </p>

         {if $multiple_support}
         <input type="checkbox" name="use_gosaMailForwardingAddress" onclick="changeState('gosaMailForwardingAddress');"
            class="center" {if $use_gosaMailForwardingAddress} checked {/if}>
         {/if}

         {render acl=$gosaMailForwardingAddressACL}
         <select {if $use_gosaMailForwardingAddress} checked {/if} id="gosaMailForwardingAddress"
            style="width:100%; height:100px;" name="forwarder_list[]" size=15 multiple>
            {html_options values=$gosaMailForwardingAddress output=$gosaMailForwardingAddress selected=$template}
            <option disabled>&nbsp;</option>
         </select>
         {/render}
         <br>
         {render acl=$gosaMailForwardingAddressACL}
         <input type='text' id='forward_address' name="forward_address" size=20 align="middle" maxlength=65 value="">
         {/render}
         {render acl=$gosaMailForwardingAddressACL}
         <button type='submit' name='add_forwarder' id="add_forwarder">{msgPool type=addButton}</button>&nbsp;

         {/render}
         {render acl=$gosaMailForwardingAddressACL}
         <button type='submit' name='add_local_forwarder' id="add_local_forwarder">{t}Add local{/t}</button>&nbsp;

         {/render}
         {render acl=$gosaMailForwardingAddressACL}
         <button type='submit' name='delete_forwarder' id="delete_forwarder">{msgPool type=delButton}</button>

         {/render}
      </td>
   </tr>
</table>
<hr>

<h3>{t}Advanced mail options{/t}
</h3>
<table summary="{t}Delivery settings{/t}">
   <tr>
      <td>
         {render acl=$gosaMailDeliveryModeLACL checkbox=$multiple_support checked=$use_only_local}
         <input {if $own_script !="" } disabled {/if} id='only_local' type=checkbox name="only_local" value="1"
            {$only_local} title="{t}Select if user can only send and receive inside his own domain{/t}" class="center">
         {/render}
         {t}User is only allowed to send and receive local mails{/t}
      </td>
   </tr>
</table>

<input type="hidden" name="mailTab" value="mailTab">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
   function validateClick() {
      alert("yes");
      if (!document.getElementById('use_vacation').checked) {
         return;
      }
   }

   function changeStates() {
      if ($('own_script').checked) {
         $("gosaVacationStart", "gosaVacationStop", "gosaVacationMessage").invoke("disable");
         $("datepicker-gosaVacationStop_image", "datepicker-gosaVacationStart_image").invoke("hide");
      } else {
         if ($('use_vacation').checked) {
            $("gosaVacationStart", "gosaVacationStop", "gosaVacationMessage").invoke("enable");
            $("datepicker-gosaVacationStop_image", "datepicker-gosaVacationStart_image").invoke("show");
         } else {
            $("gosaVacationStart", "gosaVacationStop", "gosaVacationMessage").invoke("disable");
            $("datepicker-gosaVacationStop_image", "datepicker-gosaVacationStart_image").invoke("hide");
         }
      }
   }

   changeStates();
   focus_field('mail');
</script>
{/if}