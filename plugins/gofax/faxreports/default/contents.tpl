<div class="fax-report-wrapper">
    <h2>{t}Fax reports{/t}</h2>

    <div class="fax-control-panel">
        <div class="input-field add">
            <select size="1" name="month" onChange="mainform.submit()">
                {html_options options=$months selected=$month_select}
            </select>
            <label for="month">{t}Date{/t}:</label>

            <select size="1" name="year" onChange="mainform.submit()">
                {html_options values=$years output=$years selected=$year_select}
            </select>
        </div>

        <div class="input-field add search-field">
            <input type="text" name="search_for" id="search_for" maxlength=60 value="{$search_for}" title="{t}Enter user name to search for{/t}" onChange="mainform.submit()">
            <label for="search_for">{t}Search for{/t}:</label>
            <button class="btn-small" type="submit" name="search">{t}Search{/t}</button>
        </div>
    </div>

    <hr class="divider">

    {if $search_result ne ""}
        <div class="card-content-scroll">
            <table id="fax-report-table">
                <thead>
                    <tr>
                        <th class="listheader"><a href="main.php{$plug}&amp;sort=0">{t}User{/t}</a> {$mode0}</th>
                        <th class="listheader"><a href="main.php{$plug}&amp;sort=1">{t}Date{/t}</a> {$mode1}</th>
                        <th class="listheader"><a href="main.php{$plug}&amp;sort=2">{t}Status{/t}</a> {$mode2}</th>
                        <th class="listheader"><a href="main.php{$plug}&amp;sort=3">{t}Sender{/t}</a> {$mode3}</th>
                        <th class="listheader"><a href="main.php{$plug}&amp;sort=4">{t}Receiver{/t}</a> {$mode4}</th>
                        <th class="listheader"><a href="main.php{$plug}&amp;sort=5">{t}# pages{/t}</a> {$mode5}</th>
                        <th class="listheader">&nbsp;</th>
                    </tr>
                </thead>
                <tbody>
                    {$search_result}
                </tbody>
            </table>

            <table>
                <tbody>
                    <tr>
                        <td>{$range_selector}</td>
                    </tr>
                </tbody>
            </table>
        </div>

    {else}
        <p><b>{t}Search returned no results...{/t}</b></p>
    {/if}
</div>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
    // First input field on page
    focus_field('search_for');
</script>
