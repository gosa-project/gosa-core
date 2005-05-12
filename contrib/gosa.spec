# Some sort auf "detection" of suse
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
Version: 		2.3
Release:		1
License: 		GPL
Source: 		ftp://oss.GONICUS.de/pub/gosa/beta/%{sourcename}.tar.bz2
URL: 			http://oss.gonicus.de/project/?group_id=6
Group: 			System/Administration
Vendor:			GONICUS GmbH
Packager:		Lars Scheiter <lars.scheiter@GONICUS.de>
Buildarch: 		noarch
%if %{suse}
Requires:		apache2,php4,php4-gd,php4-ldap,php4-mcrypt,php4-imap,ImageMagick,perl-Crypt-SmbHash
%else
Requires: 		php,php-ldap,perl-Crypt-SmbHash >= 0.02,ImageMagick
%endif
BuildRoot: 		%{_tmppath}/%{name}-%{version}-root
BuildArch:		noarch

%define confdir 	/etc/%{name}

%if %{suse}
	%define apacheuser wwwrun
	%define apachegroup root
	%define webconf	/etc/apache2/conf.d/
%else
	%define apacheuser apache 
	%define apachegroup apache 
	%define webconf	/etc/httpd/conf.d/	
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
Requires:		openldap2 >= 2.0.27
%else
Requires:		openldap >= 2.0.27
%endif
Obsoletes:		gosa-ldap

%description schema
Contains the Schema definition files for the GOSA admin package.

%prep
%setup -q -n %{sourcename}
find . -depth -name CVS -type d | xargs rm -rf

%build

%install
# Create buildroot
mkdir -p %{buildroot}/usr/share/gosa

DIRS="doc ihtml plugins html include locale"

for i in $DIRS; do \
  cp -ua $i %{buildroot}/usr/share/gosa/$i ; \
done

mkdir -p %{buildroot}/var/spool/gosa
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
#rmdir contrib/openldap

%clean
rm -rf %{buildroot}

%files
%defattr(-,%{apacheuser},%{apachegroup})
%doc %attr(-,root,root) AUTHORS TODO README Changelog COPYING INSTALL FAQ
%doc %attr(-,root,root) contrib/gosa.conf contrib/scripts contrib/patches
%doc %attr(-,root,root) contrib/fix_config.sh contrib/vacation_example.txt

%config(noreplace) %attr(0640,%{apacheuser},%{apachegroup}) %{webconf}/gosa_include.conf
%dir %attr(0750, %{apacheuser}, %{apachegroup}) /var/spool/gosa
%attr(-, %{apacheuser}, %{apachegroup}) /usr/share/gosa

%files schema
%defattr(-,root,root)
%doc COPYING AUTHORS README contrib/iplanet contrib/demo.ldif contrib/openldap/slapd.conf
/etc/openldap/schema/gosa

%changelog
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
