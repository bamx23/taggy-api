CC=g++
CFLAGS=-c -Wall -O2
LDFLAGS=${FCGI_LIBS}
SOURCES=error.cpp template.cpp main.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=${FCGI_MAIN}

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
