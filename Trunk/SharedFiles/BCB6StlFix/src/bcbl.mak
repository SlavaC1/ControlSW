#
# compiler
#
CC = bcc
CXX = $(CC)

#
# Basename for libraries
#
LIB_BASENAME = stlport_bcc

#
# guts for common stuff
#
#
LINK=ar crv
DYN_LINK=ar crv

OBJEXT=o
DYNEXT=so
STEXT=a
RM=rm -rf
PATH_SEP=/
MKDIR=mkdir -p

all: all_dynamic all_static

include common_macros.mak

#WARNING_FLAGS= -w -w-par -w-inl
WARNING_FLAGS= -w- -Q

XTRA_FLAGS := $(XTRA_FLAGS)

RELEASE_OBJECTS_static := $(RELEASE_OBJECTS_static)

# jww (2000-06-05): these shouldn't be needed when building with bcc,
# against the Borland RTL built with bcc

# -I/usr/local/include \
# -I/usr/include \
# -I/usr/i386-redhat-linux/include \
# -I/usr/lib/gcc-lib/i386-redhat-linux/egcs-2.91.66/include \
# -I/usr/include/g++-2 \

CXXFLAGS_COMMON = \
	-I${STLPORT_DIR} \
	-I../../../include \
	-D__linux__ \
	-u- -f- -x- -a4 -d -jb -j1 ${WARNING_FLAGS} ${XTRA_FLAGS}

CXXFLAGS_RELEASE_static = $(CXXFLAGS_COMMON) -O2
CXXFLAGS_RELEASE_dynamic = $(CXXFLAGS_COMMON) -O2 -fpic

CXXFLAGS_DEBUG_static = $(CXXFLAGS_COMMON) -v
CXXFLAGS_DEBUG_dynamic = $(CXXFLAGS_COMMON) -v -fpic

CXXFLAGS_STLDEBUG_static = $(CXXFLAGS_DEBUG_static) -D__STL_DEBUG
CXXFLAGS_STLDEBUG_dynamic = $(CXXFLAGS_DEBUG_dynamic) -D__STL_DEBUG -fpic

.SUFFIXES: .cpp .c .o .so .a

$(RELEASE_OBJDIR_static)/%.o: %.cpp
	$(CXX) $(CXXFLAGS_RELEASE_static) -n$(RELEASE_OBJDIR_static) -c $<
$(RELEASE_OBJDIR_static)/%.o: %.c
	$(CC) $(CXXFLAGS_RELEASE_static) -n$(RELEASE_OBJDIR_static) -c $<
$(RELEASE_OBJDIR_static)/%.i : %.cpp
	$(CXX) $(CXXFLAGS_RELEASE_static) $< -E  $@

$(RELEASE_OBJDIR_dynamic)/%.o : %.cpp
	$(CXX) $(CXXFLAGS_RELEASE_dynamic) -n$(RELEASE_OBJDIR_dynamic) -c $<
$(RELEASE_OBJDIR_dynamic)/%.o : %.c
	$(CC) $(CXXFLAGS_RELEASE_dynamic) -n$(RELEASE_OBJDIR_dynamic) -c $<
$(RELEASE_OBJDIR_dynamic)/%.i : %.cpp
	$(CXX) $(CXXFLAGS_RELEASE_dynamic) $< -E  $@

$(DEBUG_OBJDIR_static)/%.o : %.cpp
	$(CXX) $(CXXFLAGS_DEBUG_static) -n$(DEBUG_OBJDIR_static) -c $<
$(DEBUG_OBJDIR_static)/%.o : %.c
	$(CC) $(CXXFLAGS_DEBUG_static) -n$(DEBUG_OBJDIR_static) -c $<
$(DEBUG_OBJDIR_static)/%.i : %.cpp
	$(CXX) $(CXXFLAGS_DEBUG_static) $< -E  $@

$(DEBUG_OBJDIR_dynamic)/%.o : %.cpp
	$(CXX) $(CXXFLAGS_DEBUG_dynamic) -n$(DEBUG_OBJDIR_dynamic) -c $<
$(DEBUG_OBJDIR_dynamic)/%.o : %.c
	$(CC) $(CXXFLAGS_DEBUG_dynamic) -n$(DEBUG_OBJDIR_dynamic) -c $<
$(DEBUG_OBJDIR_dynamic)/%.i : %.cpp
	$(CXX) $(CXXFLAGS_DEBUG_dynamic) $< -E  $@

$(STLDEBUG_OBJDIR_static)/%.o : %.cpp
	$(CXX) $(CXXFLAGS_STLDEBUG_static) -n$(STLDEBUG_OBJDIR_static) -c $<
$(STLDEBUG_OBJDIR_static)/%.o : %.c
	$(CC) $(CXXFLAGS_STLDEBUG_static) -n$(STLDEBUG_OBJDIR_static) -c $<
$(STLDEBUG_OBJDIR_static)/%.i : %.cpp
	$(CXX) $(CXXFLAGS_STLDEBUG_static) $< -E  $@

$(STLDEBUG_OBJDIR_dynamic)/%.o : %.cpp
	$(CXX) $(CXXFLAGS_STLDEBUG_dynamic) -n$(STLDEBUG_OBJDIR_dynamic) -c $<
$(STLDEBUG_OBJDIR_dynamic)/%.o : %.c
	$(CC) $(CXXFLAGS_STLDEBUG_dynamic) -n$(STLDEBUG_OBJDIR_dynamic) -c $<
$(STLDEBUG_OBJDIR_dynamic)/%.i : %.cpp
	$(CXX) $(CXXFLAGS_STLDEBUG_dynamic) $< -E  $@

include common_rules.mak


#install: all
#	cp -p $(LIB_TARGET) ${D_LIB_TARGET} ../lib

#%.s: %.cpp
#	$(CXX) $(CXXFLAGS) -O4 -S -pto $<  -o $@


