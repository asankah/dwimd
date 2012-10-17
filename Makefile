O=obj

CC=cl /nologo
CFLAGS=$(CFLAGS) -DUNICODE -D_UNICODE
C2OBJ=$(CC) $(CFLAGS) /c /EHsc /Fo$@
LINK=link /nologo

EXEFILE=$(O)\dwimd.exe
OBJS=\
	$(O)\dwimd.obj	\
	$(O)\test.obj		\
	$(O)\scanner_test.obj	\
	$(O)\policy_test.obj	\
	$(O)\command_line.obj

all: mkdirs $(EXEFILE)

mkdirs:
!if !exist($(O))
	mkdir $(O)
!endif

$(O)\dwimd.obj: dwimd.cc dwimd.h
	$(C2OBJ) dwimd.cc

$(O)\test.obj: test.cc test.h
	$(C2OBJ) test.cc

$(O)\scanner_test.obj: scanner_test.cc test.h scanner_test.h
	$(C2OBJ) scanner_test.cc

$(O)\policy_test.obj: policy_test.cc test.h policy_test.h
	$(C2OBJ) policy_test.cc

$(O)\command_line.obj: command_line.cc command_line.h test_parameters.h
	$(C2OBJ) command_line.cc

$(EXEFILE): $(OBJS)
	$(LINK) /OUT:$@ $**

clean:
	del $(OBJS)
