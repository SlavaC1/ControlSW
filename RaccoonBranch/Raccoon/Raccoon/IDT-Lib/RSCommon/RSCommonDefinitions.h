#ifndef RSCommonDefsH
#define RSCommonDefsH

#include "stdafx.h"
#include <string>
#include <sstream>
#include "eccrypto.h"

using namespace std;

namespace RSCommon
{
#if defined(WIN32) || defined(_WIN32)
#	ifdef _LIB
#		define RSCOMMON_LIB
#	elif RSCOMMON_EXPORTS
#       define RSCOMMON_LIB __declspec(dllexport)
#   elif defined(RSCOMMON_IMPORT)
#       define RSCOMMON_LIB __declspec(dllimport)
#   else
#       define RSCOMMON_LIB
#   endif
#else
#   define RSCOMMON_LIB
#endif

#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

#define K283
#ifdef K283
	// Cryptography:
#define CURVE				ASN1::sect283k1()
#define PUBLIC_KEY_SIZE		72   ///331
	// ceiling of 283 bits curve
#define KEY_COEFF_SIZE		36
#define PRIVATE_KEY_SIZE	303
#endif
#ifdef K163
	// Cryptography:
#define CURVE				ASN1::sect163k1()
#define PUBLIC_KEY_SIZE		225
	// ceiling of 163 bits curve
#define KEY_COEFF_SIZE		21
#define PRIVATE_KEY_SIZE	212
#endif
#define SIGNATURE_SIZE		2 * KEY_COEFF_SIZE

	// Certificate:
#define BATCH_NO_SIZE		10

	// Errors:

#define IDTLIB_SUCCESS					0

#define IDTLIB_ERROR( module, errorcode ) ((int)((int)(module) + (int)(errorcode)))

#define FCB								0xE200
#define IDTLIB_AUTHENTICATE_CARTRIDGE	0xE300
#define IDTLIB_VERIFY_VOLUME			0xE400
#define IDTLIB_BURN_IDC					0xE500

#define EXCEPTION_CAUGHT				0xF000

#define FCB_INIT_FAILED					IDTLIB_ERROR(FCB, 0)
#define FCB_SELECTION					IDTLIB_ERROR(FCB, 1)
#define FCB_GET_IN_PLACE_STATUS_FAILED	IDTLIB_ERROR(FCB, 2)
#define FCB_SELECT_CHANNEL_FAILED		IDTLIB_ERROR(FCB, 3)
#define FCB_NULL_PARAM					IDTLIB_ERROR(FCB, 4)
#define HW_NOT_INITIALIZED				IDTLIB_ERROR(FCB, 5)
#define HSM_NOT_INITIALIZED				IDTLIB_ERROR(FCB, 6)
#define GET_INFO_FAILED					IDTLIB_ERROR(FCB, 0x10)

#define INVALID_CERTIFICATE_SIGNATURE	IDTLIB_ERROR(IDTLIB_AUTHENTICATE_CARTRIDGE, 0)
#define INVALID_TOKEN_SIGNATURE			IDTLIB_ERROR(IDTLIB_AUTHENTICATE_CARTRIDGE, 1)
#define INVALID_CERTIFICATE_FILE		IDTLIB_ERROR(IDTLIB_AUTHENTICATE_CARTRIDGE, 2)
#define INVALID_HOST_KEY				IDTLIB_ERROR(IDTLIB_AUTHENTICATE_CARTRIDGE, 3)
#define AUTHENTICATE_NULL_PARAMS		IDTLIB_ERROR(IDTLIB_AUTHENTICATE_CARTRIDGE, 4)
#define EMPTY_CERTIFICATE_FILE			IDTLIB_ERROR(IDTLIB_AUTHENTICATE_CARTRIDGE, 5)
#define INVALID_DEVICE_KEY				IDTLIB_ERROR(IDTLIB_AUTHENTICATE_CARTRIDGE, 6)
#define CHALLENGE_FAILED				IDTLIB_ERROR(IDTLIB_AUTHENTICATE_CARTRIDGE, 7)
#define FILE_SELECTION_FAILED			IDTLIB_ERROR(IDTLIB_AUTHENTICATE_CARTRIDGE, 8)

#define CARTRIDGE_NOT_AUTHENTICATED		IDTLIB_ERROR(IDTLIB_VERIFY_VOLUME, 0)
#define INVALID_VOLUME_SIGNATURE		IDTLIB_ERROR(IDTLIB_VERIFY_VOLUME, 1)
#define MATERIAL_OVERCONSUMPTION		IDTLIB_ERROR(IDTLIB_VERIFY_VOLUME, 2)
#define GENERATE_RANDOM_FAILED			IDTLIB_ERROR(IDTLIB_VERIFY_VOLUME, 3)
#define CONSUMPTION_NULL_PARAMS			IDTLIB_ERROR(IDTLIB_VERIFY_VOLUME, 4)

	// Logger:
#define LOG_EXT(level, message)					\
	{                                           \
	std::ostringstream oss;						\
	oss << message;								\
	WriteLogMessage(level, oss.str().c_str());	\
}\

#define LEVEL_DEBUG	"DEBUG"
#define LEVEL_INFO	"INFO"
#define LEVEL_WARN	"WARN"
#define LEVEL_ERROR	"ERROR"
#define LEVEL_FATAL	"FATAL"

	enum enAdapterType
	{
		VaultIC100 = 0
	};

	// These defines for Borland compatibility
	using namespace CryptoPP;
	typedef ECDSA<EC2N, SHA256>::PublicKey PubKey;
	typedef ECDSA<EC2N, SHA256>::Verifier  Verif;
}

#endif