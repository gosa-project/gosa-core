<div class="faxaccount-wrapper">
    <div class="row">
        <div class="col s12 xl6">
            <h3>{t}Blocklist settings{/t}</h3>

            <div class="input-field">
                <select name="block_list[]" size=15 multiple>
                    {html_options values=$cblocklist output=$cblocklist}
                </select>
                <label>{t}Blocked numbers/lists{/t}</label>
            </div>

            <div class="input-field add">
                <input type='text' name="block_number" size=25 align="middle" maxlength=30 value="">
                <button class="btn-small" type='submit' name='add_blocklist_number'>{msgPool type=addButton}</button>
                <button class="btn-small" type='submit' name='delete_blocklist_number'>{msgPool type=delButton}</button>
            </div>
        </div>

        <div class="col s12 xl6">
            <h3>{t}List of predefined blacklists{/t}</h3>
            {$predefinedList}
            <button class="btn-small" type='submit' name='add_blocklist'>{t}Add the list to the blacklists{/t}</button>
        </div>
    </div>
</div>

<div class="card-action">
    <button class="btn-small primary" type='submit' name='edit_blocklists_finish'>{msgPool type=applyButton}</button>
    <button class="btn-small primary" type='submit' name='edit_blocklists_cancel'>{msgPool type=cancelButton}</button>
</div>
