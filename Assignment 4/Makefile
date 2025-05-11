CC = gcc
CFLAGS = -Wall -g
LDFLAGS =  # No -lfl

all: json2relcsv

json2relcsv: scanner.o parser.o ast.o schema.o main.o
	@echo "Linking..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

parser.tab.h parser.c: parser.y
	@echo "Generating parser.c and parser.tab.h..."
	bison --header=parser.tab.h parser.y -o parser.c

scanner.c: scanner.l parser.tab.h
	@echo "Generating scanner.c..."
	flex -o scanner.c scanner.l

scanner.o: scanner.c parser.tab.h
	@echo "Compiling scanner.c..."
	$(CC) $(CFLAGS) -c scanner.c

parser.o: parser.c ast.h
	@echo "Compiling parser.c..."
	$(CC) $(CFLAGS) -c parser.c

ast.o: ast.c ast.h
	@echo "Compiling ast.c..."
	$(CC) $(CFLAGS) -c ast.c

schema.o: schema.c schema.h ast.h
	@echo "Compiling schema.c..."
	$(CC) $(CFLAGS) -c schema.c

main.o: main.c ast.h schema.h
	@echo "Compiling main.c..."
	$(CC) $(CFLAGS) -c main.c

clean:
	@echo "Cleaning up..."
	rm -f *.o scanner.c parser.c parser.tab.h json2relcsv json2relcsv.exe