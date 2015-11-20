#!/bin/bash

SUSI_BINARY_PATH=/home/tino/Code/susi/bin
CONTAINER_NAME="susi-test"

function bootstrap_debian {
    debootstrap \
        --arch amd64 \
        --include dbus \
        jessie \
        /var/lib/machines/jessie \
        http://ftp.debian.de/debian
    if [ $? != 0 ]; then
        echo "can not bootstrap debian jessie, is your internet connection broken?"
        exit 1
    fi
}

function create_new_container {
    CONTAINER=$1
    if [ ! -d /var/lib/machines/jessie ]; then
        echo "no debian jessie found, try to bootstrap one."
        bootstrap_debian
    fi
    cp -rf /var/lib/machines/jessie /var/lib/machines/$CONTAINER
    echo $CONTAINER > /var/lib/machines/$CONTAINER/etc/hostname
}

function install_binary_to_container {
    BINARY=$1
    CONTAINER=$2
    echo "installing $BINARY to $CONTAINER..."
    cp $BINARY /var/lib/machines/$CONTAINER/bin/
    if [ $? != 0 ]; then
        echo "error while copy binary, did you already build susi or is your container image malformed? "
        exit 1
    fi
    for lib in $(ldd $BINARY | cut -d ' ' -f3); do
        cp $lib /var/lib/machines/$CONTAINER/lib/
        if [ $? != 0 ]; then
            echo "error while copy library, did you already build susi?"
            exit 1
        fi
    done
}

function create_keys {
    ID=$1
    CONTAINER=$2
    echo "generating keys for $ID..."
    KEY=$(printf "%s_key.pem" $ID)
    CERT=$(printf "%s_cert.pem" $ID)
    mkdir -p /var/lib/machines/$CONTAINER/etc/susi/keys
    openssl req -batch -nodes -x509 -newkey rsa:2048 -days 36500\
        -keyout /var/lib/machines/$CONTAINER/etc/susi/keys/$KEY \
        -out /var/lib/machines/$CONTAINER/etc/susi/keys/$CERT 2>/dev/null
}


function install_initd_script {
    NAME=$1
    DEPS=$2
    CONTAINER=$3

    CMD=$4

    template=$(cat unitfile.template)
    script=${template//__NAME__/$NAME}
    script=${script//__CMD__/$CMD}
    script=${script//__DEPS__/$DEPS}
    echo "$script" > /var/lib/machines/$CONTAINER/etc/systemd/system/$NAME.service
    ln -s /var/lib/machines/$CONTAINER/etc/systemd/system/$NAME.service /var/lib/machines/$CONTAINER/etc/systemd/system/multi-user.target.wants/$NAME.service
    #template=$(cat initscript.template)
    #script=${template/START_CMD/$CMD}
    #echo -n "$script" > /var/lib/machines/$CONTAINER/etc/init.d/$NAME
    #chmod +x /var/lib/machines/$CONTAINER/etc/init.d/$NAME
    #ln -s /var/lib/machines/$CONTAINER/etc/init.d/$NAME /var/lib/machines/$CONTAINER/etc/rc2.d/S03$NAME
}

function finish_start_script {
    CONTAINER=$1
    echo "bash" >> /var/lib/machines/$CONTAINER/bin/start_susi.sh
}

function setup_core {
    CONTAINER=$1
    install_binary_to_container $SUSI_BINARY_PATH/susi-core $CONTAINER
    create_keys susi-core $CONTAINER
    install_initd_script susi-core "" $CONTAINER "/bin/susi-core -k /etc/susi/keys/susi-core_key.pem -c /etc/susi/keys/susi-core_cert.pem"
}

function setup_shell {
    CONTAINER=$1
    install_binary_to_container $SUSI_BINARY_PATH/susi-shell $CONTAINER
    create_keys susi-shell $CONTAINER
    cp default_shell_config.json /var/lib/machines/$CONTAINER/etc/susi/shell-config.json
    read -p "Do you wish to edit the shell config now? [y/N]" yn
    case $yn in
        [Yy]* ) nano /var/lib/machines/$CONTAINER/etc/susi/shell-config.json ;;
        * ) ;;
    esac
    install_initd_script susi-shell "susi-core.service" $CONTAINER "/bin/susi-shell -c /etc/susi/shell-config.json"
}

function setup_duktape {
    CONTAINER=$1
    install_binary_to_container $SUSI_BINARY_PATH/susi-duktape $CONTAINER
    create_keys susi-duktape $CONTAINER
    mkdir -p /var/lib/machines/$CONTAINER/usr/share/susi
    cp default_duktape_config.json /var/lib/machines/$CONTAINER/etc/susi/duktape-config.json
    cp default_js_source.js /var/lib/machines/$CONTAINER/usr/share/susi/duktape-script.js
    read -p "Do you wish to edit the js source now? [y/N]" yn
    case $yn in
        [Yy]* ) nano /var/lib/machines/$CONTAINER/usr/share/susi/duktape-script.js ;;
        * ) ;;
    esac
    install_initd_script susi-duktape "susi-core.service" $CONTAINER "/bin/susi-duktape -c /etc/susi/duktape-config.json"
}

function setup_heartbeat {
    CONTAINER=$1
    install_binary_to_container $SUSI_BINARY_PATH/susi-heartbeat $CONTAINER
    create_keys susi-heartbeat $CONTAINER
    cp default_heartbeat_config.json /var/lib/machines/$CONTAINER/etc/susi/heartbeat-config.json
    install_initd_script susi-heartbeat "susi-core.service" $CONTAINER "/bin/susi-heartbeat -c /etc/susi/heartbeat-config.json"
}

