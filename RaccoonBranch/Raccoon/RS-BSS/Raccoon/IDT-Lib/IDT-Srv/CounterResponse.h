#ifndef CounterResponseH
#define CounterResponseH

namespace StratasysIDTSrv
{
#define COUNTER_LENGTH 4
	// random message + SN + Nounce + Counter value
#define RND_LENGTH 28

	class CounterResponse
	{
	public:
		CounterResponse();
		~CounterResponse();
		CounterResponse(const CounterResponse &rhs);

		unsigned int GetCounterValue() const { return m_counterValue; }
		unsigned char *GetRandomMessage() const { return (unsigned char *)m_randomMessage; }
		unsigned char *GetSignature() const { return m_signature; }
		unsigned short GetSignatureLength() const { return m_signatureLength; }

		void SetCounterValue(unsigned char *counterValue);
		void SetMessage(const unsigned char *rnd, const unsigned char *sn, const unsigned char *nounce);
		void SetSignature(const unsigned char *signature, unsigned short signatureLength);

		size_t Encode(unsigned char* aucBuffer);
		size_t Decode(const unsigned char* aucBuffer);

	private:
		/* New counter value */
		unsigned int m_counterValue;

		unsigned char m_randomMessage[RND_LENGTH];

		/** Signature length */
		unsigned short m_signatureLength;

		/** Signature */
		unsigned char *m_signature;
	};
}

#endif