<h3>{t}Welcome to the GOsa self service!{/t}</h3>
<hr>

{if $personalInfoAllowed}

    <h3>{t}Information about you{/t}:</h3>

    <table width="100%">
        <tr>
            <td style="width:33%">
                <table>
                    {if $uid != ""}<tr><td>{t}User-ID{/t}:</td><td>{$uid}</td></tr>{/if}
                    {if $sn != ""}<tr><td>{t}Surname{/t}:</td><td>{$sn}</td></tr>{/if}
                    {if $givenName != ""}<tr><td>{t}Given name{/t}:</td><td>{$givenName}</td></tr>{/if}
                    {if $personalTitle != ""}<tr><td>{t}Personal title{/t}:</td><td>{$personalTitle}</td></tr>{/if}
                    {if $academicTitle != ""}<tr><td>{t}Academic title{/t}:</td><td>{$academicTitle}</td></tr>{/if}

                    {if $dateOfBirth != ""}<tr><td>{t}Date of birth{/t}:</td><td>{$dateOfBirth}</td></tr>{/if}
                    {if $homePhone != ""}<tr><td>{t}Home phone{/t}:</td><td>{$homePhone}</td></tr>{/if}
                    {if $homePostalAddress != ""}<tr><td>{t}Home postal address{/t}:</td><td><pre>{$homePostalAddress}</pre></td></tr>{/if}
                </table>
            </td>
            <td style="width:33%">
                <table>
                    {if $o != ""}<tr><td>{t}Organization{/t}:</td><td>{$o}</td></tr>{/if}
                    {if $ou != ""}<tr><td>{t}Organizational unit{/t}:</td><td>{$ou}</td></tr>{/if}
                    {if $l != ""}<tr><td>{t}Location{/t}:</td><td>{$l}</td></tr>{/if}
                    {if $street != ""}<tr><td>{t}Street{/t}:</td><td>{$street}</td></tr>{/if}
                    {if $departmentNumber != ""}<tr><td>{t}Department number{/t}:</td><td>{$departmentNumber}</td></tr>{/if}

                    {if $employeeNumber != ""}<tr><td>{t}Employee number{/t}:</td><td>{$employeeNumber}</td></tr>{/if}
                    {if $employeeType != ""}<tr><td>{t}Employee type{/t}:</td><td>{$employeeType}</td></tr>{/if}

                </table>
            </td>
            {if $jpegPhoto}
                <td>
                    <img  src="getbin.php?rand={$rand}" alt='' style='max-width:147px; max-height: 200px; vertical-align: middle;' >
                </td>
            {/if}
        </tr>
    </table>

{/if}

{if $plugins != ""}
<hr>
<h3>{t}Management shortcuts{/t}</h3>
    {$plugins}
    <div class="clear"></div>
{/if}

{if $managersCnt != 0}
    <hr>
    <h3>{t}If you need assistance with this interface, please contact your administrative contact{/t}:</h3>
    {foreach from=$managers item=item}
        <div style='float: left; width:50%;'>
        {$item.str}
        </div>
    {/foreach}
{/if}
<hr>
