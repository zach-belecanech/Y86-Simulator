class WritebackStage: public Stage
{
   private:
      //TODO: provide declarations for new methods

      //set the input value for each fieldof the D pipeline register
   public:
      //These are the only methods that are called outside of the class
      bool doClockLow(PipeReg ** pregs);  
      void doClockHigh(PipeReg ** pregs);
};