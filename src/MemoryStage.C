#include <iostream>
#include <cstdint>
#include <stdio.h>
#include "Memory.h"
#include "RegisterFile.h"
#include "ConditionCodes.h"
#include "Instructions.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "Stage.h"
#include "M.h"
#include "W.h"
#include "MemoryStage.h"
#include "Status.h"


bool MemoryStage::doClockLow(PipeReg ** pregs)
{
    bool err = false;
    m_valM = 0;
    PipeReg * mreg = pregs[MREG];
    PipeReg * wreg = pregs[WREG];

    
    uint64_t icode = mreg -> get(M_ICODE);
    uint64_t valE = mreg -> get(M_VALE);
    uint64_t dstE = mreg -> get(M_DSTE);
    uint64_t dstM = mreg -> get(M_DSTM);
    uint64_t valA = mreg -> get(M_VALA);
    int32_t address = Addr(mreg);

    if (mem_write(mreg, icode)) {
        mem->putLong(valA, address, err);
    }

    if (mem_read(mreg)) {
        m_valM = mem->getLong(address, err);
    }

    m_stat = setM_Stat(err, mreg);

    setWInput(wreg, m_stat, icode, valE, m_valM, dstE, dstM); 
    return false;
}

void MemoryStage::setWInput(PipeReg * wreg, uint64_t stat, uint64_t icode,
                            uint64_t valE, uint64_t valM, uint64_t dstE, 
                            uint64_t dstM)
{
    wreg->set(W_STAT, stat);
    wreg->set(W_ICODE, icode);
    wreg->set(W_VALE, valE);
    wreg->set(W_VALM, valM);
    wreg->set(W_DSTE, dstE);
    wreg->set(W_DSTM, dstM); 
}

void MemoryStage::doClockHigh(PipeReg ** pregs)
{
    PipeReg * wreg = pregs[WREG];
    wreg->normal(); 
}

int32_t MemoryStage::Addr(PipeReg * mregs) {
    if (mregs->get(M_ICODE) == IRMMOVQ || mregs->get(M_ICODE) == IPUSHQ || mregs->get(M_ICODE) == ICALL || mregs->get(M_ICODE) == IMRMOVQ){
        return mregs->get(M_VALE);
    }
    
    if (mregs->get(M_ICODE) == IPOPQ || mregs->get(M_ICODE) == IRET) {
        return mregs->get(M_VALA); 
    }

    return 0; 
}

bool MemoryStage::mem_read(PipeReg * mregs) {
    if (mregs->get(M_ICODE) == IMRMOVQ || mregs->get(M_ICODE) == IPOPQ || mregs->get(M_ICODE) == IRET) {
        return 1; 
    }
    
    return 0; 
}

bool MemoryStage::mem_write(PipeReg * mregs, uint64_t icode) {
    if (icode == IRMMOVQ || icode == IPUSHQ || icode == ICALL) {
        return 1; 
    }
    
    return 0; 
}

uint64_t MemoryStage::setM_Stat(bool err, PipeReg * mreg) {
    if (err) {
        return SADR;
    }

    return mreg->get(M_STAT);
}

