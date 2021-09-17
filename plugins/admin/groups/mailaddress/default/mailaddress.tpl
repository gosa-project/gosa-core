<div class="mailaddress-wrapper">
   <div class="row">
      <div class="col s12 xl6">
         {if !$multiple_support}
         <h3>{t}Generic{/t}</h3>
         {render acl=$mailACL}
         <div class="input-field">
            <input type='text' id="mail" name="mail" size=35 maxlength=65 value="{$mail}">
            <label for="mail">{t}Mail address{/t}{$must}</label>
         </div>
         {/render}

         {else}
         
         <h3>{t}Generic{/t}</h3>
         <p>{t}Please specify a mail address pattern (e.g., &#123;%cn&#124;@your-domain.com) or leave empty.{/t}</p>
         {render acl=$mailACL}
         <div class="input-fiel">
            <input type='text' id="mail" name="mail" size=35 maxlength=65 value="{$mail}">
            <label for="mail">{t}Mail address pattern{/t}{$must}</label>
         </div>
         {/render}
         {/if}
      </div>

      <!-- Alternate addresses -->
      <div class="col s12 xl6">
         {if !$multiple_support}
         <h3>{t}Alternative addresses{/t}</h3>
         
         {render acl=$gosaMailAlternateAddressACL}
         <div class="input-field">
            <select style="width:100%;" name="alternates_list[]" size=10 multiple title="{t}List of alternative mail addresses{/t}">
               {html_options values=$gosaMailAlternateAddress output=$gosaMailAlternateAddress}
            </select>
         </div>
         {/render}

         {render acl=$gosaMailAlternateAddressACL}
         <div class="input-field add">
            <input type='text' name="alternate_address" size="30" align=middle maxlength="60" value="">
            <button class="btn-small" type='submit' name='add_alternate'>{msgPool type=addButton}</button>&nbsp;
            <button class="btn-small" type='submit' name='delete_alternate'>{msgPool type=delButton}</button>
         </div>
         {/render}
         {/if}

      </div>
      
      <div class="col s12 xl6">
         <h3>{t}Advanced mail options{/t}</h3>
         
         {render acl=$gosaMailForwardingAddressACL}
         <div class="input-field">
            <select style="width:100%;" name="forwarder_list[]" size=10 multiple>
               
               {if $multiple_support}
               {foreach from=$Forward_all item=item key=key}
               <option value="{$item}">{$item}&nbsp;({t}Used in all groups{/t})</option>
               {/foreach}
               
               {foreach from=$Forward_some item=item key=key}
               <option value="{$item}" style='color: #888888; background: #DDDDDD;background-color: #DDDDDD;'>{$item}&nbsp;({t}Not used in all groups{/t})</option>
               {/foreach}
               
               {else}
               {html_options values=$gosaMailForwardingAddress output=$gosaMailForwardingAddress}
               {/if}
            </select>
            <label><h2>{t}Forward messages to non group members{/t}</h2></label>   
         </div>
         {/render}
         
         {render acl=$gosaMailForwardingAddressACL}
         <div class="input-field add">
            <input type='text' name="forward_address" size=20 align=middle maxlength=65 value="">
            <button class="btn-small" type='submit' name='add_forwarder'>{msgPool type=addButton}</button>&nbsp;
            <button class="btn-small" type='submit' name='delete_forwarder'>{msgPool type=delButton}</button>
         </div>
         {/render}
      </div>
   </div>
</div>

<input type="hidden" name='mailedit' value='1'>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
   // First input field on page
	focus_field('mail');
</script>

{if $multiple_support}
	<input type="hidden" name="multiple_mail_group_posted" value="1">
{/if}
