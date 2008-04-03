
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
   <!-- STRING  
    -->
   <input type='text' name='option_value__{$key}' value="{$item.VALUE.0}" style='width:280px;'> 
  {elseif $options[$item.NAME].TYPE == "INTEGER"}
   <!-- INTEGER  
    -->
   <input type='text' name='option_value__{$key}' value="{$item.VALUE.0}" style='width:280px;'>
  {elseif $options[$item.NAME].TYPE == "BOOLEAN"}
   <!-- BOOLEAN  
    -->
   <select name="option_value__{$key}">
    <option {if $item.VALUE.0 == "FALSE"} selected {/if}value="FALSE">FALSE</option>
    <option {if $item.VALUE.0 == "TRUE"} selected {/if}value="TRUE">TRUE</option>
   </select>
  {elseif $options[$item.NAME].TYPE == "BOOL_INTEGER"}
   <!-- BOOLEAN_INTEGER 
    -->
   <select name="option_selection__{$key}" id="option_selection__{$key}"
	onChange="toggle_bool_fields('option_selection__{$key}','option_value__{$key}');">
    <option {if $item.VALUE.0 == "FALSE"} selected {/if}value="FALSE">FALSE</option>
    <option {if $item.VALUE.0 == "TRUE"} selected {/if}value="TRUE">TRUE</option>
    <option {if $item.VALUE.0 != "TRUE" && $item.VALUE.0 != "FALSE"} selected {/if}
	value="STRING">STRING</option>
    <input type='text' value="{$item.VALUE.0}" style='width:280px;' name='option_value__{$key}'
	id="option_value__{$key}"
        {if $item.VALUE.0 == "FALSE" ||  $item.VALUE.0 == "TRUE"} disabled {/if}>
   </select> 
  {elseif $options[$item.NAME].TYPE == "STRING_BOOL"}
   <!-- STRING_BOOLEAN 
    -->
   <select name="option_selection__{$key}" id="option_selection__{$key}"
	onChange="toggle_bool_fields('option_selection__{$key}','option_value__{$key}');">
    <option {if $item.VALUE.0 == "FALSE"} selected {/if}value="FALSE">FALSE</option>
    <option {if $item.VALUE.0 == "TRUE"} selected {/if}value="TRUE">TRUE</option>
    <option {if $item.VALUE.0 != "TRUE" && $item.VALUE.0 != "FALSE"} selected {/if}
	value="STRING">STRING</option>
    <input type='text' value="{$item.VALUE.0}" style='width:280px;' name='option_value__{$key}'
	id="option_value__{$key}" 
        {if $item.VALUE.0 == "FALSE" ||  $item.VALUE.0 == "TRUE"} disabled {/if}>
   </select> 
  {else}
   {$options[$item.NAME].TYPE} 
  {/if}
  </td>
  <td style='width: 40px;'>
   <input type='image' src='images/negate.png'     name='negOption_{$key}' class='center'>
   <input type='image' src='images/edittrash.png'  name='delOption_{$key}' class='center'>
  </td>
 </tr>
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


