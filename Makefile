NAME = fevlist

ifndef CPU
    $(error Specify CPU=[x86|x86_64|arm|armhf])
endif

ifndef FMOD_STUDIO_PATH
    $(error Specify FMOD_STUDIO_PATH)
endif
    
SOURCE_FILES = \
    ../list_events.cpp \

INCLUDE_DIRS = \
    -I${FMOD_STUDIO_PATH}/api/lowlevel/inc \
    -I${FMOD_STUDIO_PATH}/api/studio/inc

ifdef DEBUG
    SUFFIX = L
    CXXFLAGS += -DDEBUG -g
else
    SUFFIX = 
endif

LOWLEVEL_LIB = ${FMOD_STUDIO_PATH}/api/lowlevel/lib/${CPU}/libfmod${SUFFIX}.so
STUDIO_LIB = ${FMOD_STUDIO_PATH}/api/studio/lib/${CPU}/libfmodstudio${SUFFIX}.so

all:
	${CXX} ${CXXFLAGS} -pthread -o ${NAME} fevlist.cpp -Wl,-rpath=\$$ORIGIN/$(dir ${LOWLEVEL_LIB}),-rpath=\$$ORIGIN/$(dir ${STUDIO_LIB}) ${LOWLEVEL_LIB} ${STUDIO_LIB} ${INCLUDE_DIRS}
