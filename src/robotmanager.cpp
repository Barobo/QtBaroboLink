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

#include <stdio.h>
#include <stdlib.h>
#include "robotmanager.h"

CRobotManager::CRobotManager()
{
  int i;
  for(i = 0; i < MAX_CONNECTED; i++) {
    _mobots[i] = NULL;
  }
  for(i = 0; i < numEntries(); i++) {
    _mobots[i] = new QMobot(0);
  }
  _isPlaying = false;
}

CRobotManager::~CRobotManager()
{
}

bool CRobotManager::isConnected(int index) const
{
  if((index >= numEntries()) || index < 0) {
    return false;
  }
  if(_mobots[index] == NULL) {
    return false;
  }
  return _mobots[index]->isConnected();
}

int CRobotManager::addEntry(const char* entry)
{
  int rc;
  if((rc = ConfigFile::addEntry(entry)) != 0) {
    return rc;
  }

  /* Adjust the array of mobots */
  for(int i = (numEntries()-1); i >= 0; i--) {
    _mobots[i+1] = _mobots[i];
  }
  _mobots[0] = new QMobot(0);
  return 0;
}

int CRobotManager::addEntry(string entry)
{
  return CRobotManager::addEntry(entry.c_str());
}

int CRobotManager::disconnectAll()
{
  int i;
  for(i = 0; i < MAX_CONNECTED; i++) {
    disconnect(i);
  }
  return 0;
}

void CRobotManager::moveMobot(int destIndex, int srcIndex)
{
  _mobots[destIndex] = _mobots[srcIndex];
  _mobots[srcIndex] = NULL;
}

int CRobotManager::moveEntryUp(int index)
{
  int rc;
  if((rc = ConfigFile::moveEntryUp(index)) != 0) {
    return rc;
  }
  /* Just swap the data of this entry with the one above it */
  QMobot* tmp;
  tmp = _mobots[index-1];
  _mobots[index-1] = _mobots[index];
  _mobots[index] = tmp;
  return 0;
}

int CRobotManager::moveEntryDown(int index)
{
  int rc;
  if((rc = ConfigFile::moveEntryDown(index)) != 0) {
    return rc;
  }
  /* Swap with entry below */
  QMobot *tmp;
  tmp = _mobots[index+1];
  _mobots[index+1] = _mobots[index];
  _mobots[index] = tmp;
  return 0;
}

int CRobotManager::insertEntry(const char* entry, int index)
{
  int rc;
  if((rc = ConfigFile::insertEntry(entry, index)) != 0) {
    return rc;
  }
  /* Move the existing mobot array */
  int i;
  for(i = numEntries(); i >= index; i--) {
    _mobots[i+1] = _mobots[i];
  }
  _mobots[index] = new QMobot(0);
  return 0;
}

bool CRobotManager::isPlaying()
{
  return _isPlaying;
}

int CRobotManager::connectIndex(int index)
{
  if(isConnected(index)) {
    return 0;
  }
  char name[80];
  sprintf(name, "mobot%d", numConnected()+1);
  int err;
  if(_mobots[index] == NULL) {
    _mobots[index] = new QMobot(0);
  }
  _mobots[index]->init(name);
  printf("(barobo) INFO: connecting %s at address %s\n", name, getEntry(index));
  err = _mobots[index]->connectWithAddress(getEntry(index), 1 );
  return err;
}

int CRobotManager::disconnect(int index)
{
  if(_mobots[index] == NULL) {
    return -1;
  }
  /* First check to see how the robot is connected. If it is a TTY connection,
   * we just want to remove the robot from the list of connected robots, but we
   * don't actually want to disconnect. */
  _mobots[index]->disconnectRobot();
  //free(_mobots[index]);
  //_mobots[index] = NULL;
  return 0;
}

RecordMobot* CRobotManager::getUnboundMobot()
{
  int i;
  RecordMobot* mobot;
  mobot = NULL;
  for(i = 0; i < numEntries(); i++) {
    if(_mobots[i] == NULL) {
      continue;
    }
    if(_mobots[i]->isBound() == false) {
      mobot = _mobots[i];
      break;
    }
  }
  return mobot;
}

int CRobotManager::numConnected()
{
  int num = 0, i;
  for(i = 0; i < MAX_CONNECTED; i++) {
    if(_mobots[i] == NULL) {
      continue;
    }
    if(_mobots[i] != NULL) {
      num++;
    }
  }
  return num;
}

int CRobotManager::numAvailable()
{
	return numEntries() - numConnected();
}

void CRobotManager::record()
{
  int i;
  RecordMobot* mobot;
  for(i = 0; i < numConnected(); i++) {
    mobot = getMobot(i);
    mobot->record();
  }
}

