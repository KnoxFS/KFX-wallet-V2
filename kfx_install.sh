#!/bin/bash
# setup version: 0.0.3

TMP_FOLDER=$(mktemp -d)
CONFIG_FILE='kfx.conf'
CONFIGFOLDER='/root/.kfx'
COIN_DAEMON='kfxd'
COIN_CLI='kfx-cli'
COIN_PATH='/usr/local/bin/'
COIN_REPO='https://github.com/knoxfs/kfx-core.git'
COIN_TGZ=https://github.com/knoxfs/kfx-core/releases/download/v1.0.0/kfx-1.0.0-x86_64-linux-gnu.zip'
COIN_ZIP=$(echo $COIN_TGZ | awk -F'/' '{print $NF}')
COIN_NAME='KFX'
COIN_PORT=29929
RPC_PORT=29939

NODEIP=$(curl -s4 icanhazip.com)
BINDIP=$NODEIP

BLUE="\033[0;34m"
YELLOW="\033[0;33m"
CYAN="\033[0;36m"
PURPLE="\033[0;35m"
RED='\033[0;31m'
GREEN="\033[0;32m"
NC='\033[0m'
MAG='\e[1;35m'

# this pulls the zip file down and moves it into COIN_PATH
function download_node() {
  echo -e "${GREEN}Downloading and Installing VPS $COIN_NAME Daemon${NC}"
  cd $TMP_FOLDER >/dev/null 2>&1
  wget -q $COIN_TGZ

  # this WAS looking for a compile error - but there's no compiling going on?
  if [ "$?" -gt "0" ]; then
    echo -e "${RED}Failed to retrieve $COIN_NAME. Please investigate.${NC}"
    exit 1
  fi

  unzip $COIN_ZIP >/dev/null 2>&1
  chmod +x $COIN_DAEMON $COIN_CLI
  cp $COIN_DAEMON $COIN_CLI $COIN_PATH
  cd ~ >/dev/null 2>&1
  rm -rf $TMP_FOLDER >/dev/null 2>&1
#  clear
}

function configure_systemd() {
  echo -e "${YELLOW}Setting up systemd...${NC}."
  cat << EOF > /etc/systemd/system/$COIN_NAME.service
[Unit]
Description=$COIN_NAME service
After=network.target

[Service]
User=root
Group=root

Type=forking
#PIDFile=$CONFIGFOLDER/$COIN_NAME.pid

ExecStart=$COIN_PATH$COIN_DAEMON -daemon -conf=$CONFIGFOLDER/$CONFIG_FILE -datadir=$CONFIGFOLDER
ExecStop=$COIN_PATH$COIN_CLI -conf=$CONFIGFOLDER/$CONFIG_FILE -datadir=$CONFIGFOLDER stop

Restart=always
PrivateTmp=true
TimeoutStopSec=60s
TimeoutStartSec=10s
StartLimitInterval=120s
StartLimitBurst=5

[Install]
WantedBy=multi-user.target
EOF

  echo -e "${YELLOW}Reloading systemd...${NC}."
  systemctl daemon-reload
  sleep 3

  echo -e "${YELLOW}Starting up $COIN_NAME service...${NC}."
  systemctl start $COIN_NAME.service
  systemctl enable $COIN_NAME.service >/dev/null 2>&1
  sleep 3

  if [[ -z "$(ps axo cmd:100 | egrep $COIN_DAEMON)" ]]; then
    echo -e "${RED}$COIN_NAME is not running${NC}, please investigate. You should start by running the following commands as root:"
    echo -e "${GREEN}systemctl start $COIN_NAME.service"
    echo -e "systemctl status $COIN_NAME.service"
    echo -e "less /var/log/syslog${NC}"
    exit 1
  else
    echo -e "${GREEN}$COIN_NAME service started${NC}."
  fi
}


function create_config() {
  mkdir $CONFIGFOLDER >/dev/null 2>&1
  RPCUSER=$(tr -cd '[:alnum:]' < /dev/urandom | fold -w10 | head -n1)
  RPCPASSWORD=$(tr -cd '[:alnum:]' < /dev/urandom | fold -w22 | head -n1)
  cat << EOF > $CONFIGFOLDER/$CONFIG_FILE
rpcuser=$RPCUSER
rpcpassword=$RPCPASSWORD
rpcport=$RPC_PORT
rpcallowip=127.0.0.1
listen=1
server=1
daemon=1
port=$COIN_PORT
EOF
}

function create_key() {
  echo -e "${YELLOW}Enter your ${RED}$COIN_NAME Masternode GEN Key${NC}."
  read -e COINKEY
  if [[ -z "$COINKEY" ]]; then
    echo -e "${YELLOW}Generating key: starting daemon...${NC}"
    $COIN_PATH$COIN_DAEMON -daemon
    sleep 10
    if [ -z "$(ps axo cmd:100 | grep $COIN_DAEMON)" ]; then
     echo -e "${RED}$COIN_NAME server couldn not start. Check /var/log/syslog for errors.{$NC}"
     exit 1
    fi
    COINKEY=$($COIN_PATH$COIN_CLI masternode genkey)
    if [ "$?" -gt "0" ]; then
      echo -e "${RED}Wallet not fully loaded. Let us wait and try again to generate the GEN Key${NC}"
      sleep 20
      COINKEY=$($COIN_PATH$COIN_CLI masternode genkey)
    fi

    $COIN_PATH$COIN_CLI stop
    sleep 5
  fi
#  clear
}

function update_config() {
  sed -i 's/daemon=1/daemon=1/' $CONFIGFOLDER/$CONFIG_FILE
  cat << EOF >> $CONFIGFOLDER/$CONFIG_FILE
logintimestamps=1
maxconnections=256
bind=$BINDIP
masternode=1
externalip=$NODEIP:$COIN_PORT
masternodeprivkey=$COINKEY

#ADDNODES
addnode=185.147.75.100:29929
addnode=185.147.75.101:29929
addnode=185.147.75.102:29929
addnode=185.147.75.103:29929
addnode=185.147.75.104:29929
EOF
}

function get_ip() {
  declare -a NODE_IPS
  for ips in $(netstat -i | awk '!/Kernel|Iface|lo/ {print $1," "}')
  do
    NODE_IPS+=($(curl --interface $ips --connect-timeout 2 -s4 icanhazip.com))
  done

  if [ ${#NODE_IPS[@]} -gt 1 ]
    then
      echo -e "${GREEN}More than one IP. Please type 0 to use the first IP, 1 for the second and so on...${NC}"
      INDEX=0
      for ip in "${NODE_IPS[@]}"
      do
        echo ${INDEX} $ip
        let INDEX=${INDEX}+1
      done
      read -e choose_ip
      NODEIP=${NODE_IPS[$choose_ip]}
  else
    NODEIP=${NODE_IPS[0]}
  fi

  # NODEIP is now the public ip address, if the server is NAT'd, we'll need to bind to a local ip
  # (at least give the option)
  echo -e "${YELLOW} Bind ip address (enter if host is behind NAT firewall): ${NC}"
  read -e BINDIP
  if [[ -z "$BINDIP" ]]; then
    BINDIP=$NODEIP
  fi

}

function checks() {
  local LSB_RELEASE=$(lsb_release -d)
  if [[ $LSB_RELEASE != *Ubuntu* ]]; then
    echo -e "${RED} You are not running Ubuntu, Installation is cancelled.${NC}"
    exit 1
  fi
  if [[ $LSB_RELEASE != *16.04* && $LSB_RELEASE != *18.04* ]]; then
    echo -e "${RED}You are not running Ubuntu 18.04 or 16.04. Installation is cancelled.${NC}"
    exit 1
  fi

  if [[ $EUID -ne 0 ]]; then
    echo -e "${RED}$0 must be run as root.${NC}"
    exit 1
  fi

  # removed this check - if the daemon exists, just consider it an upgrade
#if [ -n "$(pidof $COIN_DAEMON)" ] || [ -e "$COIN_DAEMON" ] ; then
#  echo -e "${RED}$COIN_NAME is already installed.${NC}"
#  exit 1
#fi
}

function prepare_system() {
  # this installs the libraries that the binaries need to run
  echo -e "Preparing the VPS to setup. ${CYAN}$COIN_NAME${NC} ${RED}Masternode${NC}"
  apt-get update >/dev/null 2>&1
  DEBIAN_FRONTEND=noninteractive apt-get update > /dev/null 2>&1
  DEBIAN_FRONTEND=noninteractive apt-get -o Dpkg::Options::="--force-confdef" -o Dpkg::Options::="--force-confold" -y -qq upgrade >/dev/null 2>&1
  echo -e "Installing required packages, it may take some time to finish.${NC}"
  apt-get update >/dev/null 2>&1
  apt-get install -y -o Dpkg::Options::="--force-confdef" -o Dpkg::Options::="--force-confold" make software-properties-common \
  build-essential libtool autoconf libssl-dev libboost-dev automake git wget curl libdb-dev bsdmainutils libdb++-dev \
  libminiupnpc-dev libgmp3-dev pkg-config libevent-dev unzip >/dev/null 2>&1
  if [ "$?" -gt "0" ];
    then
      echo -e "${RED}Not all required packages were installed properly. Try to install them manually by running the following commands:${NC}\n"
      echo "apt-get update"
      echo "apt -y install software-properties-common"
      echo "apt-add-repository -y ppa:bitcoin/bitcoin"
      echo "apt-get update"
      echo "apt install -y make build-essential libtool software-properties-common autoconf libssl-dev libboost-dev libboost-chrono-dev libboost-filesystem-dev \
        libboost-program-options-dev libboost-system-dev libboost-test-dev libboost-thread-dev sudo automake git curl libdb4.8-dev \
        bsdmainutils libdb4.8++-dev libminiupnpc-dev libgmp3-dev ufw pkg-config libevent-dev libdb5.3++ unzip libzmq5"
     exit 1
  fi
#  clear
}

function important_information() {
  echo
  echo -e "$COIN_NAME Masternode is up and running listening on port ${GREEN}$COIN_PORT${NC}."
  echo -e "Configuration file is: ${RED}$CONFIGFOLDER/$CONFIG_FILE${NC}"
  echo -e "Start: ${RED}systemctl start $COIN_NAME.service${NC}"
  echo -e "Stop: ${RED}systemctl stop $COIN_NAME.service${NC}"
  echo -e "VPS_IP:PORT ${GREEN}$NODEIP:$COIN_PORT${NC}"
  echo -e "MASTERNODE GENKEY is: ${RED}$COINKEY${NC}"
  echo -e "Please check ${RED}$COIN_NAME${NC} is running with the following command: ${RED}systemctl status $COIN_NAME.service${NC}"
  echo -e "Use ${RED}$COIN_CLI masternode status${NC} to check your MN."
  if [[ -n $SENTINEL_REPO  ]]; then
    echo -e "${RED}Sentinel${NC} is installed in ${RED}/root/sentinel_$COIN_NAME${NC}"
    echo -e "Sentinel logs is: ${RED}$CONFIGFOLDER/sentinel.log${NC}"
  fi
}

function create_swap() {
  echo -e "${YELLOW} Checking if swap is needed...${NC}"
  PHYMEM=$(free -g|awk '/^Mem:/{print $2}')
  SWAP=$(swapon -s)
  if [[ "$PHYMEM" -lt "2"  &&  -z "$SWAP" ]]
   then
     echo -e "${GREEN}Server is running with less than 2G of RAM without SWAP, creating 2G swap file.${NC}"
     SWAPFILE=$(mktemp)
     dd if=/dev/zero of=$SWAPFILE bs=1024 count=2M
     chmod 600 $SWAPFILE
     mkswap $SWAPFILE
     swapon -a $SWAPFILE
  else
    echo -e "${GREEN}The server running with at least 2G of RAM, or a SWAP file is already in place.${NC}"
  fi
#  clear
}

function isUpgrade() {
  if [ -e $COIN_PATH/$COIN_DAEMON ]
  then
    # its already here, make sure its not running
    echo -e "${YELLOW}$COIN_PATH$COIN_DAEMON already installed, doing an upgrade...${NC}"

    if pgrep -x $COIN_DAEMON >/dev/null
    then
      # stop the daemon from running
      echo -e "${YELLOW}Shutting down daemon...${NC}"
      systemctl stop $COIN_NAME.service
      sleep 10
    fi
    return 1
  else
    return 0
  fi
}

function upgrade_node() {
  # make sure daemon is stopped (it should already be, but this wont hurt)
  echo -e "${YELLOW} Making sure daemon is stopped...${NC}"
  systemctl stop $COIN_NAME.service
  sleep 5

  # make copy of old binaries
  echo -e "${YELLOW} Backing up existing binaries...${NC}"
  cp $COIN_PATH/$COIN_CLI $COIN_PATH/$COIN_CLI~
  cp $COIN_PATH/$COIN_DAEMON $COIN_PATH/$COIN_DAEMON~

  # copy in new binaries
  echo -e "${YELLOW} Pulling new binaries...${NC}"
  download_node

  # start daemon
  echo -e "${YELLOW} Restarting daemon...${NC}"
  systemctl start $COIN_NAME.service
  sleep 5
}

function setup_node() {
  get_ip
  create_config
  create_key
  update_config
#  enable_firewall
  configure_systemd
  important_information
}


##### Main #####
clear

checks
isUpgrade
if [ $? -eq 1 ]
then
  # this is an upgrade and deamon should not be running (isUpgrade stops it)
  upgrade_node
  echo -e "${GREEN}Upgrade complete! Please check masternode status/version to verify ${NC}"

else
  prepare_system
  create_swap
  download_node
  setup_node
fi
