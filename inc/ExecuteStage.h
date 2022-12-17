class ExecuteStage: public Stage
{
   private:
      //TODO: provide declarations for new methods
      bool M_bubble;
      //set the input value for each fieldof the D pipeline register
      void setMInput(PipeReg * mreg, uint64_t stat, uint64_t icode,
                            uint64_t cnd, uint64_t valE, uint64_t valA,
                            uint64_t dstE, uint64_t dstM);
      
   public:
      //These are the only methods that are called outside of the class
      bool doClockLow(PipeReg ** pregs);  
      void doClockHigh(PipeReg ** pregs);
      uint64_t aluA(PipeReg * eregs);
      uint64_t aluB(PipeReg * eregs);
      uint64_t aluFun(PipeReg * eregs);
      bool set_cc(PipeReg * eregs, PipeReg * wreg);
      uint64_t setDstE(PipeReg * eregs);
      uint64_t bigALU(PipeReg * ereg, PipeReg * wreg, uint64_t aluA, uint64_t aluB, uint64_t aluFun);
      void bigCC(uint64_t value, bool overFlow);
      bool setCnd(uint64_t icode, uint64_t ifun);
      bool calculateControlSignals(PipeReg * wreg);
};