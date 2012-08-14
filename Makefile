# C Compiler
CC=clang++
#CC=g++
# Universal C Flags
CFLAGS+=-c -g
# Platform-Independent Libraries
LDFLAGS+= -lOIS -lboost_thread -lboost_filesystem -lboost_system -lboost_date_time -lzzip
#Platform-Specific Libraries and other Unknowns
LDFLAGS+= -lGL -lGLU -lpthread -ldl -lX11 -lXt -lfreetype -lXext -lSM -lICE -lXaw -lXrandr -lfreeimage -lCg
# OGRE Addons
LDFLAGS+= -L../lib/ -lOgreRTShaderSystemStatic -lRenderSystem_GLStatic -lPlugin_CgProgramManagerStatic -lOgreMainStatic
# Special Directories
OGREHOME=/usr/share/include/OGRE
OISHOME=/usr/share/include/OIS
INCLUDEFLAGS= -I$(OGREHOME) -I$(OISHOME) -DOGRE_STATIC_LIB
# Source Files
SOURCES=application.cpp client.cpp applicationDefaults.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
# Name of the output executable
NAME=application/OgreApp

OGRE = $(INCLUDEFLAGS) $(LDFLAGS)

all: $(SOURCES) $(NAME)

$(NAME): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDEFLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS)
