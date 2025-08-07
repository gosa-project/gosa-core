{if !$warningAccepted}

    <table summary="{t}Warning message{/t}" style='width:100%'>
     <tr>
      <td style='padding: 10px;'>{image path='images/warning.png'}</td>
      <td>
        <h3>{t}Attention{/t}</h3>
       <p>
        {t}Modifying properties may break your setup, destroy or mess up your LDAP database, lead to security holes or it can even make a login impossible!{/t}
        {t}Since configuration properties are stored in the LDAP database a copy/backup can be handy.{/t}
       </p>

       <p>
        {t}If you've debarred yourself, you can try to set 'ignoreLdapProperties' to 'true' in your gosa.conf main section. This will make GOsa ignore LDAP based property values.{/t}
       </p>

      </td> 
     </tr>
    </table>
    <hr>
      <input type='checkbox' name='warningAccepted' value='1' id='warningAccepted'>
      <label for='warningAccepted'>{t}I understand that there are certain risks, but I want to modify properties!{/t}</label>
    <hr>
    <div class="plugin-actions">
        <button name='goOn'>{msgPool type='okButton'}</button> 
    </div>
    <input type="hidden" name="ignore">

{else}

<script language="javascript" src="include/tooltip.js" type="text/javascript"></script>

<div id="mainlist">

  <div class="mainlist-header">
   <p>{$HEADLINE}&nbsp;{$SIZELIMIT}
    {if $ignoreLdapProperties}
    -  <font color='red'>{t}Ignoring LDAP defined properties!{/t}</font>
    {/if}
   </p>
   <div class="mainlist-nav">
    <table summary="{$HEADLINE}">
     <tr>
      <td>{$RELOAD}</td>
      <td class="left-border">{$ACTIONS}</td>
      <td class="left-border">{$FILTER}</td>
     </tr>
    </table>
   </div>
  </div>
  {$LIST}
</div>

<script type="text/javascript">
  Event.observe(window,"load",function() {
    $$("*").findAll(function(node){
      return node.getAttribute('title');
    }).each(function(node){
      var test = node.title;

      if($(test)){
          var t = new Tooltip(node,test);
          t.options.delta_x = -150;
          t.options.delta_y = -50;
          node.removeAttribute("title");
      }
    });
  });
</script>

{if !$is_modified}
    <input type="hidden" name="ignore">
{/if}

<div class="plugin-actions">
    <button name='saveProperties'>{msgPool type='applyButton'}</button>
    <button name='cancelProperties' {if !$is_modified} disabled {/if}>{t}Undo{/t}</button>
</div>
{/if} 
