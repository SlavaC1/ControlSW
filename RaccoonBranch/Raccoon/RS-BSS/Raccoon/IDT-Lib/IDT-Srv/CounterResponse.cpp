#include "stdafx.h"
#include "CounterResponse.h"
#include <string.h>

namespace StratasysIDTSrv
{
	CounterResponse::CounterResponse()
		:m_counterValue(0)
	{
		m_signature = NULL;
		m_signatureLength = 0;
	}

	CounterResponse::CounterResponse(const CounterResponse &rhs)
	{
		memcpy(m_randomMessage, rhs.m_randomMessage, RND_LENGTH);
		m_signatureLength = rhs.m_signatureLength;
		m_signature = new unsigned char[m_signatureLength];
		memcpy(m_signature, rhs.m_signature, m_signatureLength);
		m_counterValue = rhs.m_counterValue;
	}

	CounterResponse::~CounterResponse()
	{
		if (m_signature != NULL)
		{
			delete[] m_signature;
		}
	}

	void CounterResponse::SetCounterValue(unsigned char *counterValue)
	{
		memcpy(&m_counterValue, counterValue, COUNTER_LENGTH);
	}

	void CounterResponse::SetMessage(const unsigned char *rnd, const unsigned char *sn, const unsigned char *nounce)
	{
		memcpy(m_randomMessage, rnd, 8);
		memcpy(m_randomMessage + 8, sn, 8);
		memcpy(m_randomMessage + 16, nounce, 8);
		memcpy(m_randomMessage + 24, &m_counterValue, 4);
	}

	void CounterResponse::SetSignature(const unsigned char *signature, unsigned short signatureLength)
	{
		if (m_signature != NULL)
		{
			delete[] m_signature;
			m_signature = NULL;
			m_signatureLength = 0;
		}

		if (m_signatureLength != signatureLength)
		{
			m_signature = new unsigned char[signatureLength];
			m_signatureLength = signatureLength;
		}

		memcpy(m_signature, signature, signatureLength);
	}

	size_t CounterResponse::Encode(unsigned char* aucBuffer)
	{
		if (aucBuffer == NULL)
		{
			return sizeof(m_randomMessage) + m_signatureLength;
		}

		size_t offset = 0;
		memcpy(aucBuffer + offset, m_randomMessage, sizeof(m_randomMessage));
		offset += sizeof(m_randomMessage);
		memcpy(aucBuffer + offset, m_signature, m_signatureLength);
		offset += m_signatureLength;

		return offset;
	}

	size_t CounterResponse::Decode(const unsigned char* aucBuffer)
	{
		if (aucBuffer == NULL)
		{
			return -1;
		}

		size_t offset = 0;
		memcpy(m_randomMessage, aucBuffer + offset, sizeof(m_randomMessage));
		memcpy(&m_counterValue, m_randomMessage + 24, sizeof(m_counterValue));
		offset += sizeof(m_randomMessage);
		memcpy(m_signature, aucBuffer + offset, m_signatureLength);
		offset += m_signatureLength;

		return offset;
	}
}
