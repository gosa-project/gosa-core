
<select name='option'>
{foreach from=$options item=item key=key}
 <option value='{$key}'>{$item.NAME} ({$map[$item.TYPE]})</option>
{/foreach}
</select>


<table>
{foreach from=$sudoOption item=item key=key}
 <tr>
  <td>{$key}</td>
  <td style="width:20px;">
   {if $item.NEGATE}
    <img src='images/negate.png' alt="!">
   {/if}
  </td>
  <td>

  {if $options[$item.NAME].TYPE == "STRING"}
   <input type='text' name='option_value__{$key}' value="{$item.VALUE.0}" style='width:280px;'> 
  {elseif $options[$item.NAME].TYPE == "INTEGER"}
   <input type='text' name='option_value__{$key}' value="{$item.VALUE.0}" style='width:280px;'>
  {elseif $options[$item.NAME].TYPE == "BOOLEAN"}
   <input type='checkbox' value="1" name='option_value__{$key}'>
  {elseif $options[$item.NAME].TYPE == "BOOL_INTEGER"}
   <select name="">
    <option {if $item.VALUE == "FALSE"} selected {/if}value="FALSE">FALSE</option>
    <option {if $item.VALUE == "TRUE"} selected {/if}value="TRUE">TRUE</option>
    <option {if $item.VALUE != "TRUE" && $item.VALUE != "FALSE"} selected {/if}value="STRING">STRING</option>
    <input type='text' value="{$item.VALUE.0}" style='width:280px;'>
   </select> 
  {elseif $options[$item.NAME].TYPE == "STRING_BOOL"}
   <select name="">
    <option {if $item.VALUE == "FALSE"} selected {/if}value="FALSE">FALSE</option>
    <option {if $item.VALUE == "TRUE"} selected {/if}value="TRUE">TRUE</option>
    <option {if $item.VALUE != "TRUE" && $item.VALUE != "FALSE"} selected {/if}value="STRING">STRING</option>
    <input type='text' value="{$item.VALUE.0}" style='width:280px;'>
   </select> 
  {else}
   {$options[$item.NAME].TYPE} 
  {/if}
  </td>
 </tr>
{/foreach}
</table>
