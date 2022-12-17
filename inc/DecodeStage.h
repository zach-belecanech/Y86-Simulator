class DecodeStage: public Stage
{
   private:
      //TODO: provide declarations for new methods
      bool E_bubble; 
      //set the input value for each fieldof the D pipeline register
      void setEInput(PipeReg * ereg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun, uint64_t valC, uint64_t valA,
                           uint64_t valB, uint64_t dste, uint64_t dstm, uint64_t srcA, uint64_t srcB);
      uint64_t setSrcA(PipeReg * dreg);
      uint64_t setSrcB(PipeReg * dreg);
      uint64_t setDstE(PipeReg * dreg);
      uint64_t setDstM(PipeReg * dreg);
      uint64_t Sel_FwdA(uint64_t srcA);
      uint64_t FwdB(uint64_t srcB);
      uint64_t setDvalA(PipeReg * dreg, PipeReg * mreg, PipeReg * ereg, PipeReg * wreg, uint64_t srcA);
      uint64_t setDvalB(PipeReg * dreg, PipeReg * mreg, PipeReg * ereg, PipeReg * wreg, uint64_t srcB);
      bool calculateControlSignals(PipeReg * ereg);
   public:
      //These are the only methods that are called outside of the class
      bool doClockLow(PipeReg ** pregs);  
      void doClockHigh(PipeReg ** pregs);
};