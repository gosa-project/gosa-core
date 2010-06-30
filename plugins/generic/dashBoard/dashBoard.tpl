
<table style='width:100%' summary="{t}GOsa dash board{/t}">
    <tr>
        <td style='width:50%;' class='right-border'>
            
            <!-- Channel -->    
            <h3>{t}Channels{/t}</h3> 

            {$dbChannelStatus}

            <!--Chanel  END -->

        </td>
        <td>
            
            <!-- Plugins Status -->    
            <h3>{t}Plugin status{/t}</h3> 

            {$dbPluginStatus}

            <!-- Plugins Status  END -->

        </td>
    </tr>
    <tr><td colspan="2"><hr></td></tr>
    <tr>
        <td style='width:50%;' class='right-border'>
            
            <!-- Messages -->    
            <h3>{t}Advices{/t}</h3> 
    
            {$dbAdvices}

            <!-- Messages  END -->

        </td>
        <td>
            
            <!-- RSS Feeds -->    
            <h3>{t}Information{/t}</h3> 
    
            {$dbInformation}

            <!-- RSS Feeds  END -->

        </td>
    </tr>
</table>