int CRobotManager::remove(int index)
{
  int rc;
  disconnect(index);
  if((rc = ConfigFile::remove(index)) != 0) {
    return rc;
  }
  /* Adjust the list of mobots */
  _tmpMobot = _mobots[index];
  delete _tmpMobot;
  int i;
  for(i = index; i < numEntries(); i++) {
    _mobots[i] = _mobots[i+1];
  }
  return 0;
}

void CRobotManager::restoreSavedMobot(int index)
{
  if(_mobots[index] != NULL) {
    free(_mobots[index]);
  }
  _mobots[index] = _tmpMobot;
  _tmpMobot = NULL;
}

void CRobotManager::addDelay(double seconds)
{
  int i;
  RecordMobot* mobot;
  for(i = 0; i < numConnected(); i++) {
    mobot = getMobot(i);
    mobot->addDelay(seconds);
  }
}

void* robotManagerPlayThread(void* arg)
{
  CRobotManager *rm = (CRobotManager*)arg;
  RecordMobot* mobot;
  if(rm->numConnected() <= 0) {
    rm->_isPlaying = false;
    return NULL;
  }
  int index, i;
  /* Go through each motion */
  for(index = 0; index < rm->getMobot(0)->numMotions(); index++) {
    /* Go through each mobot */
    for(i = 0; i < rm->numConnected(); i++) {
      mobot = rm->getMobot(i);
      if(mobot->getMotionType(index) == MOTION_SLEEP) {
        /* Sleep the correct amount of time and break */
        mobot->play(index);
        break;
      } else if (mobot->getMotionType(index) == MOTION_POS) {
        mobot->play(index);
      } else {
        fprintf(stderr, "MEMORY ERROR %s:%d\n", __FILE__, __LINE__);
        rm->_isPlaying = false;
        return NULL;
      }
    }
  }
  rm->_isPlaying = false;
  return NULL;
}

void CRobotManager::play()
{
  _isPlaying = true;
  /* FIXME */
  //THREAD_T thread;
  //THREAD_CREATE(&thread, robotManagerPlayThread, this);
}

RecordMobot* CRobotManager::getMobot(int connectIndex)
{
	if(connectIndex < 0 || connectIndex >= numConnected()) {
		return NULL;
	}
  int i;
  for(i = 0; i <= MAX_CONNECTED ; i++ ) {
    if(_mobots[i] == NULL) {continue;}
    if((_mobots[i] != NULL) && (_mobots[i]->connectStatus() == RMOBOT_CONNECTED)) {
      connectIndex--;
    }
    if(connectIndex < 0) {
      break;
    }
  }
	return _mobots[i];
}

RecordMobot* CRobotManager::getMobotIndex(int index)
{
  return _mobots[index];
}

