/*
   Copyright 2013 Barobo, Inc.

   This file is part of BaroboLink.

   BaroboLink is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   BaroboLink is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with BaroboLink.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _ROBOT_MANAGER_H_
#define _ROBOT_MANAGER_H_

#include <string.h>
#include <string>

#include "configfile.h"
#include <mobot.h>
#include "recordmobot.h"
#include "QMobot.h"

#define MAX_CONNECTED 100
using namespace std;

class CRobotManager : public ConfigFile
{
  public:
    CRobotManager();
    ~CRobotManager();
    virtual int addEntry(const char* entry);
    virtual int addEntry(string entry);
    virtual void moveMobot(int destIndex, int srcIndex);
    virtual int moveEntryUp(int index);
    virtual int moveEntryDown(int index);
    virtual int insertEntry(const char* entry, int index);
    virtual bool isConnected(int index) const;
    virtual bool isPlaying();
    virtual int connectIndex(int index);
    virtual int disconnect(int index);
    virtual int disconnectAll();
    virtual RecordMobot* getUnboundMobot();
    virtual int numConnected();
    virtual int numAvailable();
    virtual void record();
    virtual int remove(int index);
    virtual void restoreSavedMobot(int index);
    virtual void addDelay(double seconds);
    virtual void play();
    virtual QMobot* getMobot(int connectIndex);
    virtual QMobot* getMobotIndex(int index);
    virtual string* generateChProgram(bool looped = false, bool holdOnExit = false);
    virtual string* generateCppProgram(bool looped = false, bool holdOnExit = false);
    virtual string* generatePythonProgram(bool looped = false, bool holdOnExit = false);
    bool _isPlaying;
    int _newDndIndex;
  protected:
    QMobot *_mobots[MAX_CONNECTED];
  private:
    QMobot *_tmpMobot;
    /* _connectAddresses is an array of pointers to 
       ConfigFile::_addresses */
};

#endif
