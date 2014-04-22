#!/bin/bash

cd $2/BaroboLink.app
sudo rm -rf /opt/ch/package/chbarobo
sudo rm -f /opt/ch/toolkit/include/mobot.h
sudo rm -f /opt/ch/toolkit/include/linkbot.h
sudo mkdir -p /opt/ch/package
sudo cp -R chbarobo /opt/ch/package/
sudo mkdir -p /opt/ch/toolkit/include
sudo cp /opt/ch/package/chbarobo/include/mobot.h /opt/ch/toolkit/include
sudo cp /opt/ch/package/chbarobo/include/linkbot.h /opt/ch/toolkit/include
touch /tmp/chbarobo_installed_2.0.7.txt
exit 0
