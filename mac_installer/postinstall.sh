#!/bin/bash

cd $2/QtBaroboLink.app/chbarobo/
sudo rm -rf /usr/local/ch/package/chbarobo
sudo rm /usr/local/ch/toolkit/include/mobot.h
sudo rm /usr/local/ch/toolkit/include/linkbot.h
sudo cp -R chbarobo /usr/local/ch/package/
sudo cp /usr/local/ch/package/chbarobo/include/mobot.h /usr/local/ch/toolkit/include
sudo cp /usr/local/ch/package/chbarobo/include/linkbot.h /usr/local/ch/toolkit/include
exit 0
