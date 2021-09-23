<div class="webdav-wrapper">
	{render acl=$webdavAccountACL checkbox=$multiple_support checked=$use_webdav}
	<label class="card-title">
		<input type="checkbox" name="webdav" value="B" {$webdavState} class="center">
		<span style="overflow: hidden;"><h2>{t}WebDAV account{/t}</h2></span>
	</label>
	{/render}
</div>

