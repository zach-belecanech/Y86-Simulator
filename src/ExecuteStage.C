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
#include "E.h"
#include "M.h"
#include "ExecuteStage.h"
#include "W.h"
#include "Status.h"
#include "Tools.h"



bool ExecuteStage::doClockLow(PipeReg ** pregs)
{
    PipeReg * ereg = pregs[EREG];
    PipeReg * mreg = pregs[MREG];
    PipeReg * wreg = pregs[WREG];
    M_bubble = calculateControlSignals(wreg);
    uint64_t stat = ereg -> get(E_STAT);
    uint64_t icode = ereg -> get(E_ICODE);
    uint64_t ifun = ereg -> get(E_IFUN);
    e_Cnd = setCnd(icode, ifun);
    uint64_t valA = ereg -> get(E_VALA);
    uint64_t aluA_val = aluA(ereg);
    uint64_t aluB_val = aluB(ereg);
    uint64_t aluFun_val = aluFun(ereg);
    e_valE = bigALU(ereg, wreg, aluA_val, aluB_val, aluFun_val);
    e_dstE = setDstE(ereg);
    uint64_t dstM = ereg -> get(E_DSTM);    
    setMInput(mreg, stat, icode, e_Cnd, e_valE, valA, e_dstE, dstM);
    return false;
}
void ExecuteStage::setMInput(PipeReg * mreg, uint64_t stat, uint64_t icode,
                            uint64_t cnd, uint64_t valE, uint64_t valA,
                            uint64_t dstE, uint64_t dstM)
{
    mreg->set(M_STAT, stat);
    mreg->set(M_ICODE, icode);
    mreg->set(M_CND, cnd);
    mreg->set(M_VALE, valE);
    mreg->set(M_VALA, valA);
    mreg->set(M_DSTE, dstE);
    mreg->set(M_DSTM, dstM);
}

void ExecuteStage::doClockHigh(PipeReg ** pregs)
{
    PipeReg * mreg = pregs[MREG];
    if (M_bubble == false) {
        mreg->normal();
    } else {
        ((M *)mreg)->bubble();
    }
     
}

uint64_t ExecuteStage::aluA(PipeReg * eregs){
    if (eregs->get(E_ICODE) == IRRMOVQ || eregs->get(E_ICODE) == IOPQ){
        return eregs->get(E_VALA);
    }

    if (eregs->get(E_ICODE) == IIRMOVQ || eregs->get(E_ICODE) == IRMMOVQ || eregs->get(E_ICODE) == IMRMOVQ){
        return eregs->get(E_VALC);
    }

    if (eregs->get(E_ICODE) == ICALL || eregs->get(E_ICODE) == IPUSHQ){
        return -8;
    }

    if (eregs->get(E_ICODE) == IRET || eregs->get(E_ICODE) == IPOPQ)
    {
        return 8;
    }

    return 0;
}

uint64_t ExecuteStage::aluB(PipeReg * eregs){
    if (eregs->get(E_ICODE) == IRMMOVQ || eregs->get(E_ICODE) == IMRMOVQ || eregs->get(E_ICODE) == IOPQ || eregs->get(E_ICODE) == ICALL || 
        eregs->get(E_ICODE) == IPUSHQ || eregs->get(E_ICODE) == IRET || eregs->get(E_ICODE) == IPOPQ){
            return eregs->get(E_VALB);
        }
    
    if (eregs->get(E_ICODE) == IRRMOVQ || eregs->get(E_ICODE) == IIRMOVQ){
        return 0; 
    }

    return 0;
}

uint64_t ExecuteStage::aluFun(PipeReg * eregs){
    if (eregs->get(E_ICODE) == IOPQ){
        return eregs->get(E_IFUN);
    }

    return ADDQ;
}

bool ExecuteStage::set_cc(PipeReg * eregs, PipeReg * wreg){
    if (eregs->get(E_ICODE) == IOPQ && !(m_stat == SADR || m_stat == SINS || m_stat == SHLT) && 
        !(wreg->get(W_STAT) == SADR || wreg->get(W_STAT) == SINS || wreg->get(W_STAT) == SHLT)) {
            return true;
        }
    return false;
}

uint64_t ExecuteStage::setDstE(PipeReg * eregs){
    if ((eregs->get(E_ICODE) == IRRMOVQ) && !e_Cnd){
        return RNONE;
    } 
    return eregs->get(E_DSTE);
}

uint64_t ExecuteStage::bigALU(PipeReg * ereg, PipeReg * wreg, uint64_t aluA, uint64_t aluB, uint64_t aluFun){
    uint64_t bigAnswer = aluA;
    bool overflow = false; 

    if (aluFun == ADDQ){
        overflow = Tools::addOverflow(aluA, aluB);
        bigAnswer = aluA + aluB;
    }

    if (aluFun == SUBQ){
        overflow = Tools::subOverflow(aluA, aluB);
        bigAnswer = aluB - aluA;
    }

    if (aluFun == XORQ){
        bigAnswer = aluA ^ aluB; 
    }

    if (aluFun == ANDQ){
        bigAnswer = aluA & aluB; 
    }
    
    if (set_cc(ereg,wreg)){
        bigCC(bigAnswer, overflow);
    }

    return bigAnswer; 
}

void ExecuteStage::bigCC(uint64_t value, bool overFlow){
    bool err = false;
    cc->setConditionCode(!value, ZF, err);
    cc->setConditionCode(overFlow, OF, err);
    cc->setConditionCode(Tools::sign(value), SF, err);
}

bool ExecuteStage::setCnd(uint64_t icode, uint64_t ifun){
    if (icode != IJXX && icode != ICMOVXX){
        return 0;
    }
    bool error = false;
    bool sf = cc->getConditionCode(SF, error);
    bool of = cc->getConditionCode(OF,error);
    bool zf = cc->getConditionCode(ZF,error);


    switch (ifun)
    {
    case 0:
        return 1;
        break;
    case 1:
        return ((sf ^ of) | zf);
        break;
    case 2:
        return (sf ^ of);
        break;
    case 3: 
        return (zf);
        break;
    case 4:
        return (!zf);
        break;
    case 6: 
        return !(sf ^ of) & !zf;
        break;
    case 5:
        return !(sf ^ of);
        break;
    default:
        break;
    }

    return 0; 
}

bool ExecuteStage::calculateControlSignals(PipeReg * wreg) {
    return ((m_stat == SADR || m_stat == SINS || m_stat == SHLT) || 
            (wreg->get(W_STAT) == SADR || wreg->get(W_STAT) == SINS || wreg->get(W_STAT) == SHLT));
}


