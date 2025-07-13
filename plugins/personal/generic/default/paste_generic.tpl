<h2>{t}User settings{/t}</h2>

<div class="row content">
	<div class="col s6 xl5">
		<div class="input-field">
			<input type="text" name="sn" id="sn" value="{$sn}">
			<label for="sn">{t}Last name{/t}</label>
		</div>
		<div class="input-field">
			<input type="text" name="givenName" id="givenName" value="{$givenName}">
			<label for="givenName">{t}First name{/t}</label>
		</div>
		<div class="input-field">
			<input type="text" name="uid" id="uid" value="{$uid}">
			<label for="uid">{t}Login{/t}</label>
		</div>
		<div class="col s6 xl5">
			<h4>{t}Password{/t}</h4>
		</div>
		<div class="devider"></div>
		<div class="section">
			<div class="col s6 xl5 left-align">
				<p>
					<label>
						<input type="radio" name="passwordTodo" {if $passwordTodo=="clear" } checked{/if} value="clear">
						<span>{t}Clear password{/t}</span>
					</label>
				</p>
				<p>
					<label>
						<input type="radio" name="passwordTodo" {if $passwordTodo=="new" } checked{/if} value="new">
						<span>{t}Set new password{/t}</span>
					</label>
				</p>

			</div>
		</div>

	</div>
	<div class="col s6 xl5 user-image">
		<div class="grey preview-image">
			<img src="getbin.php?rand={$rand}" alt='' class="materialboxed">
		</div>

		<div class="image-action">
			<button class="btn-small" type='submit' name='picture_remove'>{t}Remove picture{/t} </button>
	
			<div class="file-field input-field">
				<div class="btn-small">
					<span>{t}Browse{/t}</span>
					<input type="hidden" name="MAX_FILE_SIZE" value="2000000">
					<input id="picture_file" name="picture_file" type="file" accept="image/*.jpg">
				</div>
				<div class="file-path-wrapper">
					<input placeholder='{t}No picture selected{/t}' type="text" class="file-path validate">
				</div>
			</div>
		</div>
	</div>
</div>