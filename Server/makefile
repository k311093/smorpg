LIBOPS = -lpthread
CC = cc
COPS = 
OUTPUT = /result

server : server.c sendfile.c login.c safelist.c map.c character.c chat.c extern.c sendpacket.c broadcast.c logout.c userno.c
	$(CC) -o server server.c sendfile.c login.c safelist.c map.c character.c sendpacket.c extern.c chat.c broadcast.c logout.c userno.c $(LIBOPS)
clean :
	rm -f server
