CC = g++
CC_FLAGS = -std=c++11 -W -O3 -g

EXEC = Wunk8
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC)

%.o: %.cpp
	$(CC) -c $(CC_FLAGS) $< -o $@


clean:
	rm -f $(EXEC) $(OBJECTS)

run:
	@$(MAKE) && ./$(EXEC)
