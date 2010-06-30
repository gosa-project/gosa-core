
<table style='width:100%' summary="{t}GOsa dash board{/t}">
    <tr>
        <td style='width:50%; padding-right:5px;' class='right-border'>
            
            <!-- Channel -->    

            {$dbChannelStatus}

            <!--Chanel  END -->

        </td>
        <td>
            
            <!-- Plugins Status -->    

            {$dbPluginStatus}

            <!-- Plugins Status  END -->

        </td>
    </tr>
    <tr><td colspan="2"><hr></td></tr>
    <tr>
        <td style='width:50%; padding-right:5px;' class='right-border'>
            
            <!-- Messages -->    
    
            {$dbNotifications}

            <!-- Messages  END -->

        </td>
        <td>
            
            <!-- RSS Feeds -->    
    
            {$dbInformation}

            <!-- RSS Feeds  END -->

        </td>
    </tr>
</table>
