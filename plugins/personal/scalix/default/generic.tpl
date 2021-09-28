<div class="scalix-wrapper">
   <div class="row">
      <div class="col s12 xl6">
         <h3>{t}Generic{/t}</h3>

         {render acl=$scalixMailnodeACL}
            <div class="input-field">
               <input type='text' id="scalixMailnode" name="scalixMailnode" size=35 maxlength=65 value="{$scalixMailnode}">
               <label for="scalixMailnode">{t}SCALIX mail node{/t}{$must}</label>
            </div>
         {/render}

         {if !$scalixGroup}
            {render acl=$scalixMailboxClassACL}
               <div class="input-field">
                  <select size="1" id="scalixMailboxClass" name="scalixMailboxClass"
                     title="{t}Limited users can not make use of the group calendar functionality in SCALIX{/t}">
                     {html_options values=$mailboxClasses output=$mailboxClasses selected=$scalixMailboxClass}
                  </select>
                  <label>{t}SCALIX mailbox class{/t}</label>
               </div>
            {/render}

            {render acl=$scalixServerLanguageACL}
               <div class="input-field">
                  <select size="1" id="scalixServerLanguage" name="scalixServerLanguage"
                     title="{t}Message catalog language for client{/t}">
                     {html_options values=$serverLanguages output=$serverLanguages selected=$scalixServerLanguage}
                  </select>
                  <label for="scalixServerLanguage">{t}SCALIX server language{/t}</label>
               </div>
            {/render}
         {/if}

         {if !$scalixGroup}
            {render acl=$scalixAdministratorACL}
               <label>
                  <input type=checkbox name="scalixAdministrator" value="1" {$scalixAdministrator}
                     title="{t}Select for administrator capabilities{/t}">
                  <span>{t}SCALIX Administrator{/t}</span>
               </label>
            {/render}

            {render acl=$scalixMailboxAdministratorACL}
               <label>
                  <input type=checkbox name="scalixMailboxAdministrator" value="1" {$scalixMailboxAdministrator}
                     title="{t}Select for mailbox administrator capabilities{/t}">
                  <span>{t}SCALIX Mailbox Administrator{/t}</span>
               </label>
            {/render}

            {render acl=$scalixHideUserEntryACL}
               <label>
                  <input type=checkbox name="scalixHideUserEntry" value="1" {$scalixHideUserEntry}
                     title="{t}Hide user entry from address book{/t}">
                  <span>{t}Hide this user entry in SCALIX{/t}</span>
               </label>
            {/render}

            {render acl=$scalixLimitMailboxSizeACL}
               <div class="input-field">
                  <input type='text' id="scalixLimitMailboxSize" name="scalixLimitMailboxSize" size=5 maxlength=10 value="{$scalixLimitMailboxSize}">
                  <label for="scalixLimitMailboxSize">{t}Limit mailbox size{/t} ({t}MB{/t})</label>
               </div>
            {/render}

            {render acl=$scalixLimitOutboundMailACL}
               <label>
                  <input type=checkbox name="scalixLimitOutboundMail" value="1" {$scalixLimitOutboundMail}
                     title="{t}As sanction on mailbox quota overuse, stop user from sending mail{/t}">
                  <span>{t}Limit Outbound Mail{/t}</span>
               </label>
            {/render}

            {render acl=$scalixLimitInboundMailACL}
               <label>
                  <input type=checkbox name="scalixLimitInboundMail" value="1" {$scalixLimitInboundMail}
                     title="{t}As sanction on mailbox quota overuse, stop user from receiving mail{/t}">
                  <span>{t}Limit Inbound Mail{/t}</span>
               </label>
            {/render}

            {render acl=$scalixLimitNotifyUserACL}
               <label>
                  <input type=checkbox name="scalixLimitNotifyUser" value="1" {$scalixLimitNotifyUser}
                     title="{t}As sanction on mailbox quota overuse, notify the user by email{/t}">
                  <span>{t}Notify User{/t}</span>
               </label>
            {/render}
         {/if}
      </div>

      <div class="col s12 xl6">
         <h3>{t}SCALIX email addresses{/t}</h3>

         {render acl=$scalixEmailAddressACL}
            <div class="input-field">
               <select id="emails_list" name="emails_list[]" size="15" multiple title="{t}List of SCALIX email addresses{/t}">
                  {html_options values=$scalixEmailAddress output=$scalixEmailAddress}
               </select>
            </div>
         {/render}

         {render acl=$scalixEmailAddressACL}
            <div class="input-field add">
               <input type='text' name="email_address" size="30" align="middle" maxlength="65" value="">

               <button class="btn-small" type='submit' name='add_email'>{msgPool type=addButton}</button>
               <button class="btn-small" type='submit' name='delete_email'>{msgPool type=delButton}</button>
            </div>
         {/render}
      </div>
   </div>

   <input type="hidden" name="scalixTab" value="scalixTab">

   <!-- Place cursor -->
   <script language="JavaScript" type="text/javascript">
      // First input field on page
      focus_field('scalixMailnode');
   </script>

</div>
