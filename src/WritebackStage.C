#include <iostream>
#include <cstdint>
#include <stdio.h>
#include "Memory.h"
#include "Instructions.h"
#include "RegisterFile.h"
#include "ConditionCodes.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "Stage.h"
#include "W.h"
#include "WritebackStage.h"
#include "Status.h"
 
bool WritebackStage::doClockLow(PipeReg ** pregs)
{
    PipeReg * wreg = pregs[WREG];

    
    return wreg->get(W_STAT) != SAOK; 
}

void WritebackStage::doClockHigh(PipeReg ** pregs)
{
    PipeReg * wreg = pregs[WREG];
    bool err = false;
    rf->writeRegister(wreg->get(W_VALE), wreg->get(W_DSTE), err);
    rf->writeRegister(wreg->get(W_VALM), wreg->get(W_DSTM), err);
}