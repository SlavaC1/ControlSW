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
CND_CONF=Debug_Static
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1486021609/aardvark.o \
	${OBJECTDIR}/_ext/1486021609/aardvark_peripheral.o \
	${OBJECTDIR}/_ext/1486021609/vaultic_iso7816_protocol.o \
	${OBJECTDIR}/_ext/1486021609/vaultic_mem.o \
	${OBJECTDIR}/_ext/1486021609/vaultic_timer_delay.o \
	${OBJECTDIR}/_ext/1486021609/vaultic_twi_peripheral.o \
	${OBJECTDIR}/_ext/907750266/vaultic_block_protocol.o \
	${OBJECTDIR}/_ext/907750266/vaultic_command.o \
	${OBJECTDIR}/_ext/907750266/vaultic_comms.o \
	${OBJECTDIR}/_ext/907750266/vaultic_crc16.o \
	${OBJECTDIR}/_ext/907750266/vaultic_protocol.o \
	${OBJECTDIR}/_ext/907750266/vaultic_utils.o \
	${OBJECTDIR}/_ext/1023103441/vaultic_api.o \
	${OBJECTDIR}/_ext/1023103441/vaultic_bigdigits.o \
	${OBJECTDIR}/_ext/1023103441/vaultic_ecdsa_key_aux.o \
	${OBJECTDIR}/_ext/1023103441/vaultic_ecdsa_signer.o \
	${OBJECTDIR}/_ext/1023103441/vaultic_ecsda_strong_authentication.o \
	${OBJECTDIR}/_ext/1023103441/vaultic_elliptic-ff2n.o \
	${OBJECTDIR}/_ext/1023103441/vaultic_ff2n.o \
	${OBJECTDIR}/_ext/1023103441/vaultic_id_key_aux.o \
	${OBJECTDIR}/_ext/1023103441/vaultic_key_aux.o \
	${OBJECTDIR}/_ext/1023103441/vaultic_lib.o \
	${OBJECTDIR}/_ext/1023103441/vaultic_raw_key_aux.o \
	${OBJECTDIR}/_ext/1023103441/vaultic_sha256.o


# C Compiler Flags
CFLAGS=-L/usr/local/lib/ -lpcsclite

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_PLATFORM}/${CND_CONF}/libvaultic_api_1xx.a

${CND_DISTDIR}/${CND_PLATFORM}/${CND_CONF}/libvaultic_api_1xx.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_PLATFORM}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_PLATFORM}/${CND_CONF}/libvaultic_api_1xx.a
	${AR} -rv ${CND_DISTDIR}/${CND_PLATFORM}/${CND_CONF}/libvaultic_api_1xx.a ${OBJECTFILES} 

${OBJECTDIR}/_ext/1486021609/aardvark.o: nbproject/Makefile-${CND_CONF}.mk ../../../arch/pc/shared/aardvark.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1486021609
	${RM} $@.d
	$(COMPILE.c) -g -Wall -D_DEBUG -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_1XX_family -I/usr/local/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1486021609/aardvark.o ../../../arch/pc/shared/aardvark.c

${OBJECTDIR}/_ext/1486021609/aardvark_peripheral.o: nbproject/Makefile-${CND_CONF}.mk ../../../arch/pc/shared/aardvark_peripheral.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1486021609
	${RM} $@.d
	$(COMPILE.c) -g -Wall -D_DEBUG -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_1XX_family -I/usr/local/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1486021609/aardvark_peripheral.o ../../../arch/pc/shared/aardvark_peripheral.c

${OBJECTDIR}/_ext/1486021609/vaultic_iso7816_protocol.o: nbproject/Makefile-${CND_CONF}.mk ../../../arch/pc/shared/vaultic_iso7816_protocol.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1486021609
	${RM} $@.d
	$(COMPILE.c) -g -Wall -D_DEBUG -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_1XX_family -I/usr/local/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1486021609/vaultic_iso7816_protocol.o ../../../arch/pc/shared/vaultic_iso7816_protocol.c

${OBJECTDIR}/_ext/1486021609/vaultic_mem.o: nbproject/Makefile-${CND_CONF}.mk ../../../arch/pc/shared/vaultic_mem.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1486021609
	${RM} $@.d
	$(COMPILE.c) -g -Wall -D_DEBUG -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_1XX_family -I/usr/local/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1486021609/vaultic_mem.o ../../../arch/pc/shared/vaultic_mem.c

${OBJECTDIR}/_ext/1486021609/vaultic_timer_delay.o: nbproject/Makefile-${CND_CONF}.mk ../../../arch/pc/shared/vaultic_timer_delay.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1486021609
	${RM} $@.d
	$(COMPILE.c) -g -Wall -D_DEBUG -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_1XX_family -I/usr/local/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1486021609/vaultic_timer_delay.o ../../../arch/pc/shared/vaultic_timer_delay.c

${OBJECTDIR}/_ext/1486021609/vaultic_twi_peripheral.o: nbproject/Makefile-${CND_CONF}.mk ../../../arch/pc/shared/vaultic_twi_peripheral.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1486021609
	${RM} $@.d
	$(COMPILE.c) -g -Wall -D_DEBUG -D_LINUX -I../../../../../../IDT-HW/IDT-HW-Inf -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_1XX_family -I/usr/local/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1486021609/vaultic_twi_peripheral.o ../../../arch/pc/shared/vaultic_twi_peripheral.c

${OBJECTDIR}/_ext/907750266/vaultic_block_protocol.o: nbproject/Makefile-${CND_CONF}.mk ../../../common/vaultic_block_protocol.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -g -Wall -D_DEBUG -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_1XX_family -I/usr/local/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_block_protocol.o ../../../common/vaultic_block_protocol.c

${OBJECTDIR}/_ext/907750266/vaultic_command.o: nbproject/Makefile-${CND_CONF}.mk ../../../common/vaultic_command.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -g -Wall -D_DEBUG -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_1XX_family -I/usr/local/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_command.o ../../../common/vaultic_command.c

${OBJECTDIR}/_ext/907750266/vaultic_comms.o: nbproject/Makefile-${CND_CONF}.mk ../../../common/vaultic_comms.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -g -Wall -D_DEBUG -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_1XX_family -I/usr/local/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_comms.o ../../../common/vaultic_comms.c

${OBJECTDIR}/_ext/907750266/vaultic_crc16.o: nbproject/Makefile-${CND_CONF}.mk ../../../common/vaultic_crc16.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -g -Wall -D_DEBUG -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_1XX_family -I/usr/local/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_crc16.o ../../../common/vaultic_crc16.c

${OBJECTDIR}/_ext/907750266/vaultic_protocol.o: nbproject/Makefile-${CND_CONF}.mk ../../../common/vaultic_protocol.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -g -Wall -D_DEBUG -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_1XX_family -I/usr/local/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_protocol.o ../../../common/vaultic_protocol.c

${OBJECTDIR}/_ext/907750266/vaultic_utils.o: nbproject/Makefile-${CND_CONF}.mk ../../../common/vaultic_utils.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -g -Wall -D_DEBUG -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_1XX_family -I/usr/local/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_utils.o ../../../common/vaultic_utils.c

${OBJECTDIR}/_ext/1023103441/vaultic_api.o: nbproject/Makefile-${CND_CONF}.mk ../../../device/vaultic_1XX_family/vaultic_api.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1023103441
	${RM} $@.d
	$(COMPILE.c) -g -Wall -D_DEBUG -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_1XX_family -I/usr/local/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1023103441/vaultic_api.o ../../../device/vaultic_1XX_family/vaultic_api.c

${OBJECTDIR}/_ext/1023103441/vaultic_bigdigits.o: nbproject/Makefile-${CND_CONF}.mk ../../../device/vaultic_1XX_family/vaultic_bigdigits.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1023103441
	${RM} $@.d
	$(COMPILE.c) -g -Wall -D_DEBUG -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_1XX_family -I/usr/local/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1023103441/vaultic_bigdigits.o ../../../device/vaultic_1XX_family/vaultic_bigdigits.c

${OBJECTDIR}/_ext/1023103441/vaultic_ecdsa_key_aux.o: nbproject/Makefile-${CND_CONF}.mk ../../../device/vaultic_1XX_family/vaultic_ecdsa_key_aux.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1023103441
	${RM} $@.d
	$(COMPILE.c) -g -Wall -D_DEBUG -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_1XX_family -I/usr/local/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1023103441/vaultic_ecdsa_key_aux.o ../../../device/vaultic_1XX_family/vaultic_ecdsa_key_aux.c

${OBJECTDIR}/_ext/1023103441/vaultic_ecdsa_signer.o: nbproject/Makefile-${CND_CONF}.mk ../../../device/vaultic_1XX_family/vaultic_ecdsa_signer.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1023103441
	${RM} $@.d
	$(COMPILE.c) -g -Wall -D_DEBUG -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_1XX_family -I/usr/local/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1023103441/vaultic_ecdsa_signer.o ../../../device/vaultic_1XX_family/vaultic_ecdsa_signer.c

${OBJECTDIR}/_ext/1023103441/vaultic_ecsda_strong_authentication.o: nbproject/Makefile-${CND_CONF}.mk ../../../device/vaultic_1XX_family/vaultic_ecsda_strong_authentication.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1023103441
	${RM} $@.d
	$(COMPILE.c) -g -Wall -D_DEBUG -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_1XX_family -I/usr/local/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1023103441/vaultic_ecsda_strong_authentication.o ../../../device/vaultic_1XX_family/vaultic_ecsda_strong_authentication.c

${OBJECTDIR}/_ext/1023103441/vaultic_elliptic-ff2n.o: nbproject/Makefile-${CND_CONF}.mk ../../../device/vaultic_1XX_family/vaultic_elliptic-ff2n.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1023103441
	${RM} $@.d
	$(COMPILE.c) -g -Wall -D_DEBUG -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_1XX_family -I/usr/local/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1023103441/vaultic_elliptic-ff2n.o ../../../device/vaultic_1XX_family/vaultic_elliptic-ff2n.c

${OBJECTDIR}/_ext/1023103441/vaultic_ff2n.o: nbproject/Makefile-${CND_CONF}.mk ../../../device/vaultic_1XX_family/vaultic_ff2n.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1023103441
	${RM} $@.d
	$(COMPILE.c) -g -Wall -D_DEBUG -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_1XX_family -I/usr/local/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1023103441/vaultic_ff2n.o ../../../device/vaultic_1XX_family/vaultic_ff2n.c

${OBJECTDIR}/_ext/1023103441/vaultic_id_key_aux.o: nbproject/Makefile-${CND_CONF}.mk ../../../device/vaultic_1XX_family/vaultic_id_key_aux.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1023103441
	${RM} $@.d
	$(COMPILE.c) -g -Wall -D_DEBUG -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_1XX_family -I/usr/local/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1023103441/vaultic_id_key_aux.o ../../../device/vaultic_1XX_family/vaultic_id_key_aux.c

${OBJECTDIR}/_ext/1023103441/vaultic_key_aux.o: nbproject/Makefile-${CND_CONF}.mk ../../../device/vaultic_1XX_family/vaultic_key_aux.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1023103441
	${RM} $@.d
	$(COMPILE.c) -g -Wall -D_DEBUG -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_1XX_family -I/usr/local/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1023103441/vaultic_key_aux.o ../../../device/vaultic_1XX_family/vaultic_key_aux.c

${OBJECTDIR}/_ext/1023103441/vaultic_lib.o: nbproject/Makefile-${CND_CONF}.mk ../../../device/vaultic_1XX_family/vaultic_lib.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1023103441
	${RM} $@.d
	$(COMPILE.c) -g -Wall -D_DEBUG -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_1XX_family -I/usr/local/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1023103441/vaultic_lib.o ../../../device/vaultic_1XX_family/vaultic_lib.c

${OBJECTDIR}/_ext/1023103441/vaultic_raw_key_aux.o: nbproject/Makefile-${CND_CONF}.mk ../../../device/vaultic_1XX_family/vaultic_raw_key_aux.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1023103441
	${RM} $@.d
	$(COMPILE.c) -g -Wall -D_DEBUG -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_1XX_family -I/usr/local/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1023103441/vaultic_raw_key_aux.o ../../../device/vaultic_1XX_family/vaultic_raw_key_aux.c

${OBJECTDIR}/_ext/1023103441/vaultic_sha256.o: nbproject/Makefile-${CND_CONF}.mk ../../../device/vaultic_1XX_family/vaultic_sha256.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1023103441
	${RM} $@.d
	$(COMPILE.c) -g -Wall -D_DEBUG -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_1XX_family -I/usr/local/include/PCSC -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1023103441/vaultic_sha256.o ../../../device/vaultic_1XX_family/vaultic_sha256.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_PLATFORM}/${CND_CONF}/libvaultic_api_1xx.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