string* CRobotManager::generateChProgram(bool looped, bool holdOnExit)
{
  string buf;
  string *program = new string();
  char tbuf[200];
  int i, j;
  char tmp[80];
  *program += "/* Program generated by BaroboLink */\n";

  for(i = 0; i < numConnected(); i++) {
    if(
        (getMobot(i)->formFactor() == MOBOTFORM_I) ||
        (getMobot(i)->formFactor() == MOBOTFORM_L) 
      ) 
    {
      *program += "#include <linkbot.h>\n\n";
      break;
    }
  }

  for(i = 0; i < numConnected(); i++) {
    if(
        (getMobot(i)->formFactor() != MOBOTFORM_I) &&
        (getMobot(i)->formFactor() != MOBOTFORM_L) 
      ) 
    {
      *program += "#include <mobot.h>\n\n";
      break;
    }
  }

  //*program += "int main() {\n";

  /* Declare the appropiate number of CMobot variables */
  for(i = 0; i < numConnected(); i++) {
    if(getMobot(i)->formFactor() == MOBOTFORM_I) {
      sprintf(tbuf, "CLinkbotI robot%d;\n", i+1);
    } else if (getMobot(i)->formFactor() == MOBOTFORM_L) {
      sprintf(tbuf, "CLinkbotL robot%d;\n", i+1);
    } else {
      sprintf(tbuf, "CMobot robot%d;\n", i+1);
    }
    buf.assign(tbuf);
    *program += buf;
  }

  /* Connect to each one */
  for(i = 0; i < numConnected(); i++) {
    if(getMobot(i)) {
    }
    sprintf(tbuf, "robot%d.connect();\n", i+1);
    buf.assign(tbuf);
    *program += buf;
  }

  if(looped) {
    *program += "/* Set the \"num\" variable to the number of times to loop. */\n";
    *program += "int num = 3;\n";
    *program += "int i;\n";
    *program += "for(i = 0; i < num; i++) {";
  }
  /* Make sure there were connected mobots */
  if(getMobot(0) != NULL) {
    /* Set all of the robot names so they are in order */
    for(j = 0; j < numConnected(); j++) {
      sprintf(tmp, "robot%d", j+1);
      getMobot(j)->setName(tmp);
    }
    /* Now go through each motion */
    for(i = 0; i < getMobot(0)->numMotions(); i++) {
      *program += "\n";
      /* First, print the comment for the motion */
      sprintf(tbuf, "/* %s */\n", getMobot(0)->getMotionName(i));
      buf.assign(tbuf);
      //*program += "    ";
      if(looped) *program += "    ";
      *program += buf;
      /* Now, print each robots motion */
      for(j = 0; j < numConnected(); j++) {
        if(numConnected() == 1) {
          getMobot(j)->getChMotionStringB(i, tbuf);
        } else {
          getMobot(j)->getChMotionString(i, tbuf);
        }
        buf.assign(tbuf);
        buf += "\n";
        if(looped) *program += "    ";
        *program += buf;
        if(getMobot(j)->getMotionType(i) == MOTION_SLEEP) {
          break;
        }
      }
      /* Make sure all the robots are done moving */
      for(j = 0; j < numConnected(); j++) {
        if(getMobot(j)->getMotionType(i) == MOTION_SLEEP) {
          break;
        }
        if(numConnected() > 1) {
          sprintf(tbuf, "robot%d.moveWait();\n", j+1);
          buf.assign(tbuf);
          if(looped) *program += "    ";
          *program += buf;
        }
      }
    }
  }
  if(looped) {
    *program += "}\n";
  }

  if(holdOnExit) {
    for(i = 0; i < numConnected(); i++) {
      sprintf(tbuf, "robot%d.setExitState(ROBOT_HOLD);\n", i+1);
      buf.assign(tbuf);
      *program += buf;
    }
  }

  //*program += "return 0;\n";
  //*program += "}\n";
  *program += "\n";
  return program;
}

string* CRobotManager::generateCppProgram(bool looped, bool holdOnExit)
{
  string buf;
  string *program = new string();
  char tbuf[200];
  int i, j;
  char tmp[80];
  *program += "/* Program generated by BaroboLink */\n";

  for(i = 0; i < numConnected(); i++) {
    if(
        (getMobot(i)->formFactor() == MOBOTFORM_I) ||
        (getMobot(i)->formFactor() == MOBOTFORM_L) 
      ) 
    {
      *program += "#include <linkbot.h>\n\n";
      break;
    }
  }

  for(i = 0; i < numConnected(); i++) {
    if(
        (getMobot(i)->formFactor() != MOBOTFORM_I) &&
        (getMobot(i)->formFactor() != MOBOTFORM_L) 
      ) 
    {
      *program += "#include <mobot.h>\n\n";
      break;
    }
  }

  *program += "int main() {\n";

  /* Declare the appropiate number of CMobot variables */
  for(i = 0; i < numConnected(); i++) {
    if(getMobot(i)->formFactor() == MOBOTFORM_I) {
      sprintf(tbuf, "    CLinkbotI robot%d;\n", i+1);
    } else if (getMobot(i)->formFactor() == MOBOTFORM_L) {
      sprintf(tbuf, "    CLinkbotL robot%d;\n", i+1);
    } else {
      sprintf(tbuf, "    CMobot robot%d;\n", i+1);
    }
    buf.assign(tbuf);
    *program += buf;
  }

  /* Connect to each one */
  for(i = 0; i < numConnected(); i++) {
    if(getMobot(i)) {
    }
    sprintf(tbuf, "    robot%d.connect();\n", i+1);
    buf.assign(tbuf);
    *program += buf;
  }

  if(looped) {
    *program += "    /* Set the \"num\" variable to the number of times to loop. */\n";
    *program += "    int num = 3;\n";
    *program += "    int i;\n";
    *program += "    for(i = 0; i < num; i++) {";
  }
  /* Make sure there were connected mobots */
  if(getMobot(0) != NULL) {
    /* Set all of the robot names so they are in order */
    for(j = 0; j < numConnected(); j++) {
      sprintf(tmp, "    robot%d", j+1);
      getMobot(j)->setName(tmp);
    }
    /* Now go through each motion */
    for(i = 0; i < getMobot(0)->numMotions(); i++) {
      *program += "\n";
      /* First, print the comment for the motion */
      sprintf(tbuf, "/* %s */\n", getMobot(0)->getMotionName(i));
      buf.assign(tbuf);
      *program += "    ";
      if(looped) *program += "    ";
      *program += buf;
      /* Now, print each robots motion */
      for(j = 0; j < numConnected(); j++) {
        if(numConnected() == 1) {
          getMobot(j)->getChMotionStringB(i, tbuf);
        } else {
          getMobot(j)->getChMotionString(i, tbuf);
        }
        buf.assign(tbuf);
        buf += "\n";
        if(looped) *program += "    ";
        *program += buf;
        if(getMobot(j)->getMotionType(i) == MOTION_SLEEP) {
          break;
        }
      }
      /* Make sure all the robots are done moving */
      for(j = 0; j < numConnected(); j++) {
        if(getMobot(j)->getMotionType(i) == MOTION_SLEEP) {
          break;
        }
        if(numConnected() > 1) {
          sprintf(tbuf, "robot%d.moveWait();\n", j+1);
          buf.assign(tbuf);
          if(looped) *program += "    ";
          *program += buf;
        }
      }
    }
  }
  if(looped) {
    *program += "    }\n";
  }

  if(holdOnExit) {
    for(i = 0; i < numConnected(); i++) {
      sprintf(tbuf, "    robot%d.setExitState(ROBOT_HOLD);\n", i+1);
      buf.assign(tbuf);
      *program += buf;
    }
  }

  *program += "    return 0;\n";
  *program += "}\n";
  *program += "\n";
  return program;
}

