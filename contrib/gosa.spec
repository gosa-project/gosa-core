# Some sort of "detection" of suse
%{?suse_version:%define suse 1}
%{!?suse_version:%define suse 0}

# Define Packagename, e.g.:
# rpmbuild --rebuild --define 'sourcename gosa' gosa.srpm
%{!?sourcename:%define sourcename %{name}-%{version}}

#
# Distribution
#
Summary: 		Web Based LDAP Administration Program 
Name:			gosa
Version: 		2.5.99cvs
Release:		1
License: 		GPL
Source: 		ftp://oss.GONICUS.de/pub/gosa/%{sourcename}.tar.bz2
URL: 			http://oss.GONICUS.de/project/?group_id=6
Group: 			System/Administration
Vendor:			GONICUS GmbH
Packager:		Lars Scheiter <lars.scheiter@GONICUS.de>
Buildarch: 		noarch
%if %{suse}
Requires:		apache2,apache2-mod_php5,php5,php5-gd,php5-ldap,php5-mcrypt,php5-mysql,php5-imap,php5-iconv,php5-mbstring,php5-gettext,php5-session,ImageMagick
%else
Requires: 		httpd,php,php-ldap,php-imap,php-snmp,php-mysql,php-mbstring,ImageMagick
%endif
BuildRoot: 		%{_tmppath}/%{name}-%{version}-root
BuildArch:		noarch

%define confdir 	/etc/%{name}

%if %{suse}
	%{echo:Building SuSE rpm}
	%define apacheuser wwwrun
	%define apachegroup root
	%define webconf	/etc/apache2/conf.d/
	%define docdir /usr/share/doc/packages/gosa
%else
	%{echo:Building other rpm}
	%define apacheuser apache 
	%define apachegroup apache 
	%define webconf	/etc/httpd/conf.d/	
	%define docdir /usr/share/doc/gosa-%{version}
%endif

%description
GOsa is a combination of system-administrator and end-user web
interface, designed to handle LDAP based setups.
Provided is access to posix, shadow, samba, proxy, fax, and kerberos
accounts. It is able to manage the postfix/cyrus server combination
and can write user adapted sieve scripts.

%package schema
Group: 			System/Administration
Summary: 		Schema Definitions for the GOSA package
%if %{suse}
Requires:		openldap2 >= 2.1.22
%else
Requires:		openldap-servers >= 2.2.0
%endif
Obsoletes:		gosa-ldap

%description schema
Contains the Schema definition files for the GOSA admin package.

%package mkntpasswd
Group: 			System/Administration
Summary: 		Schema Definitions for the GOSA package
%if %{suse}
Requires:		perl-Crypt-SmbHash
%else
Requires:		perl-Crypt-SmbHash >= 0.02
%endif

%description mkntpasswd
Wrapper Script around perl to create Samba Hashes on the fly, added for completeness only.
If in doubt use sambas "native" mkntpwd tool to generate hashes for GOsa.

%package help-en
Group: 			System/Administration
Summary: 		English online manual for GOSA package
Requires:		gosa >= %{version}

%description help-en
English online manual page for GOSA package

%package help-de
Group: 			System/Administration
Summary: 		German localized online manual for GOSA package
Requires:		gosa >= %{version}

%description help-de
German localized online manual page for GOSA package

%package help-fr
Group: 			System/Administration
Summary: 		French localized online manual for GOSA package
Requires:		gosa >= %{version}

%description help-fr
French localized online manual page for GOSA package

%package help-nl
Group: 			System/Administration
Summary: 		Dutch localized online manual for GOSA package
Requires:		gosa >= %{version}

%description help-nl
Dutch localized online manual page for GOSA package

%prep
%setup -q -n %{sourcename}
find . -depth -name CVS -type d | xargs rm -rf

%build


%install
# Create buildroot
mkdir -p %{buildroot}/usr/share/gosa

# Copy
DIRS="doc ihtml plugins html include locale setup"
for i in $DIRS; do \
  cp -ua $i %{buildroot}/usr/share/gosa/$i ; \
done
mkdir %{buildroot}/usr/bin
cp bin/mkntpasswd %{buildroot}/usr/bin/

