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

#ifndef StateMachine_INCLUDED
#define StateMachine_INCLUDED

#include "common.h"

struct EDI_StateMachineStruct;
typedef struct EDI_StateMachineStruct *EDI_StateMachine;

EDI_StateMachine EDI_StateMachineCreate(const EDI_Memory_Handling_Suite *);
void             EDI_AddState(EDI_StateMachine, EDI_StateHandler, int);
void             EDI_RemoveState(EDI_StateMachine, EDI_StateHandler, int);
void             EDI_SetStartState(EDI_StateMachine, EDI_StateHandler);
void             EDI_SetResumeState(EDI_StateMachine, EDI_StateHandler);
EDI_StateHandler EDI_GetResumeState(EDI_StateMachine);
int              EDI_StateMachineRun(EDI_StateMachine, void *);

#endif /* StateMachine_INCLUDED */
