#!/bin/bash

SUSI_BINARY_PATH=$(git config susi-dev.binarypath)
if [ $? != 0 ]; then
    read -p "tell me where your susi binaries are!" SUSI_BINARY_PATH
    git config susi-dev.binarypath $SUSI_BINARY_PATH
fi

SUSI_PROJECT_FILE=$1

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
    rm -rf /var/lib/machines/$CONTAINER
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
}

function setup_core {
    CONTAINER=$1
    install_binary_to_container $SUSI_BINARY_PATH/susi-core $CONTAINER
    create_keys susi-core $CONTAINER
    install_initd_script susi-core "" $CONTAINER "/bin/susi-core -k /etc/susi/keys/susi-core_key.pem -c /etc/susi/keys/susi-core_cert.pem"
}

function setup_component {
    CONTAINER=$1
    COMPONENT=$2
    install_binary_to_container $SUSI_BINARY_PATH/$COMPONENT $CONTAINER
    create_keys $COMPONENT $CONTAINER
    defaultConfig=$(printf "default_%s_config.json" $(echo $COMPONENT|cut -d- -f2))
    targetConfig=$(echo $defaultConfig|cut -d_ -f2,3,4)
    cp $defaultConfig /var/lib/machines/$CONTAINER/etc/susi/$targetConfig
    install_initd_script $COMPONENT "susi-core.service" $CONTAINER "/bin/$COMPONENT -c /etc/susi/$targetConfig"
}

function init {
    SUSI_PROJECT_FILE=$1
    if [ x"$SUSI_PROJECT_FILE" = x"" ]; then
        echo usage: "$0 init <project-file.json>"
        exit 1
    fi
    cat $SUSI_PROJECT_FILE | jq -c ".nodes[]" | while read line; do
        CONTAINER=$(echo $line | jq -c .id|cut -d\" -f2)
        create_new_container $CONTAINER
        setup_core $CONTAINER
        echo $line|jq -c .components|tr -d '[]"'| tr ',' '\n'|while read COMPONENT; do
            echo setup component $COMPONENT ...
            setup_component $CONTAINER $COMPONENT
        done
    done
}

function start {
    CONTAINER=$1
    if [ x"$CONTAINER" = x"" ]; then
        echo usage: "$0 start <node>"
        exit 1
    fi
    xterm -e "systemd-nspawn -b -D /var/lib/machines/$CONTAINER" &
}

case $1 in
    init) init $2;;
    start) start $2;;
    *) echo usage: "$0 <init|start|stop> <other arguments>"
esac
