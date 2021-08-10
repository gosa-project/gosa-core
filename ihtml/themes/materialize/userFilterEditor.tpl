<h3>{t}Filter editor{/t}</h3>

<hr>

<div class="row">
  <div class="col s6">
    <div class="input-field">
      <input type='text' name='name' id='name' value="{$name}">
      <label for='name'>{t}Name{/t}{$must}</label>
    </div>

    <div class="input-field">
      <input type='text' name='description' id='description' value="{$description}">
      <label for='description'>{t}Description{/t}{$must}</label>
    </div>

    <div class="input-field">
      <select name='parent' size='1'>
        {html_options values=$fixedFilters output=$fixedFilters selected=$parent}
      </select>
      <label>{t}Parent filter{/t}</label>
    </div>

    <p>
      <label>
        <input type='checkbox' name='shareFilter' value='1' {if $share} checked {/if}>
        <span>{t}Public visible{/t}</span>
      </label>
    </p>

    <p>
      <label>
        <input type='checkbox' name='enableFilter' value='1' {if $enable} checked {/if}>
        <span>{t}Enabled{/t}</span>
      </label>
    </p>
  </div>

  <div class="col s6">
    <div class="input-field">
      <select id='usedCategory' name='usedCategory[]' size='4' multiple style='width:100%;'>
        {html_options options=$selectedCategories}
      </select>
      <label>{t}Categories where the filter is visible{/t}</label><br>
    </div>

    <div class="row">
      <div class="input-field col s4">
        <select id='availableCategory' name='availableCategory' size='1'
          onChange="$('manualCategory').value=$('availableCategory').options[$('availableCategory').selectedIndex].value">
          <option value=''>&nbsp;</option>
          {html_options values=$availableCategories output=$availableCategories}
        </select>
      </div>

      <div class="input-field col s4">
        <input type='text' id='manualCategory' name='manualCategory' value=''>
      </div>

      <div class="col s4">
        <button class="btn-small gonicus-color" type='submit' name='addCategory'>{msgPool type='addButton'}</button>
        <button class="btn-small gonicus-color" type='submit' name='delCategory'>{msgPool type='delButton'}</button>
      </div>
    </div>

  </div>
</div>


{foreach from=$queries item=item key=key}
<b>{t}Query{/t} #{$key}</b>
<select name='backend_{$key}' size='1'>
  {html_options output=$backends values=$backends selected=$item.backend}
</select>
<button class="btn-small gonicus-color" type='submit' name='removeQuery_{$key}'>{msgPool type='delButton'}</button>
<textarea name='filter_{$key}' id='filter_{$key}' cols="0" style='width:100%; height: 120px;'>{$item.filter}</textarea>
<hr>
{/foreach}
<button class="btn-small gonicus-color" type='submit' name='addQuery'>{msgPool type='addButton'}</button>
<input type='hidden' value='1' name='userFilterEditor'>
<br>

<div class="card-action">
  <button class="btn-small gonicus-color" type='submit' name='saveFilterSettings'>{msgPool type='saveButton'}</button>
  <button class="btn-small gonicus-color" type='submit' name='cancelFilterSettings'>{msgPool
    type='cancelButton'}</button>
</div>