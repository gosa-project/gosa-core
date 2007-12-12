{if $readable}
<table width="100%" summary="">
	<tr>
		<td>{t}Argument{/t}
		</td>
		<td>{t}Name{/t}
		</td>
		<td>{t}type{/t}
		</td>
		<td>{t}Default value{/t}
		</td>
		<td>
		&nbsp;
		</td>
	</tr>
		{$vars}
</table>
{else}
	<h2>{t}You are not allowed to view the macro parameter settings{/t}</h2>
{/if}
<input type="hidden" name="phoneparemeters">

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('cn');
  -->
</script>
