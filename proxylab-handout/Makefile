################################################################################
# Fill in your student ID 
STUDENT_ID=0000-00000

################################################################################
# DON'T MODIFY 
REPORT=report.pdf
MAINFILE=proxy.c

CC=gcc -m32
CFLAGS=-O2 -Wall
LIBS=-lpthread

FILES = Makefile csapp.h csapp.c echoclient.c echoservert.c echo.c proxy.c

PROGS = echoclient echoserver proxy

all: $(PROGS)

echoclient: echoclient.c csapp.o csapp.h
	$(CC) $(CFLAGS) $(LIBS) -o echoclient echoclient.c csapp.o

echoserver: echoservert.c csapp.o csapp.h echo.o echo.h
	$(CC) $(CFLAGS) $(LIBS) -o echoserver echoservert.c csapp.o echo.o

proxy: proxy.c csapp.o csapp.h
	$(CC) $(CFLAGS) $(LIBS) -o proxy proxy.c csapp.o

csapp.o: csapp.c csapp.h
	$(CC) $(CFLAGS) -c csapp.c

echo.o: echo.c csapp.h
	$(CC) $(CFLAGS) -c echo.c

handin:
	mkdir $(STUDENT_ID)
	cp $(REPORT) $(STUDENT_ID)/$(STUDENT_ID)_$(REPORT)
	cp $(MAINFILE) $(STUDENT_ID)/$(STUDENT_ID)_$(MAINFILE)
	tar czvf $(STUDENT_ID).tgz $(STUDENT_ID) 
	rm -rf $(STUDENT_ID)

clean:
	rm -f *.o *~ *.exe *.tar
	rm -f $(PROGS)
