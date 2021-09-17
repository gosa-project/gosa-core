<div class="mailadress-wrapper">
  <div class="row">
    <div class="col s12 xl6">
      {if !$multiple_support}
      <h3>{t}User Mail Address{/t}</h3>
      {render acl=$mailACL}
      <div class="input-field">
        <input type='text' id="mail" name="mail" size=35 maxlength=65 value="{$mail}">
        <label for="mail">{t}Mail address{/t}{$must}</label>
      </div>
      {/render}

      {else}
      <h3>{t}Users' Mail Address Scheme{/t}</h3>
      <p>{t}Please specify a mail address pattern (e.g., &#123;%uid&#124;@your-domain.com). Valid placeholders are: &#123;%uid&#124;, &#123;%givenName&#124; and &#123;%sn&#124;. For no action on the users' mail addresses, please leave empty.{/t}</p>
      {render acl=$mailACL}
      <div class="input-field">
        <input type='text' id="mail" name="mail" size=35 maxlength=65 value="">
        <label for="mail">{t}Mail address pattern{/t}</label></td>
      </div>
      {/render}

      {/if}
    </div>
    
    <div class="col s12 xl6">
      <!-- further mail aliases -->
      {if !$multiple_support}
      <h3>{t}User's Mail Aliases{/t}</h3>
      {render acl=$aliasACL}
      {$aliasWidget}
      {/render}

      {render acl=$aliasACL}
      <div class="input-field add">
        <input type="text" id="single_alias" name="single_alias" maxlength=65
          value="{t}Email alias address{/t}" onFocus='document.getElementById("single_alias").value=""'>
          <label for="single_alias">{t}Alias addresses{/t}</label>
          <button class="btn-small" id="add_alias" type="submit" name="add_alias">{t}Add{/t}</button>
      </div>
      {/render}
         
      {else}
      <h3>{t}Users' Mail Aliases{/t}</h3>
      <p>{t}Mass editing of email aliases is not supported.{/t}</p>
      {/if}
    </div>
  </div>
</div>

<input type="hidden" name="mailAccountTab" value="mailAccountTab">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  focus_field('mail');
</script>
