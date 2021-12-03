server: server.c
	@clang server.c -o server.out -lm
	@clang client.c -o client.out -lm

debug: client.c
	@clang client.c -o client.out -lm -D DEBUG_MODE
	@clang server.c -o server.out -lm -D DEBUG_MODE

clean:
	@rm -f server.out client.out



