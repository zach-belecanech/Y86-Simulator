//TODO add more #includes as you need them
#include <iostream>
#include <cstdint>
#include <stdio.h>
#include "Memory.h"
#include "ConditionCodes.h"
#include "Instructions.h"
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "W.h"
#include "M.h"
#include "F.h"
#include "D.h"
#include "E.h"
#include "Stage.h"
#include "FetchStage.h"
#include "Status.h"
#include "Debug.h"
#include "Tools.h"

/*
 * doClockLow
 *
 * Performs the Fetch stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
bool FetchStage::doClockLow(PipeReg ** pregs)
{
   PipeReg * freg = pregs[FREG];  //pointer to object representing F pipeline register
   PipeReg * dreg = pregs[DREG];
   PipeReg * ereg = pregs[EREG];
   PipeReg * mreg = pregs[MREG];
   PipeReg * wreg = pregs[WREG];

   
   //pointer to object representing D pipeline register
   bool mem_error = false;
   uint64_t icode = INOP, ifun = FNONE, rA = RNONE, rB = RNONE;
   uint64_t valC = 0, valP = 0, stat = 0, predPC = 0;
   bool needvalC = false;
   bool needregId = false;
  
   //TODO 
   //select PC value and read byte from memory
   //set icode and ifun using byte read from memory
   uint64_t f_pc = selectPC(freg, mreg, wreg);
   uint64_t instruct = mem->getByte(f_pc, mem_error);

	icode = Tools::getBits(instruct, 4, 7);  
	ifun = Tools::getBits(instruct, 0, 3);

	if(mem_error) {
		icode = INOP;
		ifun = FNONE;
	}
   //status of this instruction is SAOK (this will change in a later lab)

   stat = f_stat(mem_error, instr_valid(icode), icode, freg);

   //TODO
   //In order to calculate the address of the next instruction,
   //you'll need to know whether this current instruction has an
   //immediate field and a register byte. (Look at the instruction encodings.)
   needvalC = needValC(icode);
   needregId = needRegIds(icode);

   getRegIds(f_pc, needregId, &rA, &rB);
   valC = buildValC(f_pc, needregId, needvalC); 

   //TODO
   //determine the address of the next sequential function
   valP = PC_Increment(f_pc, needregId, needvalC); 
   
   //TODO
   //calculate the predicted PC value
   predPC = predictPC(icode,valC,valP); 

   //set the input for the PREDPC pipe register field in the F register
   freg->set(F_PREDPC, predPC);
   calculateControlSignals(ereg,dreg,mreg);
   //set the inputs for the D register
   setDInput(dreg, stat, icode, ifun, rA, rB, valC, valP);
   return false;
}

/* doClockHigh
 *
 * applies the appropriate control signal to the F
 * and D register intances
 * 
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
*/
void FetchStage::doClockHigh(PipeReg ** pregs)
{
   PipeReg * freg = pregs[FREG];  //pointer to 
   PipeReg * dreg = pregs[DREG];
   if (!F_stall){
      freg->normal();
   }
   if (!D_stall){
      dreg->normal();
   }
   if (D_Bubble) {
      ((D *)dreg)->bubble();
   }
}

/* setDInput
 * provides the input to potentially be stored in the D register
 * during doClockHigh
 *
 * @param: dreg - pointer to the D register instance
 * @param: stat - value to be stored in the stat pipeline register within D
 * @param: icode - value to be stored in the icode pipeline register within D
 * @param: ifun - value to be stored in the ifun pipeline register within D
 * @param: rA - value to be stored in the rA pipeline register within D
 * @param: rB - value to be stored in the rB pipeline register within D
 * @param: valC - value to be stored in the valC pipeline register within D
 * @param: valP - value to be stored in the valP pipeline register within D
*/
void FetchStage::setDInput(PipeReg * dreg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun, uint64_t rA, uint64_t rB,
                           uint64_t valC, uint64_t valP)
{
   dreg->set(D_STAT, stat);
   dreg->set(D_ICODE, icode);
   dreg->set(D_IFUN, ifun);
   dreg->set(D_RA, rA);
   dreg->set(D_RB, rB);
   dreg->set(D_VALC, valC);
   dreg->set(D_VALP, valP);
}

//TODO
//Write your selectPC, needRegIds, needValC, PC increment, and predictPC methods
//Remember to add declarations for these to FetchStage.h
// Here is the HCL describing the behavior for some of these methods. 
uint64_t FetchStage::selectPC(PipeReg * freg, PipeReg * mreg, PipeReg * wreg) {
   if (mreg->get(M_ICODE) == IJXX && !mreg->get(M_CND)) {
      return mreg->get(M_VALA);
   }
   if(wreg->get(W_ICODE) == IRET) {
      return wreg->get(W_VALM);
   }
   return freg->get(F_PREDPC);
}

