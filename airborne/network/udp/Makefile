# $Author$
# $Name:  $
# $Id$
# $Source: /home/luis/cvsroot/net-api/Makefile,v $
# $Log: Makefile,v $
# Revision 1.1.1.1  2008-05-05 07:07:57  luis
# initial import
#
# Revision 1.1.1.1  2008-05-05 06:49:35  luis
# initial import
#
# Revision 1.2  2006/01/18 15:47:45  cvs
# *** empty log message ***
#
# Revision 1.1.1.1  2005/11/06 10:56:35  cvs
# initial creation
#
# Revision 1.1.1.1  2005/10/22 16:44:21  cvs
# initial creation
#
# Revision 1.2  2005/10/18 16:30:20  cvs
# *** empty log message ***
#
# Revision 1.1.1.1  2005/10/15 15:14:06  cvs
# initial project comments
#
# Revision 1.1.1.1  2004/03/03 11:03:06  srik
# Initial Release 0.1
#
#

CC = gcc 
LD = $(CC)

CFLAGS = 
LDFLAGS =  -L./ -lstate -lm


SRCS = client.c server.c state.c udp.c primitive_serialisation.c
INCLUDES = server.h state.h udp.h commands.h 
OBJS = server.o udp.o state.o client.o primitive_serialisation.o
LIBSTATE= libstate.a 
all:  $(LIBSTATE) example

%.o: %.c
	$(CC) -O2 -c $< $(CFLAGS) -o $@

example: query_state1.o query_state2.o
	$(CC) query_state1.o $(LDFLAGS) -o client 
	$(CC) query_state2.o $(LDFLAGS) -o server 

$(LIBSTATE): $(OBJS)	
	ar -cr $(LIBSTATE) $(OBJS)
	ranlib $(LIBSTATE)

clean:
	rm -f *.o client server *~ *.a