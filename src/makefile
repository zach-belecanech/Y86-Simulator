#makefile to compile the .C files in this directory
#and write the .o output to the ../obj directory
CC = g++
INC = ../inc
BIN = ../bin
OBJD = ../obj
CFLAGS = -g -c -Wall -std=c++11 -I $(INC)
OBJ = $(OBJD)/ConditionCodes.o $(OBJD)/Memory.o \
$(OBJD)/RegisterFile.o $(OBJD)/Tools.o $(OBJD)/Loader.o \
$(OBJD)/String.o $(OBJD)/yess.o $(OBJD)/CPU.o $(OBJD)/D.o \
$(OBJD)/DecodeStage.o $(OBJD)/E.o $(OBJD)/ExecuteStage.o $(OBJD)/F.o \
$(OBJD)/FetchStage.o $(OBJD)/M.o $(OBJD)/MemoryStage.o $(OBJD)/PipeReg.o \
$(OBJD)/PipeRegField.o $(OBJD)/Stage.o $(OBJD)/W.o $(OBJD)/WritebackStage.o


#1) need a macro for the .o files corresponding to the .C 
#   files in this directory (note they will be stored in the ../obj
#   directory
$(OBJD)/%.o: %.C create$(OBJD)
	$(CC) $(CFLAGS) $< -o $(OBJD)/$@

$(BIN)/yess: $(OBJ) 
	$(CC) $(OBJ) -o $(BIN)/yess
#2) need a target (with a name that doesn't match a file)
#   that is dependent upon the .o files 
#3) need a .o target for each .C
run:
	make $(BIN)/yess
	$(BIN)/run.sh
$(OBJD)/ConditionCodes.o: $(INC)/ConditionCodes.h $(INC)/Tools.h 

$(OBJD)/Memory.o: $(INC)/Memory.h $(INC)/Tools.h

$(OBJD)/RegisterFile.o: $(INC)/RegisterFile.h $(INC)/Tools.h

$(OBJD)/Tools.o: $(INC)/Tools.h

$(OBJD)/Loader.o: $(INC)/Debug.h $(INC)/Memory.h $(INC)/String.h $(INC)/Loader.h

$(OBJD)/String.o: $(INC)/Debug.h $(INC)/String.h

$(OBJD)/yess.o: $(INC)/Debug.h $(INC)/Memory.h $(INC)/String.h $(INC)/Loader.h $(INC)/RegisterFile.h $(INC)/ConditionCodes.h $(INC)/PipeRegField.h \
$(INC)/PipeReg.h $(INC)/Stage.h $(INC)/CPU.h

$(OBJD)/CPU.o: $(INC)/Memory.h $(INC)/RegisterFile.h $(INC)/ConditionCodes.h $(INC)/PipeRegField.h \
$(INC)/PipeReg.h $(INC)/F.h $(INC)/D.h $(INC)/E.h $(INC)/M.h $(INC)/W.h $(INC)/Stage.h \
$(INC)/MemoryStage.h $(INC)/ExecuteStage.h $(INC)/DecodeStage.h $(INC)/FetchStage.h \
$(INC)/WritebackStage.h $(INC)/CPU.h

$(OBJD)/D.o: $(INC)/Instructions.h $(INC)/RegisterFile.h $(INC)/PipeRegField.h $(INC)/PipeReg.h $(INC)/D.h $(INC)/Status.h

$(OBJD)/DecodeStage.o: $(INC)/Memory.h $(INC)/RegisterFile.h $(INC)/ConditionCodes.h $(INC)/Instructions.h $(INC)/PipeRegField.h $(INC)/PipeReg.h $(INC)/Stage.h $(INC)/D.h $(INC)/E.h $(INC)/M.h $(INC)/W.h $(INC)/DecodeStage.h

$(OBJD)/E.o: $(INC)/Instructions.h $(INC)/RegisterFile.h $(INC)/PipeRegField.h $(INC)/PipeReg.h $(INC)/E.h $(INC)/Status.h

$(OBJD)/ExecuteStage.o: $(INC)/Memory.h $(INC)/RegisterFile.h $(INC)/ConditionCodes.h $(INC)/Instructions.h $(INC)/PipeRegField.h $(INC)/PipeReg.h $(INC)/Stage.h $(INC)/E.h $(INC)/ExecuteStage.h $(INC)/W.h $(INC)/Status.h $(INC)/Tools.h

$(OBJD)/F.o: $(INC)/PipeRegField.h $(INC)/PipeReg.h $(INC)/F.h

$(OBJD)/FetchStage.o: $(INC)/Memory.h $(INC)/ConditionCodes.h $(INC)/Instructions.h $(INC)/RegisterFile.h $(INC)/PipeRegField.h $(INC)/PipeReg.h $(INC)/W.h $(INC)/M.h $(INC)/F.h $(INC)/D.h $(INC)/E.h $(INC)/Stage.h $(INC)/FetchStage.h $(INC)/Status.h $(INC)/Debug.h $(INC)/Tools.h

$(OBJD)/M.o: $(INC)/RegisterFile.h $(INC)/Instructions.h $(INC)/PipeRegField.h $(INC)/PipeReg.h $(INC)/M.h $(INC)/Status.h

$(OBJD)/MemoryStage.o: $(INC)/Memory.h $(INC)/RegisterFile.h $(INC)/ConditionCodes.h $(INC)/Instructions.h $(INC)/PipeRegField.h $(INC)/PipeReg.h $(INC)/Stage.h $(INC)/M.h $(INC)/MemoryStage.h $(INC)/Status.h

$(OBJD)/PipeReg.o: $(INC)/PipeRegField.h $(INC)/PipeReg.h

$(OBJD)/PipeRegField.o: $(INC)/PipeRegField.h

$(OBJD)/Stage.o: $(INC)/PipeRegField.h $(INC)/PipeReg.h $(INC)/Memory.h $(INC)/RegisterFile.h $(INC)/ConditionCodes.h $(INC)/Stage.h

$(OBJD)/W.o: $(INC)/RegisterFile.h $(INC)/Instructions.h $(INC)/PipeRegField.h $(INC)/PipeReg.h $(INC)/W.h $(INC)/Status.h

$(OBJD)/WritebackStage.o: $(INC)/Memory.h $(INC)/Instructions.h $(INC)/RegisterFile.h $(INC)/ConditionCodes.h $(INC)/PipeRegField.h $(INC)/PipeReg.h $(INC)/Stage.h $(INC)/W.h $(INC)/WritebackStage.h $(INC)/Status.h
#4) need a clean target to delete the .o files in the ../obj directory
clean:
	rm $(OBJD)
#This rule creates the ../obj directory if it doesn't exist
#This needs to be at the bottom of the makefile
create$(OBJD):
	mkdir -p $(OBJD)
