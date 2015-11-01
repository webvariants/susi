#!/bin/bash

CONTAINER=$1

if [[ x"$CONTAINER" == x"" ]]; then
    read -p "Which container do you want to start? " CONTAINER
fi

# on debian 8 and above (we need systemd)
systemd-nspawn -D /var/lib/machines/$CONTAINER /bin/bash /bin/start_susi.sh

# on other systems
# chroot /var/lib/machines/$CONTAINER /bin/bash /bin/start_susi.sh

exit 0
