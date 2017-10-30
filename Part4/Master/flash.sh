export LD_LIBRARY_PATH=/home/$USER/ti/ccsv6/ccs_base/DebugServer/drivers/
if [ -z $1 ]
  then
    mspdebug tilib -j "prog main.hex" --allow-fw-update
  else
    mspdebug tilib -j "prog main.hex" --allow-fw-update $1
fi
