CFLAGS= -Os

HEADERS= include/mcuserial.h include/MCUCommands.h

# Make file to create the MCU serial library
mcuserialTester: obj/mcuserial.o obj/mcuserialTester.o
	gcc $(CFLAGS) -I $(HELICONNECT) -I include/ $^ -o $@ 

obj/mcuserialTester.o: src/mcuserialTester.c $(HEADERS)
	gcc $(CFLAGS) -c -I include/ $< -o $@

obj/mcuserial.o: src/mcuserial.c $(HEADERS)
	gcc $(CFLAGS) -c -I include/ $< -o $@

.PHONY : clean

clean:
	rm -f obj/* mcuserialTester