function setup_leveldb {
    CONTAINER=$1
    install_binary_to_container $SUSI_BINARY_PATH/susi-leveldb $CONTAINER
    create_keys susi-leveldb $CONTAINER
    cp default_leveldb_config.json /var/lib/machines/$CONTAINER/etc/susi/leveldb-config.json
    install_initd_script susi-leveldb "susi-core.service" $CONTAINER "/bin/susi-leveldb -c /etc/susi/leveldb-config.json"
}

function setup_statefile {
    CONTAINER=$1
    install_binary_to_container $SUSI_BINARY_PATH/susi-statefile $CONTAINER
    create_keys susi-statefile $CONTAINER
    cp default_statefile_config.json /var/lib/machines/$CONTAINER/etc/susi/statefile-config.json
    install_initd_script susi-statefile "susi-core.service" $CONTAINER "/bin/susi-statefile -c /etc/susi/statefile-config.json"
}

function setup_mqtt {
    CONTAINER=$1
    install_binary_to_container $SUSI_BINARY_PATH/susi-mqtt $CONTAINER
    create_keys susi-mqtt $CONTAINER
    cp default_mqtt_config.json /var/lib/machines/$CONTAINER/etc/susi/mqtt-config.json
    install_initd_script susi-mqtt "susi-core.service" $CONTAINER "/bin/susi-mqtt -c /etc/susi/mqtt-config.json"
}

function setup_serial {
    CONTAINER=$1
    install_binary_to_container $SUSI_BINARY_PATH/susi-serial $CONTAINER
    create_keys susi-serial $CONTAINER
    cp default_serial_config.json /var/lib/machines/$CONTAINER/etc/susi/serial-config.json
    read -p "Do you wish to edit the serial config now? [y/N]" yn
    case $yn in
        [Yy]* ) nano /var/lib/machines/$CONTAINER/etc/susi/serial-config.json ;;
        * ) ;;
    esac
    install_initd_script susi-serial "susi-core.service" $CONTAINER "/bin/susi-serial -c /etc/susi/serial-config.json"
    echo "sleep 0.1" >> /var/lib/machines/$CONTAINER/bin/start_susi.sh
}

function setup_udpserver {
    CONTAINER=$1
    install_binary_to_container $SUSI_BINARY_PATH/susi-udpserver $CONTAINER
    create_keys susi-udpserver $CONTAINER
    cp default_udpserver_config.json /var/lib/machines/$CONTAINER/etc/susi/udpserver-config.json
    install_initd_script susi-udpserver "susi-core.service" $CONTAINER "/bin/susi-udpserver -c /etc/susi/udpserver-config.json"
}

function setup_webhooks {
    CONTAINER=$1
    install_binary_to_container $SUSI_BINARY_PATH/susi-webhooks $CONTAINER
    create_keys susi-webhooks $CONTAINER
    install_initd_script susi-webhooks "susi-core.service" $CONTAINER "/bin/susi-webhooks -k /etc/susi/keys/susi-webhooks_key.pem -c /etc/susi/keys/susi-webhooks_cert.pem"
}

function setup_authenticator {
    CONTAINER=$1
    install_binary_to_container $SUSI_BINARY_PATH/susi-authenticator $CONTAINER
    create_keys susi-authenticator $CONTAINER
    cp default_authenticator_config.json /var/lib/machines/$CONTAINER/etc/susi/authenticator-config.json
    read -p "Do you wish to edit the authenticator config now? [y/N]" yn
    case $yn in
        [Yy]* ) nano /var/lib/machines/$CONTAINER/etc/susi/authenticator-config.json ;;
        * ) ;;
    esac
    install_initd_script susi-authenticator "susi-core.service" $CONTAINER "/bin/susi-authenticator -c /etc/susi/authenticator-config.json"
}

function setup_cluster {
    CONTAINER=$1
    install_binary_to_container $SUSI_BINARY_PATH/susi-cluster $CONTAINER
    create_keys susi-cluster $CONTAINER
    cp default_cluster_config.json /var/lib/machines/$CONTAINER/etc/susi/cluster-config.json
    read -p "Do you wish to edit the cluster config now? [y/N]" yn
    case $yn in
        [Yy]* ) nano /var/lib/machines/$CONTAINER/etc/susi/cluster-config.json ;;
        * ) ;;
    esac
    install_initd_script susi-cluster "susi-core.service" $CONTAINER "/bin/susi-cluster -c /etc/susi/cluster-config.json"
}

function ask_and_install {
    BINARY=$1
    read -p "Do you wish to install $BINARY? [Y/n]" yn
    cmd=$(printf "setup_%s" $(echo $BINARY|cut -d'-' -f2))
    case $yn in
        [Nn]* ) ;;
        * ) $cmd $CONTAINER_NAME;;
    esac
}

read -p "Tell me the name of your project! " CONTAINER_NAME

echo "deleting old project $CONTAINER_NAME..."
rm -rf /var/lib/machines/$CONTAINER_NAME

echo "create new container from debian jessie..."
create_new_container $CONTAINER_NAME

read -p "Tell me where your susi binaries are located! [/home/tino/Code/susi/susi/bin]" SUSI_BINARY_PATH
if [ x"$SUSI_BINARY_PATH" = x"" ];then
    SUSI_BINARY_PATH=/home/tino/Code/susi/susi/bin
fi

setup_core $CONTAINER_NAME

ask_and_install susi-authenticator
ask_and_install susi-cluster
ask_and_install susi-duktape
ask_and_install susi-heartbeat
ask_and_install susi-leveldb
ask_and_install susi-mqtt
ask_and_install susi-serial
ask_and_install susi-udpserver
ask_and_install susi-webhooks
ask_and_install susi-statefile
ask_and_install susi-shell

echo "your container is now ready to use! you can start it with susi-starter.sh $CONTAINER_NAME"

exit 0
