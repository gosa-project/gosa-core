<h3>{t}List of dynamic rules{/t}</h3>

<table summary="{t}Labeled URI definitions{/t}" style='width:100%'>
 <tr>
  <td style='width:40%;'>{t}Base{/t}</td>
  <td>{t}Scope{/t}</td>
  <td>{t}Attribute{/t}</td>
  <td style='width:40%;'>{t}Filter{/t}</td>
  <td></td>
 </tr>
{foreach item=item key=key from=$labeledURIparsed}
 <tr>
  <td>
    <input style='width:98%;' type='text' value='{$item.base}' name='base_{$key}'>
  </td>
  <td>
    <select name='scope_{$key}' size='1'>
     {html_options options=$scopes selected=$item.scope}
    </select>
  </td>
  <td><input type='text' name='attr_{$key}' value='{$item.attr}'></td>
  <td><input name='filter_{$key}' type='text' style='width:98%;' value='{$item.filter}'></td>
  <td><button name='delUri_{$key}'>{msgPool type='delButton'}</button></td>
 </tr>
{/foreach}
 <tr>
  <td><button name='addUri'>{msgPool type='addButton'}</button></td>
  <td colspan="4"></td>
 </tr>
</table>
