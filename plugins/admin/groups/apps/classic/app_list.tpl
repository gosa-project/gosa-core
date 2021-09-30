<table style="width:100%;" summary="{t}Group application settings{/t}">
	{if $enableReleaseManagement}
	<tr>
		<td>
			{t}Release{/t}
			<select name="FAIrelease" onChange="document.mainform.submit();" size=1>
			{foreach from=$releases item=item key=key}
				<option value="{$key}" {if $key == $FAIrelease} selected {/if}>{$item.name} </option>
			{/foreach}
			</select>
			{image path="images/lists/copy.png" action="menu_copy" title="{t}Copy menu{/t}"}

			{if $copied}
				{image path="images/lists/paste.png" action="menu_paste" title="{t}Paste menu from{/t}&nbsp;{$copy_source}"}

			{else}
				{image path="images/lists/paste-grey.png"}

			{/if}
			{image path="images/lists/delete.png" action="menu_delete" title="{t}Delete menu{/t}"}

		</td>
	</tr>
	{/if}
	<tr>
		<td style='width:50%; '>

		<div style="height:290px; overflow:auto; 
					border-top: solid 2px #999999;
					border-left: solid 2px #999999;
					padding:5px; 
					">
			
<table style='width:100%' cellpadding=0 cellspacing=0 summary="{t}Application menu{/t}">
{foreach from=$entries item=item key=key}
	{if $item.TYPE == "OPEN"}
		<tr>
			<td colspan=3 style="background-color: #DDDDDD;height:1px"></td>
		</tr>
		<tr>
			<td style='padding-left:20px;' colspan=3>
				<table style='width:100%;' cellpadding=0 cellspacing=0 summary="{t}Sub entry{/t}">

	{elseif $item.TYPE == "CLOSE"}
				</table>
			</td>
		</tr>
		<tr>
			<td colspan=3 style="background-color: #DDDDDD;height:1px"></td>
		</tr>
	{elseif $item.TYPE == "RELEASE"}
		{if $i++ % 2 == 0}
			<tr class="rowxp0">
		{else}
			<tr class="rowxp1">
		{/if}
			<td style='width:20px; padding-top:5px;padding-bottom:5px;background-color: transparent;'>
				{image path="plugins/fai/images/fai_small.png"}

			</td>
			<td style='background-color: transparent;'>
				{$item.NAME}
			</td>
			<td style='width:100px;text-align:right;background-color: transparent;'>
			</td>
		</tr>
	{elseif $item.TYPE == "FOLDER"}
		{if $i++ %2 == 0}
			<tr class="rowxp0">
		{else}
			<tr class="rowxp1">
		{/if}
			<td width="22" style='width:22px; padding-top:3px;padding-bottom:3px; overflow:hidden;'>
				{if $item.ICON != ""}
					<div style="height:20px;width:20px; overflow:hidden;">
					{image path="?plug={$plug_id}&amp;send={$item.UNIQID}"}

					</div>
				{else}
					<div style="height:20px;width:20px; overflow:hidden">
					{image path="images/lists/folder.png"}

					</div>
				{/if}
			</td>
			<td style='background-color: transparent;'>
				<b>{$item.NAME}&nbsp; </b> 
			</td>
			<td style='width:100px;text-align:right; background-color: transparent;'>
				{image title="{t}Move up{/t}" action="up_{$item.UNIQID}" path='images/lists/sort-up.png'}
				{image title="{t}Move down{/t}" action="down_{$item.UNIQID}" path='images/lists/sort-down.png'}
				{image title="{t}Edit{/t}" action="app_entry_edit{$item.UNIQID}" path='images/lists/edit.png'}
				{image title="{t}Remove{/t}" action="del_{$item.UNIQID}" path='images/lists/trash.png'}
			</td>
		</tr>
	{elseif $item.TYPE == "SEPERATOR"}

		{if $i++ % 2 == 0}
			<tr class="rowxp0">
		{else}
			<tr class="rowxp1">
		{/if}
			<td style='background-color: transparent;width:22px; padding-top:8px;padding-bottom:8px;' colspan="2">
				<div style="height:3px; width:100%; background-color:#BBBBBB;"></div>
			</td>
            <td style='width:100px;text-align:right; background-color: transparent;'>
                {image title="{t}Move up{/t}" action="up_{$item.UNIQID}" path='images/lists/sort-up.png'}
                {image title="{t}Move down{/t}" action="down_{$item.UNIQID}" path='images/lists/sort-down.png'}
                {image title="{t}Remove{/t}" action="del_{$item.UNIQID}" path='images/lists/trash.png'}
            </td>
		</tr>
	{elseif $item.TYPE == "ENTRY"}

		{if $i++ % 2 == 0}
			<tr class="rowxp0">
		{else}
			<tr class="rowxp1">
		{/if}
			<td style='background-color: transparent;width:22px; padding-top:5px;padding-bottom:5px;'>
				<div style="width:20px; overflow:hidden; text-align:center;">
					{image path="plugins/goto/images/select_application.png"}

				</div>
			</td>
			<td style="background-color: transparent;">
				{$item.NAME} {$item.INFO}
			</td>
			<td style='width:100px;text-align:right;background-color: transparent;'>
				{image title="{t}Move up{/t}" action="up_{$item.UNIQID}" path='images/lists/sort-up.png'}
				{image title="{t}Move down{/t}" action="down_{$item.UNIQID}" path='images/lists/sort-down.png'}
				{image title="{t}Edit{/t}" action="app_entry_edit{$item.UNIQID}" path='images/lists/edit.png'}
				{image title="{t}Remove{/t}" action="del_{$item.UNIQID}" path='images/lists/trash.png'}
			</td>
		</tr>
	{/if}
{/foreach}
</table>
		</div>
			<input type="text" name="menu_folder_name" value="">
			{t}add to{/t}
			<select name="menu_folder" size=1>
			{foreach from=$folders item=item key=key}
				<option value="{$key}">{$item}</option>
			{/foreach}
			</select>
			<button type='submit' name='add_menu_to_folder' title="{t}Add selected applications to this folder.{/t}">{msgPool type=addButton}</button>

			<button type='submit' name='add_seperator' title="{t}Add a separator to this folder.{/t}">{t}Separator{/t}</button>

		</td>
		<td>

			{$app_list}	
			<select name="folder" size=1>
			{foreach from=$folders item=item key=key}
				<option value="{$key}">{$item}</option>
			{/foreach}
			</select>
			<button type='submit' name='add_to_folder' title="{t}Add selected applications to this folder.{/t}">{msgPool type=addButton}</button>

		</td>
	</tr>
</table>
	
