#include "HsmCrypt.h"
#include <stdio.h>

void PrintError(const char* funcName, unsigned __int64 err)
{
	char* pError = GetError();
	printf("%s - %s : %I64u\n", funcName, pError, err);
	FreeMemory(pError);
}

void main(int argc, wchar_t *argv[])
{
	unsigned __int64 err;
	const wchar_t* password;
	DWORD dwSlot;
	DWORD i;

	//if (argc < 3)
	//{
	//	return;
	//}

	//swscanf_s(argv[1], L"%d", &dwSlot);
	//password = argv[2];
	//dwSlot = 1;
	//password = L"qM7K-LPS5-JJXF-MKpx";
	dwSlot = 2;
	password = L"7Y/E-AbxA-KSs5-L4MS";

	//if( err = GenerateKey(dwSlot, password, TRUE, 2048) ) // RSA
	//if( err = GenerateKey(dwSlot, password, FALSE, Binary283) ) // ECC
	//	PrintError("GenerateKey", err);

	//if( err = Initialize(dwSlot, password, TRUE, 2048, HA_SHA512) ) // RSA
	if( err = Initialize(dwSlot, password, FALSE, Binary283, HA_SHA256) ) // ECC
		PrintError("Initialize", err);
	else
	{
		DWORD dwKey = GetKeySpec();

		BYTE data[] =
		{
			0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
			0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
		};

		PBYTE pbSignature;
		DWORD dwSize = sizeof(data);

		switch( dwKey )
		{
		case Prime256:
			printf("ECC P-256\n");
			break;

		case Prime384:
			printf("ECC P-384\n");
			break;

		case Prime521:
			printf("ECC P-521\n");
			break;

		case Binary163:
			printf("ECC K-163\n");
			break;

		case Binary283:
			printf("ECC K-283\n");
			break;

		default:
			printf("RSA %d bits\n", dwKey);
		}

		switch( GetHashAlg() )
		{
		case HA_SHA1:
			printf("SHA1\n");
			break;

		case HA_SHA256:
			printf("SHA256\n");
			break;

		case HA_SHA384:
			printf("SHA384\n");
			break;

		case HA_SHA512:
			printf("SHA512\n");
			break;
		}

		if( err = SignData(data, &pbSignature, &dwSize) )
			PrintError("SignData", err);
		else
		{
			PBYTE pbPublic;
			DWORD dwPblcSize;

			if( err = GetPublicKey(&pbPublic, &dwPblcSize) )
				PrintError("GetPublicKey", err);
			else
			{
				printf("Public key: ");
				for (i = 0; i < dwPblcSize; i++)
				{
					printf("0x%X, ", pbPublic[i]);
				}
				printf("\n");

				FreeMemory(pbPublic);
			}

			printf("Signature succeeded\n");

			if( err = VerifySignature(data, sizeof(data), pbSignature, dwSize) )
				PrintError("VerifySignature", err);
			else
				printf("Signature verification succeeded\n");

			FreeMemory(pbSignature);
		}

		Uninitialize();
	}
}
