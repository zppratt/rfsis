CC       = g++
INCLUDE  = lib/picotcp/build/lib/libpicotcp.a
HEADERS  = lib/picotcp/build/include
CFLAGS   = -Wall -std=c++11 -g -I $(HEADERS)
LFLAGS   =
SRC      = src
BIN      = bin
FILES    = $(wildcard $(SRC)/*.cpp)
OBJS     = $(patsubst $(SRC)/%.cpp, $(BIN)/%.o, $(FILES))
EXE      = echoserver

all : build $(EXE)

$(EXE): $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(INCLUDE) -lpthread -o $@

$(BIN)/%.o: $(SRC)/%.cpp
	$(CC) $(CFLAGS) -I include/ -c $< -o $@

build:
	@mkdir -p bin

clean:
	rm $(EXE)
	rm -rf bin
