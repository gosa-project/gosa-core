<div style="padding:3px">
{if $personalInfoAllowed}

    <h3>{t}User information{/t}</h3>

    <table width="100%">
        <tr>
            <td>
                    <img src="getbin.php?rand={$rand}" alt='' style='max-width:147px; max-height: 200px; vertical-align: middle;' >
            </td>
            <td style="width:33%">
                <table>
                    {if $uid != ""}<tr><td>{t}User ID{/t}:</td><td>{$uid}</td></tr>{/if}
                    {if $sn != ""}<tr><td>{t}Surname{/t}:</td><td>{$sn}</td></tr>{/if}
                    {if $givenName != ""}<tr><td>{t}Given name{/t}:</td><td>{$givenName}</td></tr>{/if}
                    {if $personalTitle != ""}<tr><td>{t}Personal title{/t}:</td><td>{$personalTitle}</td></tr>{/if}
                    {if $academicTitle != ""}<tr><td>{t}Academic title{/t}:</td><td>{$academicTitle}</td></tr>{/if}
                    {if $homePostalAddress != ""}<tr><td style="padding-top:15px">{t}Home postal address{/t}:</td><td style="padding-top:15px">{$homePostalAddress}</td></tr>{/if}
                    {if $dateOfBirth != ""}<tr><td style="padding-top:15px">{t}Date of birth{/t}:</td><td style="padding-top:15px">{$dateOfBirth}</td></tr>{/if}
                    {if $mail != ""}<tr><td style="padding-top:15px">{t}Mail{/t}:</td><td style="padding-top:15px">{$mail}</td></tr>{/if}
                    {if $homePhone != ""}<tr><td>{t}Home phone{/t}:</td><td>{$homePhone}</td></tr>{/if}
                </table>
            </td>
            <td style="border-left:1px solid #CCC; padding-left:10px">
                <table>
                    {if $o != ""}<tr><td>{t}Organization{/t}:</td><td>{$o}</td></tr>{/if}
                    {if $ou != ""}<tr><td>{t}Organizational unit{/t}:</td><td>{$ou}</td></tr>{/if}
                    {if $l != ""}<tr><td style="padding-top:15px">{t}Location{/t}:</td><td style="padding-top:15px">{$l}</td></tr>{/if}
                    {if $street != ""}<tr><td>{t}Street{/t}:</td><td>{$street}</td></tr>{/if}
                    {if $departmentNumber != ""}<tr><td style="padding-top:15px">{t}Department number{/t}:</td><td style="padding-top:15px">{$departmentNumber}</td></tr>{/if}

                    {if $employeeNumber != ""}<tr><td style="padding-top:15px">{t}Employee number{/t}:</td><td style="padding-top:15px">{$employeeNumber}</td></tr>{/if}
                    {if $employeeType != ""}<tr><td>{t}Employee type{/t}:</td><td>{$employeeType}</td></tr>{/if}

                </table>
            </td>
        </tr>
    </table>

{/if}

{if $plugins != ""}
<hr>
<h3>{t}User settings{/t}</h3>
    {$plugins}
    <div class="clear"></div>
{/if}

{if $managersCnt != 0}
    <hr>
    <h3>{t}Administrative contact{/t}</h3>
    {foreach from=$managers item=item}
        <div style='float:left; width:50%;'>
        {$item.str}
        </div>
    {/foreach}
 </div>
{/if}

<div class="clear"></div>
<hr>
<div class="copynotice">&copy; 2002-{$year} <a href="http://gosa.gonicus.de">{t}The GOsa team{/t}, {$revision}</a></div>
<input type="hidden" name="ignore">

