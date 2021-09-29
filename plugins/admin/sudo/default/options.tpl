<div class="sudo-options-wrapper">
  <div class="row">
    <div class="col s12 xl6">
      <h3>Used sudo role options</h3>

      <div class="sudo-options-table">
        <table id="sudo-table">
          <thead>
            <tr>
              <th><b>{t}Option name{/t}</b></th>
              <th></th>
              <th><b>{t}Value{/t}</b></th>
              <th><b>{t}Options{/t}</b></th>
            </tr>
          </thead>

          <tbody>
            {foreach from=$sudoOption item=item key=key}
            {foreach from=$item item=entry key=entry_key}
              <tr>
                <td>{$key}</td>
                <td>
                {if $entry.NEGATE}
                  {image path="<i class='material-icons md-18'>not_interested</i>"}
                {/if}
                </td>
                <td>
                {render acl=$ACL}
                {if $options[$entry.NAME].TYPE == "STRING"}
                <!-- STRING  -->
                <div class="input-field table-content">
                  <input type='text' name='option_value__{$key}_{$entry_key}' value="{$entry.VALUE}">
                </div>

                {elseif $options[$entry.NAME].TYPE == "LISTS"}
                <!-- LISTS  -->
                <div class="input-field table-content">
                  <input type='text' value="{$entry.VALUE}" name="list_value__{$key}_{$entry_key}" style='width:280px;'>
                </div>

                {elseif $options[$entry.NAME].TYPE == "INTEGER"}
                <!-- INTEGER  -->
                <div class="input-field table-content">
                  <input type='text' name='option_value__{$key}_{$entry_key}' value="{$entry.VALUE}" style='width:280px;'>
                </div>

                {elseif $options[$entry.NAME].TYPE == "BOOLEAN"}
                <!-- BOOLEAN  -->
                <div class="input-field table-content">
                  <select name="option_value__{$key}_{$entry_key}" style="width:80px;" size=1>
                    <option {if $entry.VALUE == "FALSE"} selected {/if}value="FALSE">FALSE</option>
                    <option {if $entry.VALUE == "TRUE"} selected {/if}value="TRUE">TRUE</option>
                  </select>
                </div>

                {elseif $options[$entry.NAME].TYPE == "BOOL_INTEGER"}
                <!-- BOOLEAN_INTEGER -->
                <div class="input-field table-content">
                  <select name="option_selection__{$key}_{$entry_key}" id="option_selection__{$key}_{$entry_key}" size=1
                    onChange="toggle_bool_fields('option_selection__{$key}_{$entry_key}','option_value__{$key}_{$entry_key}');">
                    <option {if $entry.VALUE == "FALSE"} selected {/if}value="FALSE">FALSE</option>
                    <option {if $entry.VALUE == "TRUE"} selected {/if}value="TRUE">TRUE</option>
                    <option {if $entry.VALUE != "TRUE" && $entry.VALUE != "FALSE"} selected {/if} value="STRING">INTEGER</option>
                  </select>
                </div>

                <div class="input-field table-content">
                  <input type='text' value="{$entry.VALUE}" name='option_value__{$key}_{$entry_key}'id="option_value__{$key}_{$entry_key}"
                    {if $entry.VALUE == "FALSE" ||  $entry.VALUE == "TRUE"} disabled {/if}>
                </div>

                {elseif $options[$entry.NAME].TYPE == "STRING_BOOL"}
                <!-- STRING_BOOLEAN -->
                <div class="input-field table-content">
                  <select name="option_selection__{$key}_{$entry_key}" id="option_selection__{$key}_{$entry_key}" size=1
                    onChange="toggle_bool_fields('option_selection__{$key}_{$entry_key}','option_value__{$key}_{$entry_key}');">
                    <option {if $entry.VALUE == "FALSE"} selected {/if}value="FALSE">FALSE</option>
                    <option {if $entry.VALUE == "TRUE"} selected {/if}value="TRUE">TRUE</option>
                    <option {if $entry.VALUE != "TRUE" && $entry.VALUE != "FALSE"} selected {/if} value="STRING">STRING</option>
                  </select>
                </div>

                <div class="input-field table-content">
                  <input type='text' value="{$entry.VALUE}" name='option_value__{$key}_{$entry_key}' id="option_value__{$key}_{$entry_key}"
                    {if $entry.VALUE == "FALSE" ||  $entry.VALUE == "TRUE"} disabled {/if}>
                </div>
                {/if}
                {/render}
              </td>
              <td style="display: flex;">
              {render acl=$ACL}
                {image path="<i class='material-icons md-18'>not_interested</i>" action="negOption_{$key}_{$entry_key}"}
              {/render}

              {render acl=$ACL}
                {image path="<i class='material-icons md-18'>delete</i>" action="delOption_{$key}_{$entry_key}"}
              {/render}
              </td>
            </tr>
            {/foreach}
            {/foreach}
          </tbody>
        </table>
      </div>
    </div>

    <div class="col s12 xl6">
      <h3>{t}Available options{/t}:</h3>

      {render acl=$ACL}
      <div class="input-field add">
        <select name='option' size=1>
          {foreach from=$options item=item key=key}
          {if !isset($sudoOption.$key) || ($sudoOption.$key && $item.TYPE == "LISTS")}
          <option value='{$key}'>{$item.NAME} ({$map[$item.TYPE]})</option>
          {/if}
          {/foreach}
        </select>
        <button class="btn-small" type='submit' name='add_option'>{msgPool type=addButton}</button>
      </div>
      {/render}
    </div>
  </div>
</div>

<script language="JavaScript" type="text/javascript">
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
</script>
