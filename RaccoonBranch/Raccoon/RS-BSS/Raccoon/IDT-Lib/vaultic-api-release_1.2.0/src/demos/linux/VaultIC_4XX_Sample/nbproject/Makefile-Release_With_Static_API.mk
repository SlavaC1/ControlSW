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
CND_CONF=Release_With_Static_API
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1329223797/vaultic_4XX_sample_code.o


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
LDLIBSOPTIONS=-lpthread -ldl ../../../build/linux/VaultIC_API_4XX/dist/GNU-Linux-x86/Release_Static/libvaultic_api_4xx.a -lpcsclite

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/vaultic_4xx_sample

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/vaultic_4xx_sample: ../../../build/linux/VaultIC_API_4XX/dist/GNU-Linux-x86/Release_Static/libvaultic_api_4xx.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/vaultic_4xx_sample: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/vaultic_4xx_sample ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/1329223797/vaultic_4XX_sample_code.o: ../../common/vaultic_4XX_sample_code.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1329223797
	${RM} $@.d
	$(COMPILE.cc) -O2 -DUSE_STATIC_API -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -I/usr/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1329223797/vaultic_4XX_sample_code.o ../../common/vaultic_4XX_sample_code.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/vaultic_4xx_sample

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
