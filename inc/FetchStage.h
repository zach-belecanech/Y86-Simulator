//class to represent the Fetch stage

class FetchStage: public Stage
{
   private:
      //TODO: provide declarations for new methods
      bool F_stall;
      bool D_stall;
      bool D_Bubble;
      //set the input value for each fieldof the D pipeline register
      void setDInput(PipeReg * dreg, uint64_t stat, uint64_t icode, 
                     uint64_t ifun, uint64_t rA, uint64_t rB,
                     uint64_t valC, uint64_t valP);
      uint64_t selectPC(PipeReg * freg, PipeReg * mreg, PipeReg * wreg);
      bool needRegIds(uint64_t icode);
      bool needValC(uint64_t icode);
      uint64_t predictPC(uint64_t f_icode, uint64_t f_valC, uint64_t f_valP);
      uint64_t PC_Increment(uint64_t f_pc, bool needRegId, bool needValC);
      void getRegIds(uint64_t instruc, bool needRegId, uint64_t * rA, uint64_t * rB);
      uint64_t buildValC(uint64_t instruc, bool needregid, bool needValC);
      bool instr_valid(uint64_t f_icode);
      uint64_t f_stat(bool mem_error, bool instr_valid, uint64_t f_icode, PipeReg * freg);
      void calculateControlSignals(PipeReg * ereg, PipeReg * dreg, PipeReg * mreg);
      bool stall_F(PipeReg * ereg, PipeReg * dreg, PipeReg * mreg);
      bool stall_D(PipeReg * ereg);
      bool bubble_D(PipeReg * ereg, PipeReg * dreg, PipeReg * mreg);
   public:
      //These are the only methods that are called outside of the class
      bool doClockLow(PipeReg ** pregs);  
      void doClockHigh(PipeReg ** pregs);
      
};
