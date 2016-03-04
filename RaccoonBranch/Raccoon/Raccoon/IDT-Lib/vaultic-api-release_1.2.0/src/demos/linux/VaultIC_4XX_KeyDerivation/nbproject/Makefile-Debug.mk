#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1329223797/vaultic_4XX_key_derivation_sample.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-ldl

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/vaultic_4xx_keyderivation

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/vaultic_4xx_keyderivation: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/vaultic_4xx_keyderivation ${OBJECTFILES} ${LDLIBSOPTIONS} -ldl

${OBJECTDIR}/_ext/1329223797/vaultic_4XX_key_derivation_sample.o: ../../common/vaultic_4XX_key_derivation_sample.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1329223797
	${RM} $@.d
	$(COMPILE.cc) -g -D_DEBUG -D_LINUX -I../../../common -I../../../arch/pc/shared -I../../../device/vaultic_4XX_family -I/usr/include/PCSC -I/usr/local/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1329223797/vaultic_4XX_key_derivation_sample.o ../../common/vaultic_4XX_key_derivation_sample.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/vaultic_4xx_keyderivation

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