bool FetchStage::needRegIds(uint64_t f_icode) {
   if(f_icode == IRRMOVQ || f_icode == IOPQ || f_icode == IPUSHQ || f_icode == IPOPQ || f_icode == IIRMOVQ || f_icode == IRMMOVQ || f_icode == IMRMOVQ) {
		return true;
	} 

	return false;
}

bool FetchStage::needValC(uint64_t f_icode) {
	if(f_icode == IIRMOVQ || f_icode == IRMMOVQ || f_icode == IMRMOVQ || f_icode == IJXX || f_icode == ICALL) {
		return true;
	} 
	return false;
}

uint64_t FetchStage::predictPC(uint64_t f_icode, uint64_t f_valC, uint64_t f_valP) {
   if (f_icode == IJXX || f_icode == ICALL) {
      return f_valC;
   }
   return f_valP;
}

uint64_t FetchStage::PC_Increment(uint64_t f_pc, bool needRegId, bool needValC) {
	if (needRegId && needValC) {
		return f_pc + 10;
	}else if (needRegId && !needValC) {
		return f_pc + 2;
	}else if (!needRegId && needValC) {
		return f_pc + 9;
	}else {
		return f_pc + 1;
	}
}

void FetchStage::getRegIds(uint64_t f_pc, bool needRegId, uint64_t * rA, uint64_t * rB){
   bool err = false;
   uint64_t reg_byte = mem->getByte(f_pc + 1, err);
   if (needRegId == true){
      *rA = Tools::getBits(reg_byte, 4, 7);
      *rB = Tools::getBits(reg_byte, 0, 3);
   }
}

uint64_t FetchStage::buildValC(uint64_t f_pc, bool needRegId, bool needValC){
   if (needValC == true) {
      uint8_t numAns[8];
      bool err = false;
      if (needRegId == true) {
         for (int i = 0; i < 8; i++) {
            numAns[i] = mem->getByte(f_pc + i + 2, err);
         }
      } else {
         for (int i = 0; i < 8; i++) {
            numAns[i] = mem->getByte(f_pc + i + 1, err);
         }
      }
      return Tools::buildLong(numAns);
   } else {
      return 0;
   }
}

bool FetchStage::instr_valid(uint64_t f_icode) {
   if (f_icode == INOP || f_icode == IHALT || f_icode == IRRMOVQ || f_icode == IIRMOVQ ||
         f_icode == IRMMOVQ || f_icode == IMRMOVQ || f_icode == IOPQ || f_icode == IJXX ||
         f_icode == ICALL || f_icode == IRET || f_icode == IPUSHQ || f_icode == IPOPQ) {
            return true;
         }
   return false;
}

uint64_t FetchStage::f_stat(bool mem_error, bool instr_valid, uint64_t f_icode, PipeReg * freg) {
   if (mem_error) {
      return SADR;
   }

   if(instr_valid == false) {
      return SINS;
   }

   if(f_icode == IHALT) {
      return SHLT;
   }

   return SAOK;
}

void FetchStage::calculateControlSignals(PipeReg * ereg, PipeReg * dreg, PipeReg * mreg) {
   F_stall = stall_F(ereg, dreg, mreg);
   D_stall = stall_D(ereg);
   D_Bubble = bubble_D(ereg, dreg, mreg);
}

bool FetchStage::stall_F(PipeReg * ereg, PipeReg * dreg, PipeReg * mreg) {
   return (((ereg->get(E_ICODE) == IMRMOVQ || ereg->get(E_ICODE) == IPOPQ) 
            && (ereg->get(E_DSTM) == d_srcA || ereg->get(E_DSTM) == d_srcB))
            || (IRET == dreg->get(D_ICODE) || IRET == ereg->get(E_ICODE)
            || IRET == mreg->get(M_ICODE)));
}

bool FetchStage::stall_D(PipeReg * ereg) {
   return ((ereg->get(E_ICODE) == IMRMOVQ || ereg->get(E_ICODE) == IPOPQ) 
            && (ereg->get(E_DSTM) == d_srcA || ereg->get(E_DSTM) == d_srcB));
}

bool FetchStage::bubble_D(PipeReg * ereg, PipeReg * dreg, PipeReg * mreg) {
   return (((ereg->get(E_ICODE) == IJXX) && !e_Cnd)
    || (!((ereg->get(E_ICODE) == IMRMOVQ || ereg->get(E_ICODE) == IPOPQ) && (ereg->get(E_DSTM) == d_srcA || ereg->get(E_DSTM) == d_srcB)) 
    && ((IRET == dreg->get(D_ICODE) || IRET == ereg->get(E_ICODE) || IRET == mreg->get(M_ICODE)))));
}
     
