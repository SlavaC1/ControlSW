#include "stdafx.h"
#include "IdentificationData.h"
#include "oids.h"

using namespace CryptoPP;

namespace RSCommon
{
	MaterialInformation::MaterialInformation(const MaterialInformation& rhs)
	{
		TagStructID = rhs.TagStructID;
		MaterialType = rhs.MaterialType;
		MfgDateTime = rhs.MfgDateTime;
		memcpy(BatchNo, rhs.BatchNo, BATCH_NO_SIZE);
		InitialWeight = rhs.InitialWeight;
		ExpirationDate = rhs.ExpirationDate;
	}

	size_t MaterialInformation::Encode(unsigned char *destination) const
	{
		size_t offset = 0;

		memcpy(destination + offset, &TagStructID, sizeof(TagStructID));
		offset += sizeof(TagStructID);
		memcpy(destination + offset, &MaterialType, sizeof(MaterialType));
		offset += sizeof(MaterialType);
		memcpy(destination + offset, &MfgDateTime, sizeof(MfgDateTime));
		offset += sizeof(MfgDateTime);
		memcpy(destination + offset, BatchNo, BATCH_NO_SIZE);
		offset += BATCH_NO_SIZE;
		memcpy(destination + offset, &InitialWeight, sizeof(InitialWeight));
		offset += sizeof(InitialWeight);
		memcpy(destination + offset, &ExpirationDate, sizeof(ExpirationDate));
		offset += sizeof(ExpirationDate);
		memcpy(destination + offset, &ExtraWeight, sizeof(ExtraWeight));
		offset += sizeof(ExtraWeight);

		return offset;
	}

	size_t MaterialInformation::Decode(const unsigned char *source)
	{
		size_t offset = 0;

		memcpy(&TagStructID, source + offset, sizeof(TagStructID));
		offset += sizeof(TagStructID);
		memcpy(&MaterialType, source + offset, sizeof(MaterialType));
		offset += sizeof(MaterialType);
		memcpy(&MfgDateTime, source + offset, sizeof(MfgDateTime));
		offset += sizeof(MfgDateTime);
		memcpy(BatchNo, source + offset, BATCH_NO_SIZE);
		offset += BATCH_NO_SIZE;
		memcpy(&InitialWeight, source + offset, sizeof(InitialWeight));
		offset += sizeof(InitialWeight);
		memcpy(&ExpirationDate, source + offset, sizeof(ExpirationDate));
		offset += sizeof(ExpirationDate);
		memcpy(&ExtraWeight, source + offset, sizeof(ExtraWeight));
		offset += sizeof(ExtraWeight);

		return offset;
	}

	IdentificationData::IdentificationData()
	{
		m_serialNoLength = 0;
		m_serialNo = NULL;
		m_pubKC = new PubKey();
		m_verifier = NULL;
	}

	IdentificationData::IdentificationData(const IdentificationData& rhs)
	{
		MaterialInfo = rhs.MaterialInfo;
		m_serialNoLength = rhs.m_serialNoLength;
		m_serialNo = new unsigned char[m_serialNoLength];
		memcpy(m_serialNo, rhs.m_serialNo, m_serialNoLength);
		m_pubKC = new PubKey(*rhs.m_pubKC);
		m_verifier = NULL;
	}

	IdentificationData::~IdentificationData()
	{
		if (m_serialNo != NULL)
		{
			delete m_serialNo;
		}
		if (m_verifier != NULL)
		{
			delete m_verifier;
		}
		if (m_pubKC != NULL)
		{
			delete m_pubKC;
		}
	}

	size_t IdentificationData::Encode(unsigned char *destination) const
	{
		size_t offset = 0;

		offset += this->MaterialInfo.Encode(destination);
		memcpy(destination + offset, &m_serialNoLength, sizeof(m_serialNoLength));
		offset += sizeof(m_serialNoLength);
		if (m_serialNoLength > 0)
		{
			memcpy(destination + offset, m_serialNo, m_serialNoLength);
			offset += m_serialNoLength;
		}

		EC2NPoint Q = m_pubKC->GetPublicElement();
		Q.x.Encode(destination + offset, Q.x.ByteCount());
		offset += Q.x.ByteCount();
		Q.y.Encode(destination + offset, Q.y.ByteCount());
		offset += Q.y.ByteCount();

		return offset;
	}

	size_t IdentificationData::Decode(const unsigned char *source)
	{
		size_t offset = 0;

		offset += this->MaterialInfo.Decode(source);
		unsigned char serialNoLength;
		memcpy(&serialNoLength, source + offset, sizeof(serialNoLength));
		offset += sizeof(serialNoLength);
		SetSerialNo(source + offset, serialNoLength);
		offset += serialNoLength;

		EC2NPoint Q;
		Q.identity = false;
		Q.x.Decode(source + offset, KEY_COEFF_SIZE);
		offset += KEY_COEFF_SIZE;
		Q.y.Decode(source + offset, KEY_COEFF_SIZE);
		offset += KEY_COEFF_SIZE;
		m_pubKC->Initialize(CURVE, Q);

		return offset;
	}

	PubKey* IdentificationData::GetCartridgePublicKey() const
	{
		return m_pubKC;
	}

	unsigned char* IdentificationData::GetSerialNo(unsigned char* pucSerialNoLength) const
	{
		*pucSerialNoLength = m_serialNoLength;
		return m_serialNo;
	}

	void IdentificationData::SetSerialNo(const unsigned char* acSerialNo, unsigned char ucSerialNoLength)
	{
		if (ucSerialNoLength != m_serialNoLength)
		{
			delete[] m_serialNo;
			m_serialNoLength = ucSerialNoLength;
			if (m_serialNoLength > 0)
			{
				m_serialNo = new unsigned char[m_serialNoLength];
			}
			else
			{
				m_serialNo = NULL;
			}
		}

		if (m_serialNoLength > 0)
		{
			memcpy(m_serialNo, acSerialNo, m_serialNoLength);
		}
	}

	void IdentificationData::SetCartridgePublicKey(const PubKey& pubKC)
	{
		if (m_verifier != NULL)
		{
			delete m_verifier;
			m_verifier = NULL;
		}

		if (m_pubKC != NULL)
		{
			delete m_pubKC;
		}

		m_pubKC = new PubKey(pubKC);
	}

	Verif* IdentificationData::GetVerifier()
	{
		if (m_verifier == NULL)
		{
			m_verifier = new Verif(*m_pubKC);
		}

		return m_verifier;
	}
}
