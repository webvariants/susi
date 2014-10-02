#!/bin/bash

FORCE_REBUILD="false"

function getPoco {
	echo installing poco libraries...
	if test -s /usr/local/lib/libPocoJSON.so.23 && test $FORCE_REBUILD = "false"; then
		echo allready installed.
		return
	fi
	wget -O /tmp/poco-1.5.3-all.tar.gz http://pocoproject.org/releases/poco-1.5.3/poco-1.5.3-all.tar.gz || return 1
	pushd /tmp
	rm -rf poco-1.5.3-all
	tar xfvz poco-1.5.3-all.tar.gz
	pushd poco-1.5.3-all
	./configure || return 1
	make -j4 || return 1
	make install || return 1
	popd
	popd
	echo finished installing poco libraries!
}

function getSoci {
	echo installing soci library...
	if test -s /usr/local/lib64/libsoci_core.so.3.2.2 && test $FORCE_REBUILD = "false"; then
		echo allready installed.
		return
	fi
	wget -O /tmp/soci-3.2.2.tar.gz http://sourceforge.net/projects/soci/files/soci/soci-3.2.2/soci-3.2.2.tar.gz/download || return 1
	pushd /tmp
	rm -rf soci-3.2.2
	tar xfvz soci-3.2.2.tar.gz
	pushd soci-3.2.2
	mkdir build
	pushd build
	cmake .. || return 1
	make -j4 || return 1
	make install || return 1
	popd
	popd
	popd
	echo finished installing soci library!
}

function getSqlite {
	echo installing sqlite3 library...
	if test -s /usr/include/sqlite3.h && test $FORCE_REBUILD = "false"; then
		echo allready installed.
		return
	fi
	wget -O /tmp/sqlite-autoconf-3080600.tar.gz http://www.sqlite.org/2014/sqlite-autoconf-3080600.tar.gz || return 1
	pushd /tmp
	rm -rf sqlite-autoconf-3080600
	tar xfvz sqlite-autoconf-3080600.tar.gz
	pushd sqlite-autoconf-3080600
	./configure || return 1
	make -j4 || return 1
	make install || return 1
	popd
	popd
	echo finished installing sqlite3 library!	
}

function getFirebird {
	echo installing firebird library...
	if test -s /usr/include/ibase.h && test $FORCE_REBUILD = "false"; then
		echo allready installed.
		return
	fi
	wget -O /tmp/FirebirdCS-2.5.3.26778-0.amd64.tar.gz http://sourceforge.net/projects/firebird/files/firebird-linux-amd64/2.5.3-Release/FirebirdCS-2.5.3.26778-0.amd64.tar.gz/download || return 1
	pushd /tmp
	rm -rf FirebirdCS-2.5.3.26778-0.amd64
	rm -rf /opt/firebird
	tar xfvz FirebirdCS-2.5.3.26778-0.amd64.tar.gz
	pushd FirebirdCS-2.5.3.26778-0.amd64
	echo -e "\npassword\n" | ./install.sh || return 1
	popd
	popd
	echo finished installing firebird library!	
}


getFirebird || exit 1
getSqlite || exit 1
getSoci || exit 1
getPoco || exit 1

if test $FORCE_REBUILD = "true"; then
	bash rebuild_all.sh hard
else
	bash rebuild_all.sh soft
fi

mkdir -p bin
ln -sf ../Core/build/susi bin/susi
ln -sf ../Core/config.json bin/config.json
ln -sf ../Core/build/susi_test bin/susi_test
ln -sf ../CPPEngine/build/susi-cppengine bin/susi-cppengine
ln -sf ../Debugger/build/susi-debugger bin/susi-debugger
ln -sf ../Watchdog/build/watchdog bin/watchdog

exit 0;

