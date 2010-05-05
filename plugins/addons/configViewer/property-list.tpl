<script language="javascript" src="include/tooltip-v0.2.js" type="text/javascript"></script>

<div id="mainlist">

  <div class="mainlist-header">
   <p>{$HEADLINE}&nbsp;{$SIZELIMIT}</p>
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
      new Tooltip(node,test);
      node.removeAttribute("title");
    });
  });
</script>

{if !$is_modified}
<input type="hidden" name="ignore">
{/if}

<div class="plugin-actions">
    <button name='saveProperties'>{msgPool type='okButton'}</button>
    <button name='cancelProperties'>{msgPool type='cancelButton'}</button>
</div>

