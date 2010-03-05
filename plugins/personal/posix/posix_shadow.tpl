
  <h3>{t}Account settings{/t}</h3>
  
  <!-- must_change_password  -->
  <div>
   <div style='float:left;'>
     {render acl=$mustchangepasswordACL checkbox=$multiple_support checked=$use_mustchangepassword}
       <input type="checkbox" class="center" name="mustchangepassword" value="1" {$mustchangepassword}>
     {/render}
   </div>
   <div style='padding-left: 25px;'>
     {t}User must change password on first login{/t}
   </div>
  </div>
    
  <div class="clear"></div>  
 
  <!-- shadowMin -->
  <div>
   <div style='float:left;'>
     {render acl=$shadowMinACL checkbox=$multiple_support checked=$use_activate_shadowMin}
       <input type="checkbox" class="center" name="activate_shadowMin" value="1" {$activate_shadowMin}>
     {/render}
   </div>
   <div style='padding-left: 25px;'>
     {render acl=$shadowMinACL}
      {$shadowmins}
     {/render}
   </div>
  </div>

  <div class="clear"></div>  
   
  <!-- activate_shadowMax -->
  <div>
   <div style='float:left;'>
     {render acl=$shadowMaxACL checkbox=$multiple_support checked=$use_activate_shadowMax}
      <input type="checkbox" class="center" name="activate_shadowMax" value="1" {$activate_shadowMax}>
     {/render}
   </div>
   <div style='padding-left: 25px;'>
     {render acl=$shadowMaxACL}
      {$shadowmaxs}
     {/render}
   </div>
  </div>
  
  <div class="clear"></div>  
 
  <!-- activate_shadowExpire -->
  <div>
   <div style='float:left;'>
    {render acl=$shadowExpireACL checkbox=$multiple_support checked=$use_activate_shadowExpire}
     <input type="checkbox" class="center" name="activate_shadowExpire" value="1" {$activate_shadowExpire}>
    {/render}
   </div>
   <div style='padding-left: 25px;'>

    <table summary="{t}Password expiration settings{/t}" border="0" cellpadding="0" cellspacing="0">
     <tr>
      <td>
       {t}Password expires on{/t}&nbsp;
      </td>
      <td style='width:125px'>

       {render acl=$shadowExpireACL}
        <input type="text" id="shadowExpire" name="shadowExpire" class="date" style='width:100px' value="{$shadowExpire}">
       {/render}

       {if $shadowExpireACL|regex_replace:"/[cdmr]/":"" == "w"}
        <script type="text/javascript">
        {literal}
        var datepicker  = new DatePicker({ relative : 'shadowExpire', language : '{/literal}{$lang}{literal}',
        keepFieldEmpty : true, enableCloseEffect : false, enableShowEffect : false});
        {/literal}
        </script>
       {/if}
      </td>
     </tr>
    </table>
    
   </div>
  </div>
  
  <div class="clear"></div>  
 
  <!-- shadowInactive -->
  <div>
   <div style='float:left;'>
    {render acl=$shadowInactiveACL checkbox=$multiple_support checked=$use_activate_shadowInactive}
     <input type="checkbox" class="center" name="activate_shadowInactive" value="1" {$activate_shadowInactive}>
    {/render}
   </div>
   <div style='padding-left: 25px;'>
    {render acl=$shadowInactiveACL}
     {$shadowinactives}
    {/render}
   </div>
  </div>
   
  <div class="clear"></div>  

  <!-- activate_shadowWarning -->
  <div>
   <div style='float:left;'>
    {render acl=$shadowWarningACL checkbox=$multiple_support checked=$use_activate_shadowWarning}
     <input type="checkbox" class="center" name="activate_shadowWarning" value="1" {$activate_shadowWarning}>
    {/render}
   </div>
   <div style='padding-left: 25px;'>
    {render acl=$shadowWarningACL}
     {$shadowwarnings}
    {/render}
   </div>
  </div>
