#!/bin/bash

SUSI_DEV_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$(pwd)"

SUSI_BINARY_PATH=$(git config susi-dev.binarypath)
if [ $? != 0 ]; then
    read -p "tell me where your susi binaries are!" SUSI_BINARY_PATH
    git config susi-dev.binarypath $SUSI_BINARY_PATH
fi

SUSI_PROJECT_FILE="susi-project-config.json"
function check_for_config {
    if [ ! -f $SUSI_PROJECT_FILE ]; then
        echo "create a susi-project-config.json (with susi-dev.sh init)"
        exit 1
    fi
}

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

function create_localfs {
    CONTAINER=$1
    mkdir -p $PROJECT_ROOT/nodes/$CONTAINER
    mkdir -p $PROJECT_ROOT/nodes/$CONTAINER/bin
    mkdir -p $PROJECT_ROOT/nodes/$CONTAINER/lib
    mkdir -p $PROJECT_ROOT/nodes/$CONTAINER/etc/susi
    mkdir -p $PROJECT_ROOT/nodes/$CONTAINER/etc/systemd/system/multi-user.target.wants
    mkdir -p $PROJECT_ROOT/nodes/$CONTAINER/usr/share/susi
    echo $CONTAINER > $PROJECT_ROOT/nodes/$CONTAINER/etc/hostname
    echo "created local filesystem for $CONTAINER"
}

function mount_container {
    CONTAINER=$1
    if [ ! -d /var/lib/machines/jessie ]; then
        echo "no debian jessie found, try to bootstrap one."
        bootstrap_debian
    fi

    work=/tmp/susi-dev/container_workdirs/$CONTAINER
    low=/var/lib/machines/jessie
    upp=$PROJECT_ROOT/nodes/$CONTAINER
    mountpoint=container/$CONTAINER

    mkdir -p $mountpoint
    mkdir -p $upp

    sudo mkdir -p $work
    sudo umount $mountpoint
    sudo mount -t overlay -o lowerdir="$low",upperdir="$upp",workdir="$work" overlay "$mountpoint"

    echo "mounted container $CONTAINER"

}

function install_binary_to_container {
    BINARY=$1
    CONTAINER=$2
    echo "installing $BINARY to $CONTAINER..."
    cp $BINARY $PROJECT_ROOT/nodes/$CONTAINER/bin/
    if [ $? != 0 ]; then
        echo "error while copy binary, did you already build susi or is your container image malformed? "
        exit 1
    fi
    for lib in $(ldd $BINARY | cut -d ' ' -f3); do
        cp $lib $PROJECT_ROOT/nodes/$CONTAINER/lib/
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
    mkdir -p $PROJECT_ROOT/nodes/$CONTAINER/etc/susi/keys
    openssl req -batch -nodes -x509 -newkey rsa:2048 -days 36500\
        -keyout $PROJECT_ROOT/nodes/$CONTAINER/etc/susi/keys/$KEY \
        -out $PROJECT_ROOT/nodes/$CONTAINER/etc/susi/keys/$CERT 2>/dev/null
}

function install_initd_script {
    NAME=$1
    DEPS=$2
    CONTAINER=$3
    CMD=$4
    template=$(cat $SUSI_DEV_ROOT/unitfile.template)
    script=${template//__NAME__/$NAME}
    script=${script//__CMD__/$CMD}
    script=${script//__DEPS__/$DEPS}
    echo "$script" > $PROJECT_ROOT/nodes/$CONTAINER/etc/systemd/system/$NAME.service
    ln -s $PROJECT_ROOT/nodes/$CONTAINER/etc/systemd/system/$NAME.service $PROJECT_ROOT/nodes/$CONTAINER/etc/systemd/system/multi-user.target.wants/$NAME.service 2>/dev/null
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
    cp $SUSI_DEV_ROOT/config_files/$defaultConfig $PROJECT_ROOT/nodes/$CONTAINER/etc/susi/$targetConfig
    install_initd_script $COMPONENT "susi-core.service" $CONTAINER "/bin/$COMPONENT -c /etc/susi/$targetConfig"
}

function install_container_to_local_systemd {
    echo "install container to local systemd"
    CONTAINER=$1
    NAME=$CONTAINER
    CMD="/usr/bin/systemd-nspawn -b -D $PROJECT_ROOT/container/susi-test-1"
    template=$(cat $SUSI_DEV_ROOT/containerunitfile.template)
    script=${template//__NAME__/$NAME}
    script=${script//__CMD__/$CMD}
    cmd="echo '$script' > /etc/systemd/system/$CONTAINER.service"
    sudo su root -c "$cmd"
}

function setup {
    check_for_config
    cat $SUSI_PROJECT_FILE | jq -c ".nodes[]" | while read line; do
        CONTAINER=$(echo $line | jq -c .id|cut -d\" -f2)
        create_localfs $CONTAINER
        mount_container $CONTAINER
        install_container_to_local_systemd $CONTAINER
        setup_core $CONTAINER
        echo $line|jq -c .components|tr -d '[]"'| tr ',' '\n'|while read COMPONENT; do
            echo setup component $COMPONENT ...
            setup_component $CONTAINER $COMPONENT
        done
    done
}



function start {
    check_for_config
    cat $SUSI_PROJECT_FILE | jq -c ".nodes[]" | while read line; do
        CONTAINER=$(echo $line | jq -c .id|cut -d\" -f2)
        sudo systemctl start $CONTAINER
    done
}

function stop {
    check_for_config
    cat $SUSI_PROJECT_FILE | jq -c ".nodes[]" | while read line; do
        CONTAINER=$(echo $line | jq -c .id|cut -d\" -f2)
        sudo systemctl stop $CONTAINER
    done
}

function init {
    cp $SUSI_DEV_ROOT/susi-project-config.json .
}

function login {
    CONTAINER=$1
    sudo machinectl login $CONTAINER
}

function status {
    check_for_config
    cat $SUSI_PROJECT_FILE | jq -c ".nodes[]" | while read line; do
        CONTAINER=$(echo $line | jq -c .id|cut -d\" -f2)
	echo "------------------------------------------"
        sudo systemctl -n0 status $CONTAINER
	echo ""
	sudo systemctl -n0 -M $CONTAINER status 'susi-*'
    done

}

case $1 in
    setup) stop; setup ;;
    start) start ;;
    stop) stop ;;
    init) init ;;
    login) login $2 ;;
    status) status ;;
    *) echo usage: "$0 <init|setup|start|stop|login|status>"
esac

exit 0
