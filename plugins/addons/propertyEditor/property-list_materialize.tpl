{if !$warningAccepted}

<div class="row valign-wrapper">
  <div class="col s2">
    {image path='<i class="material-icons md-48 orange-text">warning</i>'}
  </div>

  <div class="col s10">
    <h3>{t}Attention{/t}</h3>

    <p>
      {t}Modifying properties may break your setup, destroy or mess up your LDAP database, lead to security holes or it can even make a login impossible!{/t}
      {t}Since configuration properties are stored in the LDAP database a copy/backup can be handy.{/t}
    </p>

    <br>

    <p>
      {t}If you've debarred yourself, you can try to set 'ignoreLdapProperties' to 'true' in your gosa.conf main section. This will make GOsa ignore LDAP based property values.{/t}
    </p>
  </div>
</div>

<hr>

<div class="red lighten-3 z-depth-1 center-align">
  <label>
    <input type='checkbox' name='warningAccepted' value='1' id='warningAccepted'/>
    <span class="black-text">{t}I understand that there are certain risks, but I want to modify properties!{/t}</span>
  </label>
</div>

<hr>

<div class="card-action">
  <button class="btn-small gonicus-color" name='goOn'>{msgPool type='okButton'}</button>
</div>
<input type="hidden" name="ignore">

{else}

<script language="javascript" src="include/tooltip.js" type="text/javascript"></script>

<div>
  <span class="black-text">
    <h2>
      {$HEADLINE}&nbsp;{$SIZELIMIT}
      {if $ignoreLdapProperties}
      - <span class="red-text">{t}Ignoring LDAP defined properties!{/t}</span>
      {/if}
    </h2>
  </span>
</div>
<div class="control-panel">
  <div class="navigation">
    <div class="refresh">{$RELOAD}</div>
  </div>
  <div class="actions center-align">{$ACTIONS}</div>
  {$FILTER}
</div>
<hr>

{$LIST}
<input type="hidden" name="ignore">

<script type="text/javascript">
  Event.observe(window, "load", function () {
    $$("*").findAll(function (node) {
      return node.getAttribute('title');
    }).each(function (node) {
      var test = node.title;

      if ($(test)) {
        var t = new Tooltip(node, test);
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

<div class="card-action">
  <button class="btn-small gonicus-color" name='saveProperties'>{msgPool type='applyButton'}</button>
  <button class="btn-small gonicus-color" name='cancelProperties' {if !$is_modified} disabled {/if}>{t}Undo{/t}</button>
</div>
{/if}
