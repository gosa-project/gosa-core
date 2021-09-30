
{if $is_new}

<table style='width:100%;' summary="{t}Goto daemon event: Reboot{/t}">
  <tr>
    <td style='width:50%; ' class='right-border'>

      <table summary="{t}Schedule options{/t}">
        <tr>
          <td>
<b>{t}Schedule{/t}</b><br><br>
          {$timestamp}</td>
        </tr>
      </table>
    </td>
    <td style='width:50%; '>

      <table style='width:100%;'  summary="{t}Target list{/t}">
        <tr>
          <td>
            <b>{t}System list{/t}</b>
            <br>
            {$target_list}
          </td>
        </tr>
      </table>
    </td>
  </tr>
</table>

{else}

<table style='width:100%;'  summary="{t}Repeating jobs{/t}">
	<tr>
		<td style='width:50%; '>

			<table  summary="{t}Job interval{/t}">
				<tr>
					<td>{t}ID{/t}</td>
					<td>{$data.ID}</td>
				</tr>
				<tr>
					<td>{t}Status{/t}</td>
					<td>{$data.STATUS}</td>
				</tr>
				<tr>
					<td>{t}Result{/t}</td>
					<td>{$data.RESULT}</td>
				</tr>
				<tr>
					<td>{t}Target{/t}</td>
					<td>{$data.MACADDRESS}</td>
				</tr>
				<tr>
					<td>{t}Time stamp{/t}
</td>
					<td>{$timestamp}</td>
				</tr>
			</table>
		</td>
	</tr>
</table>

{/if}
