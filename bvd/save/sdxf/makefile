# make SDXF
# we are in sdxf/objs

projdir=./
flags=-D_SYS_UNIX +z # -fPIC

cc=cc

objs1=SDXF.o transtab.o SDXtools.o float.o UTF8.o
objs2=AES.o RC4.o rijndael.o SHA1.o runleng.o zip.o                   # zlibdummy.o

objs3=adler32.o compress.o deflate.o infblock.o infcodes.o inffast.o
objs4=inflate.o inftrees.o infutil.o trees.o uncompr.o zutil.o

objs5=tools.o parser.o lists.o

sdxsrcdir=$(projdir)SDXF/src/
sdxincdir=$(projdir)SDXF/include/
btsrcdir=$(projdir)BTools/
zlibdir=$(projdir)zlib/

all:
	gmake -C objs -f ../makefile SDXF.sl

SDXF.sl: $(objs1) $(objs2) $(objs5)
	ld  -b -lpthread   $(objs1) $(objs2) $(objs5) /lib/libc.sl       # /lib/crt0.o
#	cp  SDXF.sl /usr/lib/

zip.o: $(objs3) $(objs4)
	ld  -r -o zip.o $(objs3) $(objs4)


%.o: $(sdxincdir)*.h $(btsrcdir)*.h
	$(cc) -c -I$(sdxincdir) -I$(btsrcdir) $(flags) -o $@ $<

%.o: $(zlibdir)%.c $(zlibdir)*.h
	$(cc) -c -DSTDC $(flags) -o $@ $<

%.o: $(btsrcdir)%.c $(btsrcdir)*.h
	$(cc) -c -I$(btsrcdir) $(flags) -o $@ $<


clean:
	rm objs/*.o
	gmake -C objs -f ../makefile SDXF.sl



#
#    $@ is the name of the file to be made.
#    $? is the names of the changed dependents.
#    $< the name of the related file that caused the action.
#    $* the prefix shared by target and dependent files.
#
#    $@  -make setzt die momentan zu generierende Zieldatei einschließlich der
#         Dateikennung (Suffix) aus der aktuellen Abhängigkeitsbeschreibung ein.
#    $*  -make setzt die momentan zu generierende Zieldatei aus der aktuellen
#         Abhängigkeitsbeschreibung simul ohne Dateikennung (Typerweiterung) ein.
#    $?  -make setzt aus der aktuellen Abhängigkeitsbeschreibung immer die
#         Objektdateien der rechten Seite ein, die neuer als das Ziel sind.
#    $<  -Enthält die Namen von neueren Dateien entspr. der Liste der Abhängigkeiten
#
