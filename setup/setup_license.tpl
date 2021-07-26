<div>
 <p>
  {t}GOsa is developed under the terms of the GNU General Public License v2. Please accept the terms below.{/t}
 </p>
	<div class='default' style='margin:10px; border:1px solid #A0A0A0'>
		<div style='height:450px;padding:5px;overflow:auto; '>
			{$License}
		</div>
	</div>	
	<div class="container center-align">
		<p>
			<label class="black-text">
			  <input {if $accepted} checked {/if} type="checkbox" name="accepted"/>
			  <span>{t}I have read the license and accept it{/t}</span>
			</label>
		</p>
	</div>
	
</div>
<input type='hidden' name='step_license' value='1'>
