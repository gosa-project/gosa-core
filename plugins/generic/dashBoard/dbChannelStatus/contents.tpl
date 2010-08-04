<h3>{t}Transmit daliy stats{/t}

<button name='transmitStatistics'>{t}Send{/t}</button>
<button name='receiveStatistics'>{t}Receive{/t}</button>

{if $receivedStats}
    <h3>{t}Transmit daliy stats{/t}</h3>
    <table summary="{t}Generated usage statistics{/t}">
    {foreach from=$receivedStats key=key item=item}
        <tr>
            <td>{$key}</td>
            <td>{$item}</td>
        </tr>
    {/foreach}
    </table>
{/if}

<hr>

<h3>{t}Channels{/t}</h3> 

{$channelList}
