<div class="content-wrapper card-content-scroll">
	<p>
		{t}GOsa is developed under the terms of the GNU General Public License v2. Please accept the terms below.{/t}
	</p>

	<div class='setup-license card-content-scroll'>
		{$License}
	</div>

	<label>
		<p>
			<input {if $accepted} checked {/if} type="checkbox" name="accepted"/>
			<span>{t}I have read the license and accept it{/t}</span>
		</p>
	</label>

	<input type='hidden' name='step_license' value='1'>
</div>
