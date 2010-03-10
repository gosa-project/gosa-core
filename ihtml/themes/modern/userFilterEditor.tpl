<h3>{t}Filter editor{/t}</h3>

<hr>

<table summary="{t}Filter editor{/t}" width="100%">
  <tr>
    <td style='vertical-align: top; width:40%;'>
      <table summary='{t}Filter properties{/t}'>
        <tr>
          <td>
            <label for='name'>{t}Name{/t}</label>
          </td>
          <td>
            <input type='text' name='name' id='name' value='{$name}'>
          </td>   
        </tr>
        <tr>
          <td>
            <label for='description'>{t}Description{/t}</label>
          </td>
          <td>
            <input type='text' name='description' id='description' value='{$description}'>
          </td>   
        </tr>
        <tr>
          <td>
            <label for='parent'>{t}Extension for the base filter{/t}</label>
          </td>
          <td>
            <select name='parent'>
              {html_options values=$fixedFilters output=$fixedFilters selected=$parent}
            </select>
          </td>   
        </tr>
      </table>  
    
      <br>
      
      <input type='checkbox' name='shareFilter' value='1' {if $share} checked {/if}>
       {t}Public visible{/t}              

      <br>
    
      <input type='checkbox' name='enableFilter' value='1' {if $enable} checked {/if}>
       {t}Enabled{/t}

    </td>
    <td style='vertical-align: top;'>
      <label for='usedCategory'>{t}This filter affects the following categories{/t}</label><br>
      <select id='usedCategory' name='usedCategory[]' size='4' multiple style='width:100%;'>
        {html_options options=$selectedCategories}
      </select>
      <br>
      <select id='availableCategory' name='availableCategory'
        onChange="$('manualCategory').value=$('availableCategory').options[$('availableCategory').selectedIndex].value"> 
        <option value=''>&nbsp;</option>
        {html_options values=$availableCategories output=$availableCategories}
      </select>
      <input type='text' id='manualCategory' name='manualCategory' value=''>
      <button type='submit' name='addCategory'>{msgPool type='addButton'}</button>
      <button type='submit' name='delCategory'>{msgPool type='delButton'}</button>
    </td>
  </tr>
</table>

<hr>

<textarea name='filter' id='filter' cols="0" style='width:100%; height: 100px;'>{$filter}</textarea>

<hr>

<input type='hidden' value='1' name='userFilterEditor'>

<div class="plugin-actions">
  <button type='submit' name='saveFilterSettings'>{msgPool type='saveButton'}</button>
  <button type='submit' name='cancelFilterSettings'>{msgPool type='cancelButton'}</button>
</div>
