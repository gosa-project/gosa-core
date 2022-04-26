<div class="information-wrapper">
    {if $personalInfoAllowed}

    <h3>{t}User information{/t}</h3>

    <div class="row content">
        <div class="col s12 xl2 center-align user-image">
            {if $jpegPhoto == ""}
            <img src="plugins/users/images/default.jpg" alt='' class="matialboxed">
            {else}
            <img src="getbin.php?rand={$rand}" alt='' class="materialboxed">
            {/if}
        </div>

        <div class="col s6 xl5">

            <table class="striped">
                <tbody>
                    {if $uid != ""}
                    <tr>
                        <td>{t}User ID{/t}:</td>
                        <td><i>{$uid}</i></td>
                    </tr>
                    {/if}
                    {if $sn != ""}
                    <tr>
                        <td>{t}Surname{/t}:</td>
                        <td><i>{$sn}</i></td>
                    </tr>
                    {/if}
                    {if $givenName != ""}
                    <tr>
                        <td>{t}Given name{/t}:</td>
                        <td><i>{$givenName}</i></td>
                    </tr>
                    {/if}
                    {if $personalTitle != ""}
                    <tr>
                        <td>{t}Personal title{/t}:</td>
                        <td><i>{$personalTitle}</i></td>
                    </tr>
                    {/if}
                    {if $academicTitle != ""}
                    <tr>
                        <td>{t}Academic title{/t}:</td>
                        <td><i>{$academicTitle}</i></td>
                    </tr>
                    {/if}
                    {if $homePostalAddress != ""}
                    <tr>
                        <td>{t}Home postal address{/t}:</td>
                        <td><i>{$homePostalAddress}</i></td>
                    </tr>
                    {/if}
                    {if $dateOfBirth != ""}
                    <tr>
                        <td>{t}Date of birth{/t}:</td>
                        <td><i>{$dateOfBirth}</i></td>
                    </tr>
                    {/if}
                    {if $mail != ""}
                    <tr>
                        <td style="padding-top:15px">{t}Mail{/t}:</td>
                        <td style="padding-top:15px"><i>{$mail}</i></td>
                    </tr>
                    {/if}
                    {if $homePhone != ""}
                    <tr>
                        <td>{t}Home phone number{/t}:</td>
                        <td><i>{$homePhone}</i></td>
                    </tr>
                    {/if}
                </tbody>
            </table>
        </div>

        <div class="col s6 xl5">
            <table>
                <tbody>

                    {if $o != ""}
                    <tr>
                        <td>{t}Organization{/t}:</td>
                        <td><i>{$o}</i></td>
                    </tr>
                    {/if}
                    {if $ou != ""}
                    <tr>
                        <td>{t}Organizational Unit{/t}:</td>
                        <td><i>{$ou}</i></td>
                    </tr>
                    {/if}
                    {if $l != ""}
                    <tr>
                        <td style="padding-top:15px">{t}Location{/t}:</td>
                        <td style="padding-top:15px"><i>{$l}</i></td>
                    </tr>
                    {/if}
                    {if $street != ""}
                    <tr>
                        <td>{t}Street{/t}:</td>
                        <td><i>{$street}</i></td>
                    </tr>
                    {/if}
                    {if $departmentNumber != ""}
                    <tr>
                        <td style="padding-top:15px">{t}Department number{/t}:</td>
                        <td style="padding-top:15px"><i>{$departmentNumber}</i></td>
                    </tr>
                    {/if}
                    {if $employeeNumber != ""}
                    <tr>
                        <td style="padding-top:15px">{t}Employee number{/t}:</td>
                        <td style="padding-top:15px"><i>{$employeeNumber}</i></td>
                    </tr>
                    {/if}
                    {if $employeeType != ""}
                    <tr>
                        <td>{t}Employee type{/t}:</td>
                        <td><i>{$employeeType}</i></td>
                    </tr>
                    {/if}
                </tbody>
            </table>

        </div>
    </div>
    {/if}

    <hr class="divider">

    {if $plugins != ""}
    <div class="row">
        <h3>{t}User settings{/t}</h3>
        {$plugins}
    </div>
    <hr class="divider">
    {/if}


    {if !$personalInfoAllowed && $plugins == ""}
    <div class="row">
        <div class="no-permission attention-head">
            <i class="material-icons left">error</i>
            <h2>{t}You have no permission to edit any properties. Please contact your administrator.{/t}</h2>
        </div>
    </div>
    <hr class="divider">
    {/if}

    {if $managersCnt != 0}
    <div class="row">
        <h3>{t}Administrative contact{/t}</h3>
        {foreach from=$managers item=item}
        <div>
            {$item.str}
        </div>
        {/foreach}
    </div>
    {/if}
</div>

<div class="copynotice">&copy; 2002-{$year} <a href="http://gosa.gonicus.de">{t}The GOsa team{/t}, {$revision}</a></div>
<input type="hidden" name="ignore">