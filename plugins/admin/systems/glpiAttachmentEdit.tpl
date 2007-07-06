<br>
<h2>{t}Attachment{/t}</h2>
	
<table summary="" width="100%">
	<tr>
		<td width="50%" style="vertical-align:top;">
			<table summary="" width="100%">
				<tr>
					<td  style="vertical-align:top;">
						{t}Name{/t}
					</td>
					<td>
						<input type="text" value="{$name}" name="name">
					</td>
				</tr>
				<tr>
					<td style="vertical-align:top;">
						{t}Comment{/t}
					</td>
					<td style="vertical-align:top;">
						<textarea name="comment">{$comment}</textarea>
					</td>
				</tr>
			</table>
		</td>
		<td  style="vertical-align:top;">
			<table summary="" width="100%">
				<tr>
					<td style="vertical-align:top;">
						{t}File{/t}
					</td>
					<td style="vertical-align:top;">
						<input type="file" value="" name="filename"><input type="submit" name="upload" value="{t}Upload{/t}">
					</td>
				</tr>
				<tr>
					<td style="vertical-align:top;">
						{t}Status{/t}
					</td>
					<td style="vertical-align:top;">
						{$status}
					</td>
				</tr>
				<tr>
					<td style="vertical-align:top;">
						{t}Filename{/t}
					</td>
					<td style="vertical-align:top;">
						{$filename}
					</td>
				</tr>
				<tr>
					<td style="vertical-align:top;">
						{t}Mime-type{/t}
					</td>
					<td style="vertical-align:top;">
						{$mime}
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>


<p class="seperator">&nbsp;</p>
<div align="right">
	<p>
		<input type="submit" name="SaveAttachment" 		value="{t}Save{/t}">
		<input type="submit" name="CancelAttachment" 	value="{t}Cancel{/t}">
	</p>
</div>
<script language="JavaScript" type="text/javascript">
  <!-- // First input field on page
	focus_field('name');
  -->
</script>

