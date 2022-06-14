all: winkey

winkey.obj:
	cl.exe /WX /Wall ./sources/winkey.cpp

winkey: winkey.obj
	link.exe ./winkey.obj /OUT:winkey.exe

clean:
	del winkey.obj

fclean: clean
	del winkey.exe

re: fclean all
