<div class="filter-wrapper">
  <h2>{t}Filter editor{/t}</h2>

  <div class="content-wrapper card-content-scroll">
    <div class="row">
      <div class="col s12 xl6">
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

        <div class="row">
          <div class="col s6">
            <label>
              <p>
                <input type='checkbox' name='shareFilter' value='1' {if $share} checked {/if}>
                <span>{t}Public visible{/t}</span>
              </p>
            </label>
          </div>

          <div class="col s6">
            <label>
              <p>
                <input type='checkbox' name='enableFilter' value='1' {if $enable} checked {/if}>
                <span>{t}Enabled{/t}</span>
              </p>
            </label>
          </div>
        </div>
      </div>

      <div class="col s12 xl6">
        <div class="row category">
          <div class="col s10 input-field">
            <select id='usedCategory' name='usedCategory[]' size='4' multiple style='width:100%;'>
              {html_options options=$selectedCategories}
            </select>
            <label for="usedCategory">{t}Categories where the filter is visible{/t}</label>
          </div>

          <div class="col s2 action">
            <button class="btn-small" type='submit' name='delCategory'>{msgPool type='delButton'}</button>
          </div>
        </div>

        <div class="row add-category">
          <div class="col s5 input-field">
            <select id='availableCategory' name='availableCategory' size='1'
              onChange="$('manualCategory').value=$('availableCategory').options[$('availableCategory').selectedIndex].value">
                <option value=''>&nbsp;</option>
                {html_options values=$availableCategories output=$availableCategories}
            </select>

            <label for="availableCategory">{t}Choose category{/t}</label>
          </div>

          <div class="col s5 input-field">
            <input type='text' id='manualCategory' name='manualCategory' value=''>
            <label for="manualCategory">{t}Category to be attached{/t}</label>
          </div>

          <div class="col s2 action">
            <button class="btn-small" type='submit' name='addCategory'>{msgPool type='addButton'}</button>
          </div>
        </div>
      </div>
    </div>

    <hr class="divider">

    <div class="row query-wrapper">
      <div class="col s12">
        <button class="btn-small" type='submit' name='addQuery'>{msgPool type='addButton' what='Query'}</button>
      </div>

      {foreach from=$queries item=item key=key}
        <div class="col s12 xl6">
          <div class="query-content">
            <div class="query-head">
              <h3>{t}Query{/t} #{$key}</h3>
              <button class="btn-small" type='submit' name='removeQuery_{$key}'>{msgPool type='delButton'}</button>
            </div>

            <div class="input-field">
              <select name='backend_{$key}' size='1'>
                {html_options output=$backends values=$backends selected=$item.backend}
              </select>
            </div>

            <div class="textarea-wrapper">
              <textarea class="materialize-textarea" name='filter_{$key}' id='filter_{$key}'>
                {$item.filter}
              </textarea>
            </div>
          </div>
        </div>
      {/foreach}
    </div>
  </div>


  <input type='hidden' value='1' name='userFilterEditor'>

  <div class="card-action">
    <button class="btn-small primary" type='submit' name='saveFilterSettings'>{msgPool type='saveButton'}</button>
    <button class="btn-small primary" type='submit' name='cancelFilterSettings'>{msgPool
      type='cancelButton'}</button>
  </div>
</div>
