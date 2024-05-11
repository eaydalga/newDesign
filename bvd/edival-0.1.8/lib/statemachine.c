/*
 *  Copyright (C) 2006, 2007 Michael Edgar
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "statemachine.h"
/******************************************************************************/
#define MAX_RUN_STATES 25
/******************************************************************************/
struct EDI_StateMachineStruct {
	EDI_StateHandler startState;
	EDI_StateHandler resumeState;
	EDI_StateHandler runStates[MAX_RUN_STATES];
	int              stateCount;
	EDI_StateHandler endStates[MAX_RUN_STATES];
	int              endCount;
};
/******************************************************************************/
static int seekState(EDI_StateMachine self, EDI_StateHandler h)
{
	int i;
	for(i = 0; i < self->stateCount; i++){
		if(self->runStates[i] == h){
			return i;
		}
	}
	return -1;
}
/******************************************************************************/
EDI_StateMachine EDI_StateMachineCreate(const EDI_Memory_Handling_Suite *suite)
{
	int i;
	EDI_StateMachine new = NULL;
	new = suite->malloc_fcn(sizeof(struct EDI_StateMachineStruct));
	if(new){
		new->startState  = NULL;
		new->resumeState = NULL;
		new->stateCount  = 0;
		new->endCount    = 0;
		for(i = 0; i < MAX_RUN_STATES; i++){
            new->runStates[i] = NULL;
            new->endStates[i] = NULL;
		}
	}
	return new;
}
/******************************************************************************/
void EDI_AddState(EDI_StateMachine self, EDI_StateHandler h, int end)
{
	int check;
	
	check = seekState(self, h);
	if(check < 0){
		self->runStates[self->stateCount] = h;
		if(end){
			self->endStates[self->stateCount] = h;
			self->endCount++;
		}
		self->stateCount++;
	}
	return;
}
/******************************************************************************/
void EDI_RemoveState(EDI_StateMachine self, EDI_StateHandler h, int end)
{
	int check, index;
	
	check = seekState(self, h);
	if(check >= 0){
	    self->stateCount--;
		self->runStates[check] = NULL;
		index = check;
		while(index < self->stateCount){
		    self->runStates[index] = self->runStates[index+1];
		    self->runStates[index+1] = NULL;
		    index++;
		}
		if(end){
			self->endCount--;
			self->endStates[self->stateCount] = NULL;
		}
	}
	return;
}
/******************************************************************************/
void EDI_SetStartState(EDI_StateMachine self, EDI_StateHandler h)
{
	self->startState = h;
	return;
}
/******************************************************************************/
void EDI_SetResumeState(EDI_StateMachine self, EDI_StateHandler h)
{
	self->resumeState = h;
	return;
}
/******************************************************************************/
EDI_StateHandler EDI_GetResumeState(EDI_StateMachine self)
{
    return self->resumeState;
}
/******************************************************************************/
int EDI_StateMachineRun(EDI_StateMachine self, void *parser)
{
	int index = 0;
	EDI_StateHandler handler;
	EDI_StateHandler newState;
	
	if(self->resumeState){
		handler = self->resumeState;
		self->resumeState = NULL;
	} else {
		handler = self->startState;
	}
	while(1){
		newState = handler(parser);
		index = seekState(self, newState);
		if(self->endStates[index]){
			newState(parser);
			break;
		}
		if(self->runStates[index]){
			handler = newState;
		}
	}
	return 0;
}
