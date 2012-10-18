# See LICENSE file

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

$(EXEFILE): $(OBJS)
	$(LINK) /OUT:$@ $**

clean:
	del $(OBJS)

.cc{$(O)}.obj:
	$(C2OBJ) $<
