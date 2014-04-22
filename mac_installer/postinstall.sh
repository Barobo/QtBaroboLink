#!/bin/bash

cd $2/QtBaroboLink.app/chbarobo/
sudo rm -rf /opt/local/ch/package/chbarobo
sudo rm -f /opt/local/ch/toolkit/include/mobot.h
sudo rm -f /opt/local/ch/toolkit/include/linkbot.h
sudo mkdir -p /opt/local/ch/package
sudo cp -R chbarobo /opt/local/ch/package/
sudo mkdir -p /opt/local/ch/toolkit/include
sudo cp /opt/local/ch/package/chbarobo/include/mobot.h /opt/local/ch/toolkit/include
sudo cp /opt/local/ch/package/chbarobo/include/linkbot.h /opt/local/ch/toolkit/include
exit 0