string* CRobotManager::generatePythonProgram(bool looped, bool holdOnExit)
{
  string buf;
  string *program = new string();
  char tbuf[200];
  int i, j;
  char tmp[80];
  *program += "#!/usr/bin/env python\n";
  *program += "# Program generated by BaroboLink\n";
  *program += "from barobo.linkbot import *\n";
  *program += "import time\n";

  //*program += "int main() {\n";

  /* Declare the appropiate number of CMobot variables */
  for(i = 0; i < numConnected(); i++) {
    if(getMobot(i)->formFactor() == MOBOTFORM_I) {
      sprintf(tbuf, "robot%d = Linkbot()\n", i+1);
    } else if (getMobot(i)->formFactor() == MOBOTFORM_L) {
      sprintf(tbuf, "robot%d = Linkbot()\n", i+1);
    } else {
      sprintf(tbuf, "robot%d = Mobot();\n", i+1);
    }
    buf.assign(tbuf);
    *program += buf;
  }

  /* Connect to each one */
  for(i = 0; i < numConnected(); i++) {
    if(getMobot(i)) {
    }
    sprintf(tbuf, "robot%d.connect()\n", i+1);
    buf.assign(tbuf);
    *program += buf;
  }

  if(looped) {
    *program += "# Set the \"num\" variable to the number of times to loop\n";
    *program += "num = 3\n";
    *program += "for i in range (0, num):";
  }
  /* Make sure there were connected mobots */
  if(getMobot(0) != NULL) {
    /* Set all of the robot names so they are in order */
    for(j = 0; j < numConnected(); j++) {
      sprintf(tmp, "robot%d", j+1);
      getMobot(j)->setName(tmp);
    }
    /* Now go through each motion */
    for(i = 0; i < getMobot(0)->numMotions(); i++) {
      *program += "\n";
      /* First, print the comment for the motion */
      sprintf(tbuf, "# %s \n", getMobot(0)->getMotionName(i));
      buf.assign(tbuf);
      //*program += "    ";
      if(looped) *program += "    ";
      *program += buf;
      /* Now, print each robots motion */
      for(j = 0; j < numConnected(); j++) {
        if(numConnected() == 1) {
          getMobot(j)->getPythonMotionStringB(i, tbuf);
        } else {
          getMobot(j)->getPythonMotionString(i, tbuf);
        }
        buf.assign(tbuf);
        buf += "\n";
        if(looped) *program += "    ";
        *program += buf;
        if(getMobot(j)->getMotionType(i) == MOTION_SLEEP) {
          break;
        }
      }
      /* Make sure all the robots are done moving */
      for(j = 0; j < numConnected(); j++) {
        if(getMobot(j)->getMotionType(i) == MOTION_SLEEP) {
          break;
        }
        if(numConnected() > 1) {
          sprintf(tbuf, "robot%d.moveWait()\n", j+1);
          buf.assign(tbuf);
          if(looped) *program += "    ";
          *program += buf;
        }
      }
    }
  }

  if(holdOnExit) {
    for(i = 0; i < numConnected(); i++) {
      sprintf(tbuf, "robot%d.setExitState(ROBOT_HOLD)\n", i+1);
      buf.assign(tbuf);
      *program += buf;
    }
  }

  //*program += "return 0;\n";
  //*program += "}\n";
  *program += "\n";
  return program;
}
