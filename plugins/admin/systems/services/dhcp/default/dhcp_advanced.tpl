{* GOsa dhcp sharedNetwork - smarty template *}
<hr>

{if $show_advanced}

 <button type='submit' name='hide_advanced'>{t}Hide advanced settings{/t}</button>

 <table width="100%" summary="{t}DHCP advanced settings{/t}">
  <tr>
   <td width="50%" class='right-border'>
    <hr>{t}DHCP statements{/t}</hr>

    {render acl=$acl}
     <select name='dhcpstatements' style="width:100%;" size="14">
      {html_options options=$dhcpstatements}
     </select>
    {/render}
    <br>
    {render acl=$acl}
     <input type='text' name='addstatement' size='25' maxlength='250'>&nbsp;
    {/render}
    {render acl=$acl}
     <button type='submit' name='add_statement'>{msgPool type=addButton}</button>&nbsp;
    {/render}
    {render acl=$acl}
     <button type='submit' name='delete_statement'>{msgPool type=delButton}</button>
    {/render}
   </td>
   <td>
    <h3>{t}DHCP options{/t}</h3>
    {render acl=$acl}
     <select name='dhcpoptions' style="width:100%;" size="14">
      {html_options options=$dhcpoptions}
     </select>
    {/render}
    <br>
    {render acl=$acl}
     <input type='text' name='addoption' size='25' maxlength='250'>&nbsp;
    {/render}
    {render acl=$acl}
     <button type='submit' name='add_option'>{msgPool type=addButton}</button>&nbsp;
    {/render}
    {render acl=$acl}
     <button type='submit' name='delete_option'>{msgPool type=delButton}</button>
    {/render}
   </td>
  </tr>
 </table>
 {else}
 <button type='submit' name='show_advanced'>{t}Show advanced settings{/t}</button>
{/if}
<hr>
