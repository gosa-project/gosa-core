
<h3>{t}System trust{/t}
</h3>

{if !$multiple_support}{t}Trust mode{/t}&nbsp;
 {render acl=$trustmodeACL}
  <select name="trustmode" id="trustmode" size=1   onChange="changeSelectState('trustmode', 'add_ws');   changeSelectState('trustmode', 'del_ws');">
   {html_options options=$trustmodes selected=$trustmode}
  </select>
 {/render}
 {render acl=$trustmodeACL}
  {$trustList}
 {/render}
 {render acl=$trustmodeACL}
  <button {$trusthide}type='submit' name='add_ws' id="add_ws">
  {msgPool type=addButton}</button>&nbsp;
 {/render}
 {render acl=$trustmodeACL}
  <button {$trusthide}type='submit' name='delete_ws' id="del_ws">
  {msgPool type=delButton}</button>
 {/render}
 {else}
 
 
 <input type="checkbox" name="use_trustmode" {if $use_trustmode} checked {/if}class="center" onClick="$('div_trustmode').toggle();">{t}Trust mode{/t}&nbsp;
 
 
 <div {if !$use_trustmode} style="display: none;" {/if}id="div_trustmode">
 {render acl=$trustmodeACL}
  <select name="trustmode" id="trustmode" size=1 onChange="changeSelectState('trustmode', 'add_ws'); changeSelectState('trustmode', 'del_ws');">
   {html_options options=$trustmodes selected=$trustmode}
  </select>
 {/render}
 {render acl=$trustmodeACL}
  {$trustList}
 {/render}
 {render acl=$trustmodeACL}
  <button type='submit' name='add_ws' id="add_ws">
  {msgPool type=addButton}</button>&nbsp;
 {/render}
 {render acl=$trustmodeACL}
  <button type='submit' name='delete_ws' id="del_ws">
  {msgPool type=delButton}</button>
 {/render}
</div>

{/if}
