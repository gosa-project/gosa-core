<div class="webdav-wrapper">
	{render acl=$webdavAccountACL checkbox=$multiple_support checked=$use_webdav}
		<label>
			<input type="checkbox" name="webdav" value="B" {$webdavState} class="center">
			<span style="overflow: hidden;"><h3>{t}WebDAV account{/t}</h3></span>
		</label>
	{/render}
</div>
