CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lfl

all: json2relcsv

json2relcsv: scanner.o parser.o ast.o schema.o main.o
	@echo "Linking..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Generate parser.c and parser.h
parser.h parser.c: parser.y
	@echo "Generating parser.c and parser.h..."
	bison -d parser.y -o parser.c

# Generate scanner.c, depending on parser.h
scanner.c: scanner.l parser.h
	@echo "Generating scanner.c..."
	flex -o scanner.c scanner.l

# Compile scanner.c, depending on scanner.c and parser.h
scanner.o: scanner.c parser.h
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
	rm -f *.o scanner.c parser.c parser.h json2relcsv