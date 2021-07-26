<div style="padding:3px">
{if $personalInfoAllowed}

    <h3>{t}User information{/t}</h3>

    <table width="100%">
        <tr>
            <td style='width:200px; vertical-align: middle;' >
                {if $jpegPhoto == ""}
                    <img  src="plugins/users/images/default.jpg" alt=''>
                {else}
                    <img  src="getbin.php?rand={$rand}" alt='' style='border:1px solid #CCC; max-width:147px; max-height: 200px; vertical-align: middle;' >
                {/if}
            </td>
            <td style="width:40%">
                <table>
                    {if $uid != ""}<tr><td>{t}User ID{/t}:</td><td><i>{$uid}</i></td></tr>{/if}
                    {if $sn != ""}<tr><td>{t}Surname{/t}:</td><td><i>{$sn}</i></td></tr>{/if}
                    {if $givenName != ""}<tr><td>{t}Given name{/t}:</td><td><i>{$givenName}</i></td></tr>{/if}
                    {if $personalTitle != ""}<tr><td>{t}Personal title{/t}:</td><td><i>{$personalTitle}</i></td></tr>{/if}
                    {if $academicTitle != ""}<tr><td>{t}Academic title{/t}:</td><td><i>{$academicTitle}</i></td></tr>{/if}
                    {if $homePostalAddress != ""}<tr><td style="padding-top:15px">{t}Home postal address{/t}:</td><td style="padding-top:15px"><i>{$homePostalAddress}</i></td></tr>{/if}
                    {if $dateOfBirth != ""}<tr><td style="padding-top:15px">{t}Date of birth{/t}:</td><td style="padding-top:15px"><i>{$dateOfBirth}</i></td></tr>{/if}
                    {if $mail != ""}<tr><td style="padding-top:15px">{t}Mail{/t}:</td><td style="padding-top:15px"><i>{$mail}</i></td></tr>{/if}
                    {if $homePhone != ""}<tr><td>{t}Home phone number{/t}:</td><td><i>{$homePhone}</i></td></tr>{/if}
                </table>
            </td>
            <td style="border-left:1px solid #CCC; padding-left:10px">
                <table>
                    {if $o != ""}<tr><td>{t}Organization{/t}:</td><td><i>{$o}</i></td></tr>{/if}
                    {if $ou != ""}<tr><td>{t}Organizational Unit{/t}:</td><td><i>{$ou}</i></td></tr>{/if}
                    {if $l != ""}<tr><td style="padding-top:15px">{t}Location{/t}:</td><td style="padding-top:15px"><i>{$l}</i></td></tr>{/if}
                    {if $street != ""}<tr><td>{t}Street{/t}:</td><td><i>{$street}</i></td></tr>{/if}
                    {if $departmentNumber != ""}<tr><td style="padding-top:15px">{t}Department number{/t}:</td><td style="padding-top:15px"><i>{$departmentNumber}</i></td></tr>{/if}

                    {if $employeeNumber != ""}<tr><td style="padding-top:15px">{t}Employee number{/t}:</td><td style="padding-top:15px"><i>{$employeeNumber}</i></td></tr>{/if}
                    {if $employeeType != ""}<tr><td>{t}Employee type{/t}:</td><td><i>{$employeeType}</i></td></tr>{/if}

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

{if !$personalInfoAllowed && $plugins == ""}
    <div style='width:100%;text-align:center;padding-top:100px;padding-bottom:100px'>
    <b>{t}You have no permission to edit any properties. Please contact your administrator.{/t}</b>
    </div>
{/if}

{if $managersCnt != 0}
    <hr>
    <h3>{t}Administrative contact{/t}</h3>
    {foreach from=$managers item=item}
        <div style='float:left; padding-right:20px;'>
        {$item.str}
        </div>
    {/foreach}
 </div>
{/if}

<div class="clear"></div>
<hr>
<div class="copynotice">&copy; 2002-{$year} <a href="http://gosa.gonicus.de">{t}The GOsa team{/t}, {$revision}</a></div>
<input type="hidden" name="ignore">

