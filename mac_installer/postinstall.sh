#!/bin/bash

cd $2/QtBaroboLink.app/chbarobo/
sudo rm -rf /opt/ch/package/chbarobo
sudo rm -f /opt/ch/toolkit/include/mobot.h
sudo rm -f /opt/ch/toolkit/include/linkbot.h
sudo mkdir -p /opt/ch/package
sudo cp -R chbarobo /opt/ch/package/
sudo mkdir -p /opt/ch/toolkit/include
sudo cp /opt/ch/package/chbarobo/include/mobot.h /opt/ch/toolkit/include
sudo cp /opt/ch/package/chbarobo/include/linkbot.h /opt/ch/toolkit/include
exit 0
