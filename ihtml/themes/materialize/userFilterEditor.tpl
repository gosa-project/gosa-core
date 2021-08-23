<h2>{t}Filter editor{/t}</h2>

<div class="row content">
  <div class="filter-editor">
    <div class="col s6 xl6">
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

    <button class="btn-small" type='submit' name='addQuery'>{msgPool type='addButton'}</button>

    {foreach from=$queries item=item key=key}
    <h4>{t}Query{/t} #{$key}</h4>
    <div class="input-field">
      <select name='backend_{$key}' size='1'>
        {html_options output=$backends values=$backends selected=$item.backend}
      </select>
    </div>

    <button class="btn-small" type='submit' name='removeQuery_{$key}'>{msgPool type='delButton'}</button>

    <div class="input-field">
      <textarea class="materialize-textarea" name='filter_{$key}' id='filter_{$key}'>
        {$item.filter}
      </textarea>
    </div>
    {/foreach}
  </div>

  <div class="col s6 xl6">
    <div class="input-field">
      <select id='usedCategory' name='usedCategory[]' size='4' multiple style='width:100%;'>
        {html_options options=$selectedCategories}
      </select>
      <label>{t}Categories where the filter is visible{/t}</label>
    </div>

    <div class="input-field">
      <select id='availableCategory' name='availableCategory' size='1'
        onChange="$('manualCategory').value=$('availableCategory').options[$('availableCategory').selectedIndex].value">
        <option value=''>&nbsp;</option>
        {html_options values=$availableCategories output=$availableCategories}
      </select>
    </div>

    <div class="input-field">
      <input type='text' id='manualCategory' name='manualCategory' value=''>
    </div>

    <button class="btn-small" type='submit' name='addCategory'>{msgPool type='addButton'}</button>
    <button class="btn-small" type='submit' name='delCategory'>{msgPool type='delButton'}</button>
  </div>
  </div>
  
</div>


<input type='hidden' value='1' name='userFilterEditor'>

<div class="card-action">
  <button class="btn-small gonicus-color" type='submit' name='saveFilterSettings'>{msgPool type='saveButton'}</button>
  <button class="btn-small gonicus-color" type='submit' name='cancelFilterSettings'>{msgPool
    type='cancelButton'}</button>
</div>