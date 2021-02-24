#!/bin/sh
export templdpath=$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=./linux64:$LD_LIBRARY_PATH
export SteamAppId=892970

NAME="Valheim"
PORT=2456
WORLD="Valheim"
PASSWORD=""

# Parse arguments
TEMP=$(getopt -o n:w:p:P: \
--long name:,world:,password:,port: \
-- "$@")                                                                

eval set -- "$TEMP"
while true; do     
  case $1 in 
    -n|--name)
      NAME="$2"; shift; shift; continue
    ;;
    -w|--world)
      WORLD="$2"; shift; shift; continue
    ;;
    -p|--port)
      PORT=$2; shift; shift; continue;
    ;;
    -P|--password)
      PASSWORD="$2"; shift; shift; continue;
    ;;
    --)
      break
    ;;
    *)
      printf "Unknown option %s\n" "$1"
      exit 1
    ;;
  esac                                                                        
done

/steamcmd/steamcmd.sh +login anonymous +force_install_dir /valheim +app_update 896660 validate +quit

echo "Starting server PRESS CTRL-C to exit"

# Tip: Make a local copy of this script to avoid it being overwritten by steam.
# NOTE: Minimum password length is 5 characters & Password cant be in the server name.
# NOTE: You need to make sure the ports 2456-2458 is being forwarded to your server through your local router & firewall.
if [$PASSWORD == ""] 
then
  ./valheim_server.x86_64 -name $NAME -port $PORT -world $WORLD -savedir /valheim/save
else
  ./valheim_server.x86_64 -name $NAME -port $PORT -world $WORLD -password $PASSWORD -savedir /valheim/save
fi

export LD_LIBRARY_PATH=$templdpath
