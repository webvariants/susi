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

function add_to_start_script {
    BINARY=$1
    CONTAINER=$2
    ADDITIONAL=$3
    KEY=$(printf "/etc/susi/keys/%s_key.pem" $BINARY)
    CERT=$(printf "/etc/susi/keys/%s_cert.pem" $BINARY)
    echo "$BINARY -k $KEY -c $CERT $ADDITIONAL &" >> /var/lib/machines/$CONTAINER/bin/start_susi.sh
    echo "sleep 0.1" >> /var/lib/machines/$CONTAINER/bin/start_susi.sh
}

function finish_start_script {
    CONTAINER=$1
    echo "wait" >> /var/lib/machines/$CONTAINER/bin/start_susi.sh
}

function setup_core {
    CONTAINER=$1
    install_binary_to_container $SUSI_BINARY_PATH/susi-core $CONTAINER
    create_keys susi-core $CONTAINER
    add_to_start_script susi-core $CONTAINER
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
    echo "susi-cluster -c /etc/susi/cluster-config.json &" >> /var/lib/machines/$CONTAINER/bin/start_susi.sh
    echo "sleep 0.1" >> /var/lib/machines/$CONTAINER/bin/start_susi.sh
}

function setup_duktape {
    CONTAINER=$1
    install_binary_to_container $SUSI_BINARY_PATH/susi-duktape $CONTAINER
    create_keys susi-duktape $CONTAINER
    mkdir -p /var/lib/machines/$CONTAINER/usr/share/susi
    cp default_js_source.js /var/lib/machines/$CONTAINER/usr/share/susi/duktape_script.js
    read -p "Do you wish to edit the js source now? [y/N]" yn
    case $yn in
        [Yy]* ) nano /var/lib/machines/$CONTAINER/usr/share/susi/duktape_script.js ;;
        * ) ;;
    esac
    add_to_start_script susi-duktape $CONTAINER "-s /usr/share/susi/duktape_script.js"
}

function setup_heartbeat {
    CONTAINER=$1
    install_binary_to_container $SUSI_BINARY_PATH/susi-heartbeat $CONTAINER
    create_keys susi-heartbeat $CONTAINER
    add_to_start_script susi-heartbeat $CONTAINER
}

function setup_leveldb {
    CONTAINER=$1
    install_binary_to_container $SUSI_BINARY_PATH/susi-leveldb $CONTAINER
    create_keys susi-leveldb $CONTAINER
    read -p "Tell me where the db should be saved! [/usr/share/susi/susi.ldb]" LEVELDB_PATH
    if [ x"$LEVELDB_PATH" = x"" ]; then
        LEVELDB_PATH="/usr/share/susi/susi.ldb"
    fi
    add_to_start_script susi-leveldb $CONTAINER "--db $LEVELDB_PATH"
}

function setup_mqtt {
    CONTAINER=$1
    install_binary_to_container $SUSI_BINARY_PATH/susi-mqtt $CONTAINER
    create_keys susi-mqtt $CONTAINER
    read -p "Tell me the host address of your MQTT server! [localhost]" MQTT_ADDR
    read -p "Tell me the port of your MQTT server! [1883]" MQTT_PORT
    read -p "Tell me which topics should be forwarded to the MQTT server (comma seperated list)! [.*]" MQTT_FORWARD
    read -p "Tell me which topics should be subscribed from the MQTT server (comma seperated list)! [susi]" MQTT_SUBSCRIBE
    if [ x"$MQTT_ADDR" = x"" ]; then
        MQTT_ADDR="localhost"
    fi
    if [ x"$MQTT_PORT" = x"" ]; then
        MQTT_PORT="1883"
    fi
    if [ x"$MQTT_FORWARD" = x"" ]; then
        MQTT_FORWARD=".*"
    fi
    if [ x"$MQTT_SUBSCRIBE" = x"" ]; then
        MQTT_SUBSCRIBE="susi"
    fi
    case $yn in
        [Yy]* ) nano /var/lib/machines/$CONTAINER/etc/susi/cluster-config.json ;;
        * ) ;;
    esac
    add_to_start_script susi-mqtt $CONTAINER "--mqttAddr $MQTT_ADDR --mqttPort $MQTT_PORT -s '$MQTT_SUBSCRIBE' -f '$MQTT_FORWARD'"
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
    echo "susi-serial -c /etc/susi/serial-config.json &" >> /var/lib/machines/$CONTAINER/bin/start_susi.sh
    echo "sleep 0.1" >> /var/lib/machines/$CONTAINER/bin/start_susi.sh
}

function setup_udpserver {
    CONTAINER=$1
    install_binary_to_container $SUSI_BINARY_PATH/susi-udpserver $CONTAINER
    create_keys susi-udpserver $CONTAINER
    read -p "Tell me the port the UDP server should listen on! [4001]" UDP_PORT
    if [ x"$UDP_PORT" = x"" ]; then
        UDP_PORT="4001"
    fi
    add_to_start_script susi-udpserver $CONTAINER "-l $UDP_PORT"
}

function setup_webhooks {
    CONTAINER=$1
    install_binary_to_container $SUSI_BINARY_PATH/susi-webhooks $CONTAINER
    create_keys susi-webhooks $CONTAINER
    add_to_start_script susi-webhooks $CONTAINER
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
read -p "Tell me where your susi binaries are located! " SUSI_BINARY_PATH

setup_core $CONTAINER_NAME

ask_and_install susi-cluster
ask_and_install susi-duktape
ask_and_install susi-heartbeat
ask_and_install susi-leveldb
ask_and_install susi-mqtt
ask_and_install susi-serial
ask_and_install susi-udpserver
ask_and_install susi-webhooks

finish_start_script $CONTAINER_NAME

echo "your container is now ready to use! you can start it with susi-starter.sh $CONTAINER_NAME"

exit 0
