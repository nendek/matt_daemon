all:
	make -f ./Makefile.Ben_AFK
	make -f ./Makefile.Matt_daemon

Ben_AFK:
	make -f ./Makefile.Ben_AFK

Matt_daemon:
	make -f ./Makefile.Matt_daemon

clean:
	make clean -f ./Makefile.Ben_AFK
	make clean -f ./Makefile.Matt_daemon

fclean:
	make fclean -f ./Makefile.Ben_AFK
	make fclean -f ./Makefile.Matt_daemon

re: clean all
