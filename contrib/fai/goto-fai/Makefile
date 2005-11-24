all:
	@echo "Nothing to do for all"

install:
	mkdir -p $(DESTDIR)/usr/sbin
	mkdir -p $(DESTDIR)/etc/goto
	mkdir -p $(DESTDIR)/usr/lib/goto
	mkdir -p $(DESTDIR)/fai/hooks
	cp secret $(DESTDIR)/etc/goto
	cp -a get_fai_dir faimond $(DESTDIR)/usr/sbin
	cp -a goto-support.lib $(DESTDIR)/usr/lib/goto
	cp -a ldap2fai $(DESTDIR)/usr/sbin
	cp confdir.DEFAULT.source $(DESTDIR)/fai/hooks
	chmod go-rwx $(DESTDIR)/etc/goto/secret

	# Install diversions
	mkdir -p $(DESTDIR)/usr/lib/fai/sbin
	cp diversions/setup_harddisks $(DESTDIR)/usr/lib/fai/sbin

