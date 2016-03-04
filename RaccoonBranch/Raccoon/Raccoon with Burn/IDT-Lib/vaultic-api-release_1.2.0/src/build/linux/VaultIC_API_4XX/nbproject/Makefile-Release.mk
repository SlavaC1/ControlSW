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
CND_CONF=Release
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
	${OBJECTDIR}/_ext/1486021609/cheetah.o \
	${OBJECTDIR}/_ext/1486021609/vaultic_iso7816_protocol.o \
	${OBJECTDIR}/_ext/1486021609/vaultic_mem.o \
	${OBJECTDIR}/_ext/1486021609/vaultic_spi_aardvark.o \
	${OBJECTDIR}/_ext/1486021609/vaultic_spi_cheetah.o \
	${OBJECTDIR}/_ext/1486021609/vaultic_spi_peripheral.o \
	${OBJECTDIR}/_ext/1486021609/vaultic_timer_delay.o \
	${OBJECTDIR}/_ext/1486021609/vaultic_twi_peripheral.o \
	${OBJECTDIR}/_ext/907750266/vaultic_AES.o \
	${OBJECTDIR}/_ext/907750266/vaultic_DES.o \
	${OBJECTDIR}/_ext/907750266/vaultic_TDES.o \
	${OBJECTDIR}/_ext/907750266/vaultic_block_protocol.o \
	${OBJECTDIR}/_ext/907750266/vaultic_cipher.o \
	${OBJECTDIR}/_ext/907750266/vaultic_cipher_tests.o \
	${OBJECTDIR}/_ext/907750266/vaultic_command.o \
	${OBJECTDIR}/_ext/907750266/vaultic_comms.o \
	${OBJECTDIR}/_ext/907750266/vaultic_crc16.o \
	${OBJECTDIR}/_ext/907750266/vaultic_file_system.o \
	${OBJECTDIR}/_ext/907750266/vaultic_identity_authentication.o \
	${OBJECTDIR}/_ext/907750266/vaultic_key_wrapping.o \
	${OBJECTDIR}/_ext/907750266/vaultic_ms_auth.o \
	${OBJECTDIR}/_ext/907750266/vaultic_padding.o \
	${OBJECTDIR}/_ext/907750266/vaultic_protocol.o \
	${OBJECTDIR}/_ext/907750266/vaultic_scp02.o \
	${OBJECTDIR}/_ext/907750266/vaultic_scp03.o \
	${OBJECTDIR}/_ext/907750266/vaultic_secure_channel.o \
	${OBJECTDIR}/_ext/907750266/vaultic_signer_aes_cmac.o \
	${OBJECTDIR}/_ext/907750266/vaultic_signer_iso9797.o \
	${OBJECTDIR}/_ext/907750266/vaultic_symmetric_signer.o \
	${OBJECTDIR}/_ext/907750266/vaultic_utils.o \
	${OBJECTDIR}/_ext/1612643956/vaultic_api.o \
	${OBJECTDIR}/_ext/1612643956/vaultic_lib.o \
	${OBJECTDIR}/_ext/1612643956/vaultic_putkey_aux.o \
	${OBJECTDIR}/_ext/1612643956/vaultic_readkey_aux.o


# C Compiler Flags
CFLAGS=-L/lib/ -lpcsclite

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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_PLATFORM}/${CND_CONF}/libvaultic_api_4xx.${CND_DLIB_EXT}

${CND_DISTDIR}/${CND_PLATFORM}/${CND_CONF}/libvaultic_api_4xx.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_PLATFORM}/${CND_CONF}
	${LINK.c} -o ${CND_DISTDIR}/${CND_PLATFORM}/${CND_CONF}/libvaultic_api_4xx.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -shared -fPIC

${OBJECTDIR}/_ext/1486021609/aardvark.o: ../../../arch/pc/shared/aardvark.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1486021609
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1486021609/aardvark.o ../../../arch/pc/shared/aardvark.c

${OBJECTDIR}/_ext/1486021609/aardvark_peripheral.o: ../../../arch/pc/shared/aardvark_peripheral.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1486021609
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1486021609/aardvark_peripheral.o ../../../arch/pc/shared/aardvark_peripheral.c

${OBJECTDIR}/_ext/1486021609/cheetah.o: ../../../arch/pc/shared/cheetah.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1486021609
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1486021609/cheetah.o ../../../arch/pc/shared/cheetah.c

${OBJECTDIR}/_ext/1486021609/vaultic_iso7816_protocol.o: ../../../arch/pc/shared/vaultic_iso7816_protocol.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1486021609
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1486021609/vaultic_iso7816_protocol.o ../../../arch/pc/shared/vaultic_iso7816_protocol.c

${OBJECTDIR}/_ext/1486021609/vaultic_mem.o: ../../../arch/pc/shared/vaultic_mem.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1486021609
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1486021609/vaultic_mem.o ../../../arch/pc/shared/vaultic_mem.c

${OBJECTDIR}/_ext/1486021609/vaultic_spi_aardvark.o: ../../../arch/pc/shared/vaultic_spi_aardvark.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1486021609
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1486021609/vaultic_spi_aardvark.o ../../../arch/pc/shared/vaultic_spi_aardvark.c

${OBJECTDIR}/_ext/1486021609/vaultic_spi_cheetah.o: ../../../arch/pc/shared/vaultic_spi_cheetah.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1486021609
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1486021609/vaultic_spi_cheetah.o ../../../arch/pc/shared/vaultic_spi_cheetah.c

${OBJECTDIR}/_ext/1486021609/vaultic_spi_peripheral.o: ../../../arch/pc/shared/vaultic_spi_peripheral.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1486021609
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1486021609/vaultic_spi_peripheral.o ../../../arch/pc/shared/vaultic_spi_peripheral.c

${OBJECTDIR}/_ext/1486021609/vaultic_timer_delay.o: ../../../arch/pc/shared/vaultic_timer_delay.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1486021609
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1486021609/vaultic_timer_delay.o ../../../arch/pc/shared/vaultic_timer_delay.c

${OBJECTDIR}/_ext/1486021609/vaultic_twi_peripheral.o: ../../../arch/pc/shared/vaultic_twi_peripheral.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1486021609
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1486021609/vaultic_twi_peripheral.o ../../../arch/pc/shared/vaultic_twi_peripheral.c

${OBJECTDIR}/_ext/907750266/vaultic_AES.o: ../../../common/vaultic_AES.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_AES.o ../../../common/vaultic_AES.c

${OBJECTDIR}/_ext/907750266/vaultic_DES.o: ../../../common/vaultic_DES.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_DES.o ../../../common/vaultic_DES.c

${OBJECTDIR}/_ext/907750266/vaultic_TDES.o: ../../../common/vaultic_TDES.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_TDES.o ../../../common/vaultic_TDES.c

${OBJECTDIR}/_ext/907750266/vaultic_block_protocol.o: ../../../common/vaultic_block_protocol.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_block_protocol.o ../../../common/vaultic_block_protocol.c

${OBJECTDIR}/_ext/907750266/vaultic_cipher.o: ../../../common/vaultic_cipher.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_cipher.o ../../../common/vaultic_cipher.c

${OBJECTDIR}/_ext/907750266/vaultic_cipher_tests.o: ../../../common/vaultic_cipher_tests.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_cipher_tests.o ../../../common/vaultic_cipher_tests.c

${OBJECTDIR}/_ext/907750266/vaultic_command.o: ../../../common/vaultic_command.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_command.o ../../../common/vaultic_command.c

${OBJECTDIR}/_ext/907750266/vaultic_comms.o: ../../../common/vaultic_comms.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_comms.o ../../../common/vaultic_comms.c

${OBJECTDIR}/_ext/907750266/vaultic_crc16.o: ../../../common/vaultic_crc16.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_crc16.o ../../../common/vaultic_crc16.c

${OBJECTDIR}/_ext/907750266/vaultic_file_system.o: ../../../common/vaultic_file_system.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_file_system.o ../../../common/vaultic_file_system.c

${OBJECTDIR}/_ext/907750266/vaultic_identity_authentication.o: ../../../common/vaultic_identity_authentication.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_identity_authentication.o ../../../common/vaultic_identity_authentication.c

${OBJECTDIR}/_ext/907750266/vaultic_key_wrapping.o: ../../../common/vaultic_key_wrapping.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_key_wrapping.o ../../../common/vaultic_key_wrapping.c

${OBJECTDIR}/_ext/907750266/vaultic_ms_auth.o: ../../../common/vaultic_ms_auth.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_ms_auth.o ../../../common/vaultic_ms_auth.c

${OBJECTDIR}/_ext/907750266/vaultic_padding.o: ../../../common/vaultic_padding.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_padding.o ../../../common/vaultic_padding.c

${OBJECTDIR}/_ext/907750266/vaultic_protocol.o: ../../../common/vaultic_protocol.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_protocol.o ../../../common/vaultic_protocol.c

${OBJECTDIR}/_ext/907750266/vaultic_scp02.o: ../../../common/vaultic_scp02.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_scp02.o ../../../common/vaultic_scp02.c

${OBJECTDIR}/_ext/907750266/vaultic_scp03.o: ../../../common/vaultic_scp03.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_scp03.o ../../../common/vaultic_scp03.c

${OBJECTDIR}/_ext/907750266/vaultic_secure_channel.o: ../../../common/vaultic_secure_channel.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_secure_channel.o ../../../common/vaultic_secure_channel.c

${OBJECTDIR}/_ext/907750266/vaultic_signer_aes_cmac.o: ../../../common/vaultic_signer_aes_cmac.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_signer_aes_cmac.o ../../../common/vaultic_signer_aes_cmac.c

${OBJECTDIR}/_ext/907750266/vaultic_signer_iso9797.o: ../../../common/vaultic_signer_iso9797.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_signer_iso9797.o ../../../common/vaultic_signer_iso9797.c

${OBJECTDIR}/_ext/907750266/vaultic_symmetric_signer.o: ../../../common/vaultic_symmetric_signer.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_symmetric_signer.o ../../../common/vaultic_symmetric_signer.c

${OBJECTDIR}/_ext/907750266/vaultic_utils.o: ../../../common/vaultic_utils.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/907750266
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/907750266/vaultic_utils.o ../../../common/vaultic_utils.c

${OBJECTDIR}/_ext/1612643956/vaultic_api.o: ../../../device/vaultic_4XX_family/vaultic_api.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1612643956
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1612643956/vaultic_api.o ../../../device/vaultic_4XX_family/vaultic_api.c

${OBJECTDIR}/_ext/1612643956/vaultic_lib.o: ../../../device/vaultic_4XX_family/vaultic_lib.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1612643956
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1612643956/vaultic_lib.o ../../../device/vaultic_4XX_family/vaultic_lib.c

${OBJECTDIR}/_ext/1612643956/vaultic_putkey_aux.o: ../../../device/vaultic_4XX_family/vaultic_putkey_aux.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1612643956
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1612643956/vaultic_putkey_aux.o ../../../device/vaultic_4XX_family/vaultic_putkey_aux.c

${OBJECTDIR}/_ext/1612643956/vaultic_readkey_aux.o: ../../../device/vaultic_4XX_family/vaultic_readkey_aux.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1612643956
	${RM} $@.d
	$(COMPILE.c) -O2 -DVAULTIC_4XX_FIPS -D_LINUX -I../../../common -I../../../arch/pc/shared -I/usr/include/PCSC -I../../../device/vaultic_4XX_family -I/usr/local/include/PCSC -fPIC  -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1612643956/vaultic_readkey_aux.o ../../../device/vaultic_4XX_family/vaultic_readkey_aux.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_PLATFORM}/${CND_CONF}/libvaultic_api_4xx.${CND_DLIB_EXT}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
