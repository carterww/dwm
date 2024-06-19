#!/bin/bash
xhost +local:carter
Xephyr -screen 800x600 -reset -terminate -extension GLX :2 &
sleep 1
export DISPLAY=:2
./dwm
