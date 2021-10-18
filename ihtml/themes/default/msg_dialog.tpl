{if $frame}
{if $IE}
<iframe id='e_layer3' style="
				position:absolute;
				width:100%;
				height:100%;
				top:0px;
				left:0px;
				border:none;
				border-style:none;
				border-width:0pt;
				display:block;
				allowtransparency='true';
				background-color: #FFFFFF;
				filter:chroma(color=#FFFFFF);
				z-index:100; ">
</iframe>
<div id='e_layer2'
	style="
				position: absolute;
				left: 0px;
				top: 0px;
				right:0px;
				bottom:0px;
				z-index:100;
				width:100%;
				height:100%;
				filter:progid:DXImageTransform.Microsoft.AlphaImageLoader(enabled=true, sizingMethod=scale, src='images/opacity_black.png'); ">
{else}
<div id='e_layer2'
	style="
				position: absolute;
				left: 0px;
				top: 0px;
				right:0px;
				bottom:0px;
				z-index:100;
				background-image: url(images/opacity_black.png);">
{/if}
{else}

{if !$JS}
{if $i_Type == $smarty.const.INFO_DIALOG || $i_Type == $smarty.const.CONFIRM_DIALOG || $i_Type == $smarty.const.OK_CANCEL_DIALOG}
	<div id='e_layer{$i_ID}' class="modal" style='top:200px;left:200px;'>
{else}
	<div id='e_layer{$i_ID}' class="modal" style='top:200px;left:200px;'>
{/if}
		<div id="e_layerTitle{$i_ID}" class="modal-content">
			<div class="attention-head">
				{if $i_Type == $smarty.const.ERROR_DIALOG}
					{image path='<i class="material-icons left">error</i>'}
				{elseif $i_Type == $smarty.const.WARNING_DIALOG}
					{image path='<i class="material-icons left">warning</i>'}
				{elseif $i_Type == $smarty.const.INFO_DIALOG || $i_Type == $smarty.const.CONFIRM_DIALOG ||
					$i_Type == $smarty.const.OK_CANCEL_DIALOG}
					{image path='<i class="material-icons left">info_outline</i>'}
				{/if}

				<h2>{$s_Title}</h2>
			</div>

			<div class="attention-content">
				<p>
					{$s_Message}
				</p>
			</div>
		</div>

		<div class="modal-footer">
		{if $i_Type == $smarty.const.ERROR_DIALOG || $i_Type == $smarty.const.WARNING_DIALOG || $i_Type == $smarty.const.INFO_DIALOG}
			<button class="btn-small primary" type='submit' name='MSG_OK{$i_ID}'>{t}OK{/t}</button>
		{elseif $buttononst.CONFIRM_DIALOG || $i_Type == $smarty.const.OK_CANCEL_DIALOG}
			<button class="btn-small primary" type='submit' name='MSG_OK{$i_ID}'>{t}OK{/t}</button>
			<button class="btn-small primary" type='submit' name='MSG_CANCEL{$i_ID}'>{t}Cancel{/t}</button>
		{/if}
		</div>
	</div>
{else}
{if $s_Trace != "" && $i_TraceCnt != 0}
	<div id='trace_{$i_ID}' style='display:none;'>
		{$s_Trace}
	</div>
{/if}

{if $i_Type == $smarty.const.INFO_DIALOG || $i_Type == $smarty.const.CONFIRM_DIALOG || $i_Type == $smarty.const.OK_CANCEL_DIALOG}
	<div id='e_layer{$i_ID}' class="modal" style='top:200px;left:200px;'>
{else}
	<div id='e_layer{$i_ID}' class="modal" style='top:200px;left:200px;'>
{/if}
		<div id="e_layerTitle{$i_ID}" class="modal-content attention-elem">
			<div class="attention-head">
				{if $i_Type == $smarty.const.ERROR_DIALOG}
					{image path='<i class="material-icons left">error</i>'}
				{elseif $i_Type == $smarty.const.WARNING_DIALOG}
					{image path='<i class="material-icons left">warning</i>'}
				{elseif $i_Type == $smarty.const.INFO_DIALOG || $i_Type == $smarty.const.CONFIRM_DIALOG ||
					$i_Type == $smarty.const.OK_CANCEL_DIALOG}
					{image path='<i class="material-icons left">info_outline</i>'}
				{/if}

				<h2>{$s_Title}</h2>
			</div>

			<div class="attention-content">
				<p>
					{$s_Message}
				</p>
			</div>
			{if $s_Trace != "" && $i_TraceCnt != 0}
				<div onClick="$('trace_{$i_ID}').toggle();"><u>Trace</u></div>
			{/if}
		</div>

		<div class="modal-footer">
		{if $i_Type == $smarty.const.ERROR_DIALOG || $i_Type == $smarty.const.WARNING_DIALOG || $i_Type == $smarty.const.INFO_DIALOG}
			<button class="btn-small primary" id='MSG_OK{$i_ID}' type='button' name='MSG_OK{$i_ID}' onClick='next_msg_dialog();'>{t}OK{/t}</button>
		{elseif $i_Type == $smarty.const.CONFIRM_DIALOG || $i_Type == $smarty.const.OK_CANCEL_DIALOG}
			<button class="btn-small primary" id='MSG_OK{$i_ID}' type='submit' name='MSG_OK{$i_ID}' onClick='next_msg_dialog();'>{t}OK{/t}</button>
			<button class="btn-small primary" type='button' name='MSG_CANCEL{$i_ID}' onClick='next_msg_dialog();'>{t}Cancel{/t}</button>
		{/if}
		</div>
	</div>

<script language="JavaScript" type="text/javascript">
	focus_field('MSG_OK{$i_ID}');
</script>

{/if}
{/if}
