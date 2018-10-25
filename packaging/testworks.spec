Name: testworks
Version: 18.10Beta
Release: 14809.CentOS62
Group: Development
Source: /tmp/testworks-%{version}.tar.gz
License: Commercial
Requires: rpm >= 4.0.2-8
Requires: /usr/bin/zenity
Requires: jre >= 1.7
Requires: pexpect
Obsoletes: testworks-stack-SbSctpIp
Obsoletes: testworks-install-manager
Obsoletes: testworks-lib-ccpu
Obsoletes: gnomemeeting
Obsoletes: openh323
Autoreq: 0

BuildRoot: %{_tmppath}/%{name}-%{version}-buildroot
Summary: Testworks Protocol Tester

%description
Testworks Protocol Tester is an application that allows testing 
signaling and bearer interfaces. It supports a wide range of 
protocols and interfaces that allows developers 
and testers to create an environment to test network equipments

This package includes the graphical interface and all hardware independent stacks.

%define __debug_install_post   \
   %{_rpmconfigdir}/find-debuginfo.sh %{?_find_debuginfo_opts} "%{_builddir}/%{?buildsubdir}"\
%{nil}


%prep 
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT
%setup -q

%build

%install
mkdir -p $RPM_BUILD_ROOT
cp -r --parents * $RPM_BUILD_ROOT
#tar xzv -C$RPM_BUILD_ROOT -f testworks-extra.tar.gz 

%clean
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

%post
chmod 777 /opt/testworks/bin/rclocal.sh
./opt/testworks/bin/rclocal.sh
rm -f /opt/testworks/bin/rclocal.sh

%files
%defattr(-,root,root)
/opt/testworks
/home/tester/.vnc/xstartup
/opt/testworks/bin/rclocal.sh
/usr/share/applications/testworks.desktop
/usr/share/applications/testworks-auto.desktop
/usr/share/applications/testworks-log-viewer.desktop
/opt/testworks/bin/SbSctpIp.bin 
/opt/testworks/protocolDescription/supportedProtocols.xml

%changelog
* Thu Nov 11 2006 Samuel Mello <samuel.mello.ext@siemens.com>
  First version
