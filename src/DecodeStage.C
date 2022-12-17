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
#include "D.h"
#include "E.h"
#include "M.h"
#include "W.h"
#include "DecodeStage.h"

bool DecodeStage::doClockLow(PipeReg ** pregs)
{
    PipeReg * dreg = pregs[DREG];
    PipeReg * ereg = pregs[EREG];
    PipeReg * mreg = pregs[MREG];
    PipeReg * wreg = pregs[WREG];
    uint64_t stat = dreg -> get(D_STAT);
    uint64_t icode = dreg -> get(D_ICODE);
    uint64_t ifun = dreg -> get(D_IFUN);
    uint64_t valC = dreg -> get(D_VALC);
    uint64_t dste = setDstE(dreg);
    uint64_t dstM = setDstM(dreg);
    d_srcA = setSrcA(dreg);
    d_srcB = setSrcB(dreg);
    uint64_t valA = setDvalA(dreg, mreg, ereg, wreg, d_srcA);
    uint64_t valB = setDvalB(dreg, mreg, ereg, wreg, d_srcB);
    E_bubble = calculateControlSignals(ereg);
    setEInput(ereg,stat,icode,ifun,valC,valA,valB,dste,dstM,d_srcA,d_srcB);
    return false;

}

void DecodeStage::setEInput(PipeReg * ereg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun, uint64_t valC, uint64_t valA,
                           uint64_t valB, uint64_t dste, uint64_t dstm, uint64_t srcA, uint64_t srcB)
{
   ereg->set(E_STAT, stat);
   ereg->set(E_ICODE, icode);
   ereg->set(E_IFUN, ifun);
   ereg->set(E_VALC, valC);
   ereg->set(E_VALA, valA);
   ereg->set(E_VALB, valB);
   ereg->set(E_DSTE, dste);
   ereg->set(E_DSTM, dstm);
   ereg->set(E_SRCA, srcA);
   ereg->set(E_SRCB, srcB); 
}

void DecodeStage::doClockHigh(PipeReg ** pregs)
{
    PipeReg * ereg = pregs[EREG];
    if(E_bubble){
        ((E *)ereg)->bubble();
    }
    else{
        ereg->normal();
    }
}

uint64_t DecodeStage::setSrcA(PipeReg * dreg){
    if (dreg->get(D_ICODE) == IRRMOVQ || dreg->get(D_ICODE) == IRMMOVQ || dreg->get(D_ICODE) == IOPQ || dreg->get(D_ICODE) == IPUSHQ){
        return dreg->get(D_RA);
    }

    if (dreg->get(D_ICODE) == IPOPQ || dreg->get(D_ICODE) == IRET){
        return RSP;
    }

    return RNONE;
}

uint64_t DecodeStage::setSrcB(PipeReg * dreg){
    if (dreg->get(D_ICODE) == IOPQ || dreg->get(D_ICODE) == IRMMOVQ || dreg->get(D_ICODE) == IMRMOVQ){
        return dreg->get(D_RB);
    }

    if (dreg->get(D_ICODE) == IPUSHQ || dreg->get(D_ICODE) == IPOPQ || dreg->get(D_ICODE) == ICALL || dreg->get(D_ICODE) == IRET){
        return RSP;
    }
    return RNONE;
}

uint64_t DecodeStage::setDstE(PipeReg * dreg){
    if (dreg->get(D_ICODE) == IRRMOVQ || dreg->get(D_ICODE) == IIRMOVQ || dreg->get(D_ICODE) == IOPQ){
        return dreg->get(D_RB);
    }

    if (dreg->get(D_ICODE) == IPUSHQ || dreg->get(D_ICODE) == IPOPQ || dreg->get(D_ICODE) == ICALL || dreg->get(D_ICODE) == IRET){
        return RSP;
    }
    return RNONE;
}

uint64_t DecodeStage::setDstM(PipeReg * dreg){
    if (dreg->get(D_ICODE) == IMRMOVQ || dreg->get(D_ICODE) == IPOPQ){
        return dreg->get(D_RA);
    }
    return RNONE;
}

uint64_t DecodeStage::setDvalA(PipeReg * dreg, PipeReg * mreg, PipeReg * ereg, PipeReg * wreg, uint64_t srcA) {
    if (dreg->get(D_ICODE) == ICALL || dreg->get(D_ICODE) == IJXX) {
        return dreg->get(D_VALP);
    }
    if (srcA == RNONE) {
        return 0;
    }
    bool error = false;
    if (srcA == e_dstE) {
        return e_valE;
    }
    if (srcA == mreg->get(M_DSTM)) {
        return m_valM;
    }
    if (srcA == mreg->get(M_DSTE)) {
        return mreg->get(M_VALE);
    }
    if (srcA == wreg->get(W_DSTM)) {
        return wreg->get(W_VALM);
    }
    if (srcA == wreg->get(W_DSTE)) {
        return wreg->get(W_VALE);
    }
    return rf->readRegister(srcA,error);
}

uint64_t DecodeStage::setDvalB(PipeReg * dreg, PipeReg * mreg, PipeReg * ereg, PipeReg * wreg, uint64_t srcB) {
    if (srcB == RNONE) {
        return 0;
    }
    bool error = false;
    if (srcB == e_dstE) {
        return e_valE;
    }
    if (srcB == mreg->get(M_DSTM)) {
        return m_valM;
    }
    if (srcB == mreg->get(M_DSTE)) {
        return mreg->get(M_VALE);
    }
    if (srcB == wreg->get(W_DSTM)) {
        return wreg->get(W_VALM);
    }
    if (srcB == wreg->get(W_DSTE)) {
        return wreg->get(W_VALE);
    }
    return rf->readRegister(srcB,error);
}

bool DecodeStage::calculateControlSignals(PipeReg * ereg){
    return ((ereg->get(E_ICODE) == IJXX && !e_Cnd) || ((ereg->get(E_ICODE) == IMRMOVQ || ereg->get(E_ICODE) == IPOPQ) 
            && (ereg->get(E_DSTM) == d_srcA || ereg->get(E_DSTM) == d_srcB)));
}

