<div class="app-wrapper">
	<div class="row">
		<h3>{t}Group application settings{/t}</h3>

		<div class="col s12 xl6">
			{if $enableReleaseManagement}
			
			<div class="input-field add">
				<select name="FAIrelease" onChange="document.mainform.submit();" size=1>
					{foreach from=$releases item=item key=key}
					<option value="{$key}" {if $key == $FAIrelease} selected {/if}>{$item.name} </option>
					{/foreach}
				</select>
				<label>{t}Release{/t}</label>
				{image path="<i class='material-icons input-icons'>content_copy</i>" action="menu_copy" title="{t}Copy menu{/t}"}
	
				{if $copied}
					{image path="<i class='material-icons input-icons'>content_paste</i>" action="menu_paste" title="{t}Paste menu from{/t}&nbsp;{$copy_source}"}
	
				{else}
					{image path="<i class='material-icons text-grey'>content_paste</i>"}
	
				{/if}
				{image path="<i class='material-icons input-icons'>delete</i>" action="menu_delete" title="{t}Delete menu{/t}"}
			</div>
			{/if}

			<table class="striped">
				<tbody>
					{foreach from=$entries item=item key=key}
					{if $item.TYPE == "OPEN"}
					<tr>
						<td colspan=3></td>
					</tr>
					<tr>
						<td style='padding-left:20px;' colspan=3>
							<table cellpadding=0 cellspacing=0>
								<tbody>
					{elseif $item.TYPE == "ENTRY"}
									<tr>
										<td>
											<div>
												{image path="<i class='material-icons'>apps</i>"}
											</div>
										</td>
										<td>
											{$item.NAME} {$item.INFO}
										</td>
										<td>
											<div class="inline-icons">
												{image title="{t}Edit{/t}" action="app_entry_edit{$item.UNIQID}" path='<i class="material-icons input-icons">edit</i>'}
												{image title="{t}Remove{/t}" action="del_{$item.UNIQID}" path='<i class="material-icons input-icons">delete</i>'}
												{image title="{t}Move up{/t}" action="up_{$item.UNIQID}" path='<i class="material-icons input-icons">keyboard_arrow_up</i>'}
												{image title="{t}Move down{/t}" action="down_{$item.UNIQID}" path='<i class="material-icons input-icons">keyboard_arrow_down</i>'}
											</div>
										</td>
									</tr>
					{elseif $item.TYPE == "FOLDER"}
									<tr>
										<td>
											{if $item.ICON != ""}
											<div>
												{image path="?plug={$plug_id}&amp;send={$item.UNIQID}"}
											</div>
											{else}
											<div>
												{image path="<i class='material-icons'>folder</i>"}
											</div>
											{/if}
										</td>
										<td>
											<b>{$item.NAME}&nbsp; </b> 
										</td>
										<td>
											<div class="inline-icons">
												{image title="{t}Edit{/t}" action="app_entry_edit{$item.UNIQID}" path='<i class="material-icons input-icons">edit</i>'}
												{image title="{t}Remove{/t}" action="del_{$item.UNIQID}" path='<i class="material-icons input-icons">delete</i>'}
												{image title="{t}Move up{/t}" action="up_{$item.UNIQID}" path='<i class="material-icons input-icons">keyboard_arrow_up</i>'}
												{image title="{t}Move down{/t}" action="down_{$item.UNIQID}" path='<i class="material-icons input-icons">keyboard_arrow_down</i>'}
											</div>
											
										</td>
									</tr>
					{elseif $item.TYPE == "RELEASE"}
									<tr>
										<td>
											{image path="fai"}
										</td>
										<td>
											{$item.NAME}
										</td>
										<td>
										</td>
									</tr>
					{elseif $item.TYPE == "SEPERATOR"}
									<tr>
										<td style="display: flex;">
											<div class="seperator" style="flex-grow: 1;"></div>
										</td>
										<td></td>
										<td>
											<div class="inline-icons">
												{image title="{t}Remove{/t}" action="del_{$item.UNIQID}" path='<i class="material-icons input-icons">delete</i>'}
												{image title="{t}Move up{/t}" action="up_{$item.UNIQID}" path='<i class="material-icons input-icons">keyboard_arrow_up</i>'}
												{image title="{t}Move down{/t}" action="down_{$item.UNIQID}" path='<i class="material-icons input-icons">keyboard_arrow_down</i>'}
											</div>
										</td>
									</tr>
					{elseif $item.TYPE == "CLOSE"}
								</tbody>
							</table>
						</td>
					</tr>
					<tr>
						<td colspan=3></td>
					</tr>
					{/if}
					{/foreach}
				</tbody>
			</table>

			<div class="input-field">
				<input type="text" name="menu_folder_name" value="" id="menu_folder_name">
				<label for="menu_folder_name">{t}add to{/t}</label>
			</div>

			<div class="input-field add">
				<select name="menu_folder" size=1>
				{foreach from=$folders item=item key=key}
					<option value="{$key}">{$item}</option>
				{/foreach}
				</select>
				<button class="btn-small" type='submit' name='add_menu_to_folder' title="{t}Add selected applications to this folder.{/t}">{msgPool type=addButton}</button>
				<button class="btn-small" type='submit' name='add_seperator' title="{t}Add a separator to this folder.{/t}">{t}Separator{/t}</button>
			</div>
		</div>

		<div class="col s12 xl6">
			{$app_list}

			<div class="input-field add">
				<select name="folder" size=1>
				{foreach from=$folders item=item key=key}
					<option value="{$key}">{$item}</option>
				{/foreach}
				</select>
				<button class="btn-small" type='submit' name='add_to_folder' title="{t}Add selected applications to this folder.{/t}">{msgPool type=addButton}</button>
			</div>
		</div>
	</div>
</div>
