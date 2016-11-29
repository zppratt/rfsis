CC       = g++
INCLUDE  = /home/pi/picotcp/build/lib/libpicotcp.a
HEADERS  = /home/pi/picotcp/build/include
CFLAGS   = -Wall -g -I $(HEADERS)
LFLAGS   =
SRC      = src
BIN      = bin
FILES    = $(wildcard $(SRC)/*.cpp)
OBJS     = $(patsubst $(SRC)/%.cpp, $(BIN)/%.o, $(FILES))
EXE      = test

all : build $(EXE)

$(EXE): $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(INCLUDE) -o $@

$(BIN)/%.o: $(SRC)/%.cpp
	$(CC) $(CFLAGS) -I include/ -c $< -o $@

build:
	@mkdir -p bin

clean:
	rm $(EXE)
	rm -rf bin
