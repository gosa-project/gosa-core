
<select name='option'>
{foreach from=$options item=item key=key}
 {if !isset($sudoOption.$key) || ($sudoOption.$key && $item.TYPE == "LISTS")}
 <option value='{$key}'>{$item.NAME} ({$map[$item.TYPE]})</option>
 {/if}
{/foreach}
</select>
<input type="submit" name="add_option" value="{msgPool type=addButton}">

<table>
{foreach from=$sudoOption item=item key=key}
  {foreach from=$item item=entry key=entry_key} 
 <tr>
  <td>{$key}</td>
  <td style="width:20px;">
   {if $entry.NEGATE}
    <img src='images/negate.png' alt="!">
   {/if}
  </td>
  <td>

  {if $options[$entry.NAME].TYPE == "STRING"}
   <!-- STRING  
    -->
   <input type='text' name='option_value__{$key}_{$entry_key}' value="{$entry.VALUE}" style='width:280px;'> 
  {elseif $options[$entry.NAME].TYPE == "LISTS"}
    <input type='text' value="{$entry.VALUE}" name="list_value__{$key}_{$entry_key}"></td>
  {elseif $options[$entry.NAME].TYPE == "INTEGER"}
   <!-- INTEGER  
    -->
   <input type='text' name='option_value__{$key}_{$entry_key}' value="{$entry.VALUE}" style='width:280px;'>
  {elseif $options[$entry.NAME].TYPE == "BOOLEAN"}
   <!-- BOOLEAN  
    -->
   <select name="option_value__{$key}_{$entry_key}">
    <option {if $entry.VALUE == "FALSE"} selected {/if}value="FALSE">FALSE</option>
    <option {if $entry.VALUE == "TRUE"} selected {/if}value="TRUE">TRUE</option>
   </select>
  {elseif $options[$entry.NAME].TYPE == "BOOL_INTEGER"}
   <!-- BOOLEAN_INTEGER 
    -->
   <select name="option_selection__{$key}_{$entry_key}" id="option_selection__{$key}_{$entry_key}"
	onChange="toggle_bool_fields('option_selection__{$key}_{$entry_key}','option_value__{$key}_{$entry_key}');">
    <option {if $entry.VALUE == "FALSE"} selected {/if}value="FALSE">FALSE</option>
    <option {if $entry.VALUE == "TRUE"} selected {/if}value="TRUE">TRUE</option>
    <option {if $entry.VALUE != "TRUE" && $entry.VALUE != "FALSE"} selected {/if}
	value="STRING">STRING</option>
    <input type='text' value="{$entry.VALUE}" style='width:280px;' name='option_value__{$key}_{$entry_key}'
	id="option_value__{$key}_{$entry_key}"
        {if $entry.VALUE == "FALSE" ||  $entry.VALUE == "TRUE"} disabled {/if}>
   </select> 
  {elseif $options[$entry.NAME].TYPE == "STRING_BOOL"}
   <!-- STRING_BOOLEAN 
    -->
   <select name="option_selection__{$key}_{$entry_key}" id="option_selection__{$key}_{$entry_key}"
	onChange="toggle_bool_fields('option_selection__{$key}_{$entry_key}','option_value__{$key}_{$entry_key}');">
    <option {if $entry.VALUE == "FALSE"} selected {/if}value="FALSE">FALSE</option>
    <option {if $entry.VALUE == "TRUE"} selected {/if}value="TRUE">TRUE</option>
    <option {if $entry.VALUE != "TRUE" && $entry.VALUE != "FALSE"} selected {/if}
	value="STRING">STRING</option>
    <input type='text' value="{$entry.VALUE}" style='width:280px;' name='option_value__{$key}_{$entry_key}'
	id="option_value__{$key}_{$entry_key}" 
        {if $entry.VALUE == "FALSE" ||  $entry.VALUE == "TRUE"} disabled {/if}>
   </select> 
  {/if}
  </td>
  <td style='width: 40px;'>
   <input type='image' src='images/negate.png'     name='negOption_{$key}_{$entry_key}' class='center'>
   <input type='image' src='images/edittrash.png'  name='delOption_{$key}_{$entry_key}' class='center'>
  </td>
 </tr>
  {/foreach}
{/foreach}
</table>

<script language="JavaScript" type="text/javascript">
 <!-- 
  {literal}
  function toggle_bool_fields(source_select,target_input)
  {
   var select= document.getElementById(source_select); 
   var input = document.getElementById(target_input); 
   if(select.value == "TRUE" || select.value == "FALSE"){
    input.disabled = true;
    input.value = select.value;
   }else{
    input.disabled = false;
    input.value = "";
   }
  }
  {/literal}
 -->
</script>


