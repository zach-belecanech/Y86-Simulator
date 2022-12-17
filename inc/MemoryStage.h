class MemoryStage: public Stage
{
   private:
      //TODO: provide declarations for new methods
      //set the input value for each fieldof the D pipeline register

      void setWInput(PipeReg * wreg, uint64_t stat, uint64_t icode,
                            uint64_t valE, uint64_t valM, uint64_t dstE, 
                            uint64_t dstM);
    
   public:
      //These are the only methods that are called outside of the class
      bool doClockLow(PipeReg ** pregs);  
      void doClockHigh(PipeReg ** pregs);
      int32_t Addr(PipeReg * mregs);
      bool mem_read(PipeReg * mregs);
      bool mem_write(PipeReg * mregs, uint64_t icode);
      uint64_t setM_Stat(bool err, PipeReg * mreg);
};