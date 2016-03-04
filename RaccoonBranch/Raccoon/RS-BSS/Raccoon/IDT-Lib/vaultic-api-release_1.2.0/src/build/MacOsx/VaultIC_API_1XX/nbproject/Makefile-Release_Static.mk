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
CND_PLATFORM=GNU-MacOSX
CND_DLIB_EXT=dylib
CND_CONF=Release_Static
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1023103441/vaultic_ecdsa_key_aux.o \
	${OBJECTDIR}/_ext/1486021609/vaultic_mem.o \
	${OBJECTDIR}/_ext/1023103441/vaultic_key_aux.o \
	${OBJECTDIR}/_ext/1023103441/vaultic_ecsda_strong_authentication.o \
	${OBJECTDIR}/_ext/1023103441/vaultic_id_key_aux.o \
	${OBJECTDIR}/_ext/1023103441/vaultic_api.o \
	${OBJECTDIR}/_ext/1023103441/vaultic_raw_key_aux.o \
	${OBJECTDIR}/_ext/432963042/vaultic_comms.o \
	${OBJECTDIR}/_ext/432963042/vaultic_utils.o \
	${OBJECTDIR}/_ext/432963042/vaultic_command.o \
	${OBJECTDIR}/_ext/1023103441/vaultic_lib.o \
	${OBJECTDIR}/_ext/1486021609/vaultic_iso7816_protocol.o \
	${OBJECTDIR}/_ext/1023103441/vaultic_ecdsa_signer.o \
	${OBJECTDIR}/_ext/1023103441/vaultic_bigdigits.o \
	${OBJECTDIR}/_ext/432963042/vaultic_protocol.o \
	${OBJECTDIR}/_ext/432963042/vaultic_crc16.o \
	${OBJECTDIR}/_ext/1023103441/vaultic_elliptic-ff2n.o \
	${OBJECTDIR}/_ext/1023103441/vaultic_sha256.o \
	${OBJECTDIR}/_ext/1023103441/vaultic_ff2n.o \
	${OBJECTDIR}/_ext/1486021609/vaultic_timer_delay.o


# C Compiler Flags
CFLAGS=-lpcsclite

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
	$(RANLIB) ${CND_DISTDIR}/${CND_PLATFORM}/${CND_CONF}/libvaultic_api_1xx.a

${OBJECTDIR}/_ext/1023103441/vaultic_ecdsa_key_aux.o: ../../../device/vaultic_1XX_family/vaultic_ecdsa_key_aux.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1023103441
	${RM} $@.d
	$(COMPILE.c) -O2 -D_MACOS -I/usr/include/PCSC -I/usr/local/include/PCSC -I../../../common -I../../../arch/pc/shared -I../../../device/vaultic_1XX_family -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1023103441/vaultic_ecdsa_key_aux.o ../../../device/vaultic_1XX_family/vaultic_ecdsa_key_aux.c

${OBJECTDIR}/_ext/1486021609/vaultic_mem.o: ../../../arch/pc/shared/vaultic_mem.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1486021609
	${RM} $@.d
	$(COMPILE.c) -O2 -D_MACOS -I/usr/include/PCSC -I/usr/local/include/PCSC -I../../../common -I../../../arch/pc/shared -I../../../device/vaultic_1XX_family -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1486021609/vaultic_mem.o ../../../arch/pc/shared/vaultic_mem.c

${OBJECTDIR}/_ext/1023103441/vaultic_key_aux.o: ../../../device/vaultic_1XX_family/vaultic_key_aux.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1023103441
	${RM} $@.d
	$(COMPILE.c) -O2 -D_MACOS -I/usr/include/PCSC -I/usr/local/include/PCSC -I../../../common -I../../../arch/pc/shared -I../../../device/vaultic_1XX_family -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1023103441/vaultic_key_aux.o ../../../device/vaultic_1XX_family/vaultic_key_aux.c

${OBJECTDIR}/_ext/1023103441/vaultic_ecsda_strong_authentication.o: ../../../device/vaultic_1XX_family/vaultic_ecsda_strong_authentication.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1023103441
	${RM} $@.d
	$(COMPILE.c) -O2 -D_MACOS -I/usr/include/PCSC -I/usr/local/include/PCSC -I../../../common -I../../../arch/pc/shared -I../../../device/vaultic_1XX_family -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1023103441/vaultic_ecsda_strong_authentication.o ../../../device/vaultic_1XX_family/vaultic_ecsda_strong_authentication.c

${OBJECTDIR}/_ext/1023103441/vaultic_id_key_aux.o: ../../../device/vaultic_1XX_family/vaultic_id_key_aux.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1023103441
	${RM} $@.d
	$(COMPILE.c) -O2 -D_MACOS -I/usr/include/PCSC -I/usr/local/include/PCSC -I../../../common -I../../../arch/pc/shared -I../../../device/vaultic_1XX_family -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1023103441/vaultic_id_key_aux.o ../../../device/vaultic_1XX_family/vaultic_id_key_aux.c

${OBJECTDIR}/_ext/1023103441/vaultic_api.o: ../../../device/vaultic_1XX_family/vaultic_api.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1023103441
	${RM} $@.d
	$(COMPILE.c) -O2 -D_MACOS -I/usr/include/PCSC -I/usr/local/include/PCSC -I../../../common -I../../../arch/pc/shared -I../../../device/vaultic_1XX_family -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1023103441/vaultic_api.o ../../../device/vaultic_1XX_family/vaultic_api.c

${OBJECTDIR}/_ext/1023103441/vaultic_raw_key_aux.o: ../../../device/vaultic_1XX_family/vaultic_raw_key_aux.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1023103441
	${RM} $@.d
	$(COMPILE.c) -O2 -D_MACOS -I/usr/include/PCSC -I/usr/local/include/PCSC -I../../../common -I../../../arch/pc/shared -I../../../device/vaultic_1XX_family -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1023103441/vaultic_raw_key_aux.o ../../../device/vaultic_1XX_family/vaultic_raw_key_aux.c

${OBJECTDIR}/_ext/432963042/vaultic_comms.o: /Users/inside/dev/VaultIC_Librairies/VaultIC_API/common/vaultic_comms.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/432963042
	${RM} $@.d
	$(COMPILE.c) -O2 -D_MACOS -I/usr/include/PCSC -I/usr/local/include/PCSC -I../../../common -I../../../arch/pc/shared -I../../../device/vaultic_1XX_family -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/432963042/vaultic_comms.o /Users/inside/dev/VaultIC_Librairies/VaultIC_API/common/vaultic_comms.c

${OBJECTDIR}/_ext/432963042/vaultic_utils.o: /Users/inside/dev/VaultIC_Librairies/VaultIC_API/common/vaultic_utils.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/432963042
	${RM} $@.d
	$(COMPILE.c) -O2 -D_MACOS -I/usr/include/PCSC -I/usr/local/include/PCSC -I../../../common -I../../../arch/pc/shared -I../../../device/vaultic_1XX_family -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/432963042/vaultic_utils.o /Users/inside/dev/VaultIC_Librairies/VaultIC_API/common/vaultic_utils.c

${OBJECTDIR}/_ext/432963042/vaultic_command.o: /Users/inside/dev/VaultIC_Librairies/VaultIC_API/common/vaultic_command.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/432963042
	${RM} $@.d
	$(COMPILE.c) -O2 -D_MACOS -I/usr/include/PCSC -I/usr/local/include/PCSC -I../../../common -I../../../arch/pc/shared -I../../../device/vaultic_1XX_family -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/432963042/vaultic_command.o /Users/inside/dev/VaultIC_Librairies/VaultIC_API/common/vaultic_command.c

${OBJECTDIR}/_ext/1023103441/vaultic_lib.o: ../../../device/vaultic_1XX_family/vaultic_lib.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1023103441
	${RM} $@.d
	$(COMPILE.c) -O2 -D_MACOS -I/usr/include/PCSC -I/usr/local/include/PCSC -I../../../common -I../../../arch/pc/shared -I../../../device/vaultic_1XX_family -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1023103441/vaultic_lib.o ../../../device/vaultic_1XX_family/vaultic_lib.c

${OBJECTDIR}/_ext/1486021609/vaultic_iso7816_protocol.o: ../../../arch/pc/shared/vaultic_iso7816_protocol.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1486021609
	${RM} $@.d
	$(COMPILE.c) -O2 -D_MACOS -I/usr/include/PCSC -I/usr/local/include/PCSC -I../../../common -I../../../arch/pc/shared -I../../../device/vaultic_1XX_family -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1486021609/vaultic_iso7816_protocol.o ../../../arch/pc/shared/vaultic_iso7816_protocol.c

${OBJECTDIR}/_ext/1023103441/vaultic_ecdsa_signer.o: ../../../device/vaultic_1XX_family/vaultic_ecdsa_signer.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1023103441
	${RM} $@.d
	$(COMPILE.c) -O2 -D_MACOS -I/usr/include/PCSC -I/usr/local/include/PCSC -I../../../common -I../../../arch/pc/shared -I../../../device/vaultic_1XX_family -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1023103441/vaultic_ecdsa_signer.o ../../../device/vaultic_1XX_family/vaultic_ecdsa_signer.c

${OBJECTDIR}/_ext/1023103441/vaultic_bigdigits.o: ../../../device/vaultic_1XX_family/vaultic_bigdigits.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1023103441
	${RM} $@.d
	$(COMPILE.c) -O2 -D_MACOS -I/usr/include/PCSC -I/usr/local/include/PCSC -I../../../common -I../../../arch/pc/shared -I../../../device/vaultic_1XX_family -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1023103441/vaultic_bigdigits.o ../../../device/vaultic_1XX_family/vaultic_bigdigits.c

${OBJECTDIR}/_ext/432963042/vaultic_protocol.o: /Users/inside/dev/VaultIC_Librairies/VaultIC_API/common/vaultic_protocol.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/432963042
	${RM} $@.d
	$(COMPILE.c) -O2 -D_MACOS -I/usr/include/PCSC -I/usr/local/include/PCSC -I../../../common -I../../../arch/pc/shared -I../../../device/vaultic_1XX_family -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/432963042/vaultic_protocol.o /Users/inside/dev/VaultIC_Librairies/VaultIC_API/common/vaultic_protocol.c

${OBJECTDIR}/_ext/432963042/vaultic_crc16.o: /Users/inside/dev/VaultIC_Librairies/VaultIC_API/common/vaultic_crc16.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/432963042
	${RM} $@.d
	$(COMPILE.c) -O2 -D_MACOS -I/usr/include/PCSC -I/usr/local/include/PCSC -I../../../common -I../../../arch/pc/shared -I../../../device/vaultic_1XX_family -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/432963042/vaultic_crc16.o /Users/inside/dev/VaultIC_Librairies/VaultIC_API/common/vaultic_crc16.c

${OBJECTDIR}/_ext/1023103441/vaultic_elliptic-ff2n.o: ../../../device/vaultic_1XX_family/vaultic_elliptic-ff2n.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1023103441
	${RM} $@.d
	$(COMPILE.c) -O2 -D_MACOS -I/usr/include/PCSC -I/usr/local/include/PCSC -I../../../common -I../../../arch/pc/shared -I../../../device/vaultic_1XX_family -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1023103441/vaultic_elliptic-ff2n.o ../../../device/vaultic_1XX_family/vaultic_elliptic-ff2n.c

${OBJECTDIR}/_ext/1023103441/vaultic_sha256.o: ../../../device/vaultic_1XX_family/vaultic_sha256.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1023103441
	${RM} $@.d
	$(COMPILE.c) -O2 -D_MACOS -I/usr/include/PCSC -I/usr/local/include/PCSC -I../../../common -I../../../arch/pc/shared -I../../../device/vaultic_1XX_family -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1023103441/vaultic_sha256.o ../../../device/vaultic_1XX_family/vaultic_sha256.c

${OBJECTDIR}/_ext/1023103441/vaultic_ff2n.o: ../../../device/vaultic_1XX_family/vaultic_ff2n.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1023103441
	${RM} $@.d
	$(COMPILE.c) -O2 -D_MACOS -I/usr/include/PCSC -I/usr/local/include/PCSC -I../../../common -I../../../arch/pc/shared -I../../../device/vaultic_1XX_family -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1023103441/vaultic_ff2n.o ../../../device/vaultic_1XX_family/vaultic_ff2n.c

${OBJECTDIR}/_ext/1486021609/vaultic_timer_delay.o: ../../../arch/pc/shared/vaultic_timer_delay.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1486021609
	${RM} $@.d
	$(COMPILE.c) -O2 -D_MACOS -I/usr/include/PCSC -I/usr/local/include/PCSC -I../../../common -I../../../arch/pc/shared -I../../../device/vaultic_1XX_family -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1486021609/vaultic_timer_delay.o ../../../arch/pc/shared/vaultic_timer_delay.c

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