# Create files for temporary stuff
for i in compile config cache; do \
  mkdir -p %{buildroot}/var/spool/gosa/$i ; \
done

# Cleanup manual dirs
for i in admin devel; do \
  rm -rf %{buildroot}/usr/share/gosa/doc/guide/$i ; \
done

# Remove (some) unneeded files
for i in gen_locale.sh gen_online_help.sh gen_function_list.php update.sh; do \
 rm -rf %{buildroot}/usr/share/gosa/$i ; \
done

# Cleanup lyx warnings
find %{buildroot}/usr/share/gosa -name WARNINGS |xargs rm


# Cleanup guide
rm -rf %{buildroot}/usr/share/gosa/doc/guide/user/*/lyx-source


# Copy default config
mkdir -p %{buildroot}%{confdir}
mkdir -p %{buildroot}%{webconf}

cat > %{buildroot}%{webconf}/gosa_include.conf <<EOF
# Just to be sure
<Directory "/usr/share/gosa/html">
	Options None
	AllowOverride None
	Order allow,deny
	Allow from all
</Directory>
# Set alias to gosa
Alias /gosa /usr/share/gosa/html
EOF

mkdir -p %{buildroot}/etc/openldap/schema/gosa
mv contrib/openldap/*.schema %{buildroot}/etc/openldap/schema/gosa
sed 's%"CONFIG_TEMPLATE_DIR", "../contrib/"%"CONFIG_TEMPLATE_DIR", "%{docdir}/"%g' %{buildroot}/usr/share/gosa/include/functions.inc > %{buildroot}/usr/share/gosa/include/functions.inc.new
mv -f %{buildroot}/usr/share/gosa/include/functions.inc.new %{buildroot}/usr/share/gosa/include/functions.inc

mv -f doc manual
mkdir -p %{buildroot}/etc/gosa/vacation
mv -f %{buildroot}/usr/share/gosa/plugins/personal/mail/sieve-*.txt %{buildroot}/etc/gosa
mkdir -p %{buildroot}/usr/share/doc/gosa-%{version}
rm -rf %{buildroot}/usr/share/gosa/contrib
#rm -rf %{buildroot}/usr/share/gosa/doc
#rmdir contrib/openldap
bzip2 -9 contrib/opensides/goSamba.pl

%clean
rm -rf %{buildroot}

%post
# Add shells file to /etc/gosa 
/bin/cp /etc/shells /etc/gosa

%pre
# Cleanup compile dir on updates, always exit cleanly even on errors
[ -d /var/spool/gosa ] && rm -rf /var/spool/gosa/* ; exit 0

%postun
# Remove temporary files, just to be sure
[ -d /var/spool/gosa ] && rm -rf /var/spool/gosa/* ; exit 0

%files
%defattr(-,%{apacheuser},%{apachegroup})
%doc %attr(-,root,root) AUTHORS TODO README README.safemode Changelog COPYING INSTALL FAQ
%doc %attr(-,root,root) contrib/altlinux contrib/fix_config.sh contrib/gosa.conf contrib/mysql contrib/opensides
%doc %attr(-,root,root) contrib/patches contrib/scripts contrib/vacation_example.txt contrib/demo.ldif contrib/openldap

%config(noreplace) %attr(0600,%{apacheuser},%{apachegroup}) %{webconf}/gosa_include.conf
%config(noreplace) %attr(0700,%{apacheuser},%{apachegroup}) /etc/gosa
%attr(0700, %{apacheuser}, %{apachegroup}) /var/spool/gosa
%attr(0744, %{apacheuser}, %{apachegroup}) /usr/share/gosa/html
%attr(0744, %{apacheuser}, %{apachegroup}) /usr/share/gosa/ihtml
%attr(0744, %{apacheuser}, %{apachegroup}) /usr/share/gosa/include
%attr(0744, %{apacheuser}, %{apachegroup}) /usr/share/gosa/locale
%attr(0744, %{apacheuser}, %{apachegroup}) /usr/share/gosa/setup
%attr(0744, %{apacheuser}, %{apachegroup}) /usr/share/gosa/plugins
%attr(0744, %{apacheuser}, %{apachegroup}) /usr/share/gosa/doc/guide.xml

%files schema
%defattr(-,root,root)
%doc COPYING AUTHORS README contrib/demo.ldif contrib/openldap
/etc/openldap/schema/gosa

%files mkntpasswd
%defattr(-,root,root)
/usr/bin/mkntpasswd

%files help-en
%defattr(-,root,root)
/usr/share/gosa/doc/guide/user/en

%files help-de
%defattr(-,root,root)
/usr/share/gosa/doc/guide/user/de

%files help-fr
%defattr(-,root,root)
/usr/share/gosa/doc/guide/user/fr

%files help-nl
%defattr(-,root,root)
/usr/share/gosa/doc/guide/user/nl

%changelog
* Mon May 7 2007 Lars Scheiter <lars.scheiter@GONICUS.de> 2.5.99cvs
- Changed packageversion to reflect CVS status of resulting build

* Wed Apr 11 2007 Lars Scheiter <lars.scheiter@GONICUS.de> 2.5.10
- New upstream
- Added new subpackage mkntpasswd
- Remove perl dependencies off of GOsa main package

* Tue Mar 6 2007 Lars Scheiter <lars.scheiter@GONICUS.de> 2.5.9
- New upstream
- fixed typo in updateprocess

* Mon Jan 15 2007 Lars Scheiter <lars.scheiter@GONICUS.de> 2.5.8
- New upstream release with security fixes

* Wed Dec 20 2006 Lars Scheiter <lars.scheiter@GONICUS.de> 2.5.7
- New upstream
- %pre and %postun always end successfully now, even on errors

* Fri Nov 17 2006 Lars Scheiter <lars.scheiter@GONICUS.de> 2.5.6
- New upstream
- Cleanup temporary dir after package removal
- Cleanup temporary dir before update

* Thu Sep 28 2006 Lars Scheiter <lars.scheiter@GONICUS.de> 2.5.4
- New upstream version
- Downgraded SuSE dependencies to php4

* Wed Jun 21 2006 Lars Scheiter <lars.scheiter@GONICUS.de> 2.5.1
- New upstream version

* Tue May 30 2006 Lars Scheiter <lars.scheiter@GONICUS.de> 2.5
- Updated RedHat dependencies
- New upstream version
- Spelling errors fixed ;)
- Seperation of online manual

* Mon Dec 19 2005 Lars Scheiter <lars.scheiter@GONICUS.de> 2.4-2
- Updated SuSE dependencies to php5

* Mon Nov 21 2005 Lars Scheiter <lars.scheiter@GONICUS.de> 2.4
- New upstream version
- Removed %doc for postgresql and openexchange

* Wed Jun 01 2005 Lars Scheiter <lars.scheiter@GONICUS.de> 2.4beta1
- New upstream version
- Added gosa.conf to contrib dir
- Rearranged documentation stuff
- Updated dependencies
- compress some files

* Mon Feb 21 2005 Lars Scheiter <lars.scheiter@GONICUS.de> 2.3
- Update version to 2.3 (upstream)

* Mon Dec 13 2004 Lars Scheiter <lars.scheiter@GONICUS.de> 2.2-2
- Optionally allow different sourcenames

* Mon Nov 22 2004 Lars Scheiter <lars.scheiter@GONICUS.de> 2.2
- Update to 2.2 (upstream)
- reintroduction of suse detection
- small fixes
- Corrected URL
- Synchronize schema package name with debian

* Mon May 19 2004 Levente Farkas <lfarkas@lfarkas.org> 2.1.1
- update to 2.1.1

* Mon Apr 19 2004 Levente Farkas <lfarkas@lfarkas.org> 2.1
- update to 2.1

* Fri Apr 16 2004 Levente Farkas <lfarkas@lfarkas.org> 2.1
- minor fixes
- update to 2.1rc2

* Tue Jan 24 2004 Henning P. Schmiedehausen <hps@intermeta.de> 2.1-2t
- bumped to 2.1beta2
- first INTERMETA internal build

* Mon Oct 20 2003 Lars Scheiter <lars.scheiter@GONICUS.de>
- Update to new upstream release (2.0rc1)

* Fri Oct 17 2003 Lars Scheiter <lars.scheiter@GONICUS.de>
- First build of GOsa as an RPM, should work on SuSE and RedHat
