<table>
    <tr>
        <td>
            <a href="plugins/gofax/getfax.php?id={$detail}&amp;download=1">
                <img src='plugins/gofax/getfax.php?id={$detail}' align="bottom" alt="{t}Click on fax to download{/t}">
                <br> 
                {t}Click on fax to download{/t}
            </a>
        </td>
        <td>
            <table>
                <tr>
                    <td><b>{t}FAX ID{/t}</b></td>
                    <td>{$fax_id}</td>
                </tr>
                <tr>
                    <td><b>{t}User{/t}</b></td>
                    <td>{$fax_uid}</td>
                </tr>
                <tr>
                    <td><b>{t}Date / Time{/t}</b></td>
                    <td>{$date} / {$time}</td>
                </tr>
                <tr>
                    <td><b>{t}Sender MSN{/t}</b></td>
                    <td>{$fax_sender_msn}</td>
                </tr>
                <tr>
                    <td><b>{t}Sender ID{/t}</b></td>
                    <td>{$fax_sender_id}</td>
                </tr>
                <tr>
                    <td><b>{t}Receiver MSN{/t}</b></td>
                    <td>{$fax_receiver_msn}</td>
                </tr>
                <tr>
                    <td><b>{t}Receiver ID{/t}</b></td>
                    <td>{$fax_receiver_id}</td>
                </tr>
                <tr>
                    <td><b>{t}Status{/t}</b></td>
                    <td>{$fax_status}</td>
                </tr>
                <tr>
                    <td><b>{t}Status message{/t}</b></td>
                    <td>{$fax_status_message}</td>
                </tr>
                <tr>
                    <td><b>{t}Transfer time{/t}</b></td>
                    <td>{$fax_transfer_time}</td>
                </tr>
                <tr>
                    <td><b>{t}# pages{/t}</b></td>
                    <td>{$fax_pages}</td>
                </tr>
            </table>
        </td>
    </tr>
</table>

<hr>
<div class="card-action">
    <button class="btn-small" type='submit' name='bck_to_list'>{msgPool type=backButton}</button>
</div>