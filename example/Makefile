#Compiler and Linker
CC          := g++
AR          := ar

EXE_1	  := PipeTest
OBJ_1    := obj/PipeTest.o

EXE_2   := Fifo1
OBJ_2   := obj/Fifo1.o

EXE_3   := Fifo2
OBJ_3   := obj/Fifo2.o

EXE_4   := FifoServer
OBJ_4   := obj/FifoServer.o

EXE_5   := FifoClient
OBJ_5   := obj/FifoClient.o

EXE_6   := BiPipeTest
OBJ_6   := obj/BiPipeTest.o

TARGET  :=   \
  $(EXE_1)   \
	$(EXE_2)   \
	$(EXE_3)   \
	$(EXE_4)   \
	$(EXE_5)   \
	$(EXE_6)   \


#The Directories, Source, Includes, Objects, Binary and Resources
SRCDIR      := src
LIB_INC     := ../lib/include 
INCDIR      := ../include 
BUILDDIR    := obj
TARGETDIR   := ../bin
SRCEXT      := cpp
DEPEXT      := d
OBJEXT      := o
MY_LIB      := Pipes

#Flags, Libraries and Includes
CFLAGS      := -Wall -O3 -g -std=c++11
LIB         := -lm -lpthread -L./bin -L../bin -l$(MY_LIB)
INC         := -I$(INCDIR) -I$(LIB_INC) -I/usr/local/include 
INCDEP      := -I$(INCDIR) -I$(LIB_INC)

#---------------------------------------------------------------------------------
#DO NOT EDIT BELOW THIS LINE EXCEPT FOR ADDING TARGETS
#---------------------------------------------------------------------------------
SOURCES     := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

#Defauilt Make
all: resources $(TARGET)

#Remake
remake: cleaner all

#Copy Resources from Resources Directory to Target Directory
resources: directories
#	@cp $(RESDIR)/* $(TARGETDIR)/

#Make the Directories
directories:
	mkdir -p $(TARGETDIR)
	mkdir -p $(BUILDDIR)

#Clean only Objecst
clean:
	@$(RM) -rf $(BUILDDIR)

#Full Clean, Objects and Binaries
cleaner: clean
	@$(RM) -rf $(TARGETDIR)

#Pull in dependency info for *existing* .o files
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

################# ADD MORE TARGETS HERE #############################
#Link
$(EXE_1): $(OBJECTS)
	$(CC) -o $(TARGETDIR)/$(EXE_1) $(OBJ_1) $(LIB)

$(EXE_2): $(OBJECTS)
	$(CC) -o $(TARGETDIR)/$(EXE_2) $(OBJ_2) $(LIB)

$(EXE_3): $(OBJECTS)
	$(CC) -o $(TARGETDIR)/$(EXE_3) $(OBJ_3) $(LIB)

$(EXE_4): $(OBJECTS)
	$(CC) -o $(TARGETDIR)/$(EXE_4) $(OBJ_4) $(LIB)

$(EXE_5): $(OBJECTS)
	$(CC) -o $(TARGETDIR)/$(EXE_5) $(OBJ_5) $(LIB)

$(EXE_6): $(OBJECTS)
	$(CC) -o $(TARGETDIR)/$(EXE_6) $(OBJ_6) $(LIB)

#####################################################################
#Compile
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -I../lib/include -c -o $@ $<
	@$(CC) $(CFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

#Non-File Targets
.PHONY: all remake clean cleaner resources