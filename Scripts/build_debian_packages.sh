#!/bin/bash

bash Scripts/rebuild_all.sh hard

mkdir -p susi_package/DEBIAN
mkdir -p susi_package/usr/bin
mkdir -p susi_package/usr/share
mkdir -p susi_package/usr/lib
mkdir -p susi_package/etc/susi

cat > susi_package/DEBIAN/control <<EOF
Package: susi
Priority: optional
Section: devel
Maintainer: Tino Rusch <tino.rusch@webvariants.de>
Architecture: all
Version: 0.1.5
Description: The SUSI server
EOF

#copy core
cp Core/build/libsusi.so susi_package/usr/lib/
cp Core/build/susi susi_package/usr/bin/
cp Core/config.json susi_package/etc/susi/

#copy cpp engine lib
cp CPPEngine/build/libsusicpp.so susi_package/usr/lib/

#copy debugger
cp Debugger/build/susi-debugger susi_package/usr/bin/

#copy watchdog
cp Watchdog/build/watchdog susi_package/usr/bin/

#copy third party libs
cp /usr/local/lib/libPocoFoundation.so.23 susi_package/usr/lib/
cp /usr/local/lib/libPocoUtil.so.23 susi_package/usr/lib/
cp /usr/local/lib/libPocoJSON.so.23 susi_package/usr/lib/
cp /usr/local/lib/libPocoNet.so.23 susi_package/usr/lib/
cp /usr/local/lib/libPocoXML.so.23 susi_package/usr/lib/
cp /usr/local/lib64/libsoci_core.so.3.2 susi_package/usr/lib/

dpkg -b susi_package

rm -rvf susi_package

exit 0;
