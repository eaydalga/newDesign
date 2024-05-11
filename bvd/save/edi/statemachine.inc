<?php
/******************************************************************************/
function seekState($self, EDI_StateHandler $h)
{
    for($i = 0; $i < $self->stateCount; $i++){
        if($self->runStates[$i] == h){
            return $i;
        }
    }
    return -1;
}
/******************************************************************************/
function EDI_StateMachineCreate($suite)
{
    $new = EDI_StateMachine;
    $new = $suite->malloc_fcn(sizeof(EDI_StateMachineStruct));
    if($new){
        $new->startState  = '';
        $new->resumeState = '';
        $new->stateCount  = 0;
        $new->endCount    = 0;
        for($i = 0; $i < MAX_RUN_STATES; $i++){
            $new->runStates[$i] = '';
            $new->endStates[$i] = '';
        }
    }
    return $new;
}
/******************************************************************************/
function EDI_AddState($self, $h, $myend)
{
    $check = seekState($self, $h);
    if($check < 0){
        $self->runStates[$self->stateCount] = h;
        if($myend){
            $self->endStates[$self->stateCount] = h;
            $self->endCount++;
        }
        $self->stateCount++;
    }
    return;
}
/******************************************************************************/
function EDI_RemoveState($self, $h, $myend)
{
    $check = seekState($self, $h);
    if($check >= 0){
        $self->stateCount--;
        $self->runStates[$check] = '';
        $index = $check;
        while($index < $self->stateCount){
            $self->runStates[$index] = $self->runStates[$index+1];
            $self->runStates[$index+1] = '';
            $index++;
        }
        if($myend){
            $self->endCount--;
            $self->endStates[$self->stateCount] = '';
        }
    }
    return;
}
/******************************************************************************/
function EDI_SetStartState($self, $h)
{
    $self->startState = $h;
    return;
}
/******************************************************************************/
function EDI_SetResumeState($self, $h)
{
    $self->resumeState = $h;
    return;
}
/******************************************************************************/
function EDI_GetResumeState($self)
{
    return $self->resumeState;
}
/******************************************************************************/
function EDI_StateMachineRun($self, $parser)
{
    $index = 0;
    $handler = EDI_StateHandler;
    $newState = EDI_StateHandler;
    
    if($self->resumeState){
        $handler = $self->resumeState;
        $self->resumeState = '';
    } else {
        $handler = $self->startState;
    }
    while(1){
        $newState = handler($parser);
        $index = seekState($self, $newState);
        if($self->endStates[$index]){
            newState($parser);
            break;
        }
        if($self->runStates[$index]){
            $handler = $newState;
        }
    }
    return 0;
}
?>
