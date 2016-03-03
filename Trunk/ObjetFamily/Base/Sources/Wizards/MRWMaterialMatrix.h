#ifndef _MRWMaterialMatrix_H_
#define _MRWMaterialMatrix_H_

#include <XMLDoc.hpp>
#include <MSXMLDOM.hpp>
#include <map>

#include "QException.h"
#include "QTypes.h"

#define MATRIX_NODE_ROOT_NAME 			"grid"
#define MATRIX_ATTR_LONG_MECHANICAL 	"economyLong"  //aka "longtube_mechanical"
#define MATRIX_ATTR_SHORT_MECHANICAL 	"economyShort" //aka "shorttube_mechanical"
#define MATRIX_ATTR_LONG_TINT 			"fullLong"     //aka "longtube_tint"
#define MATRIX_ATTR_SHORT_TINT 			"fullShort"    //aka "shorttube_tint"

#define _COUNTOF(a) (sizeof(a)/sizeof(*(a)))

typedef enum {tLongTube=0, tShortTube} 			TTubeType;
typedef enum {tFlushMechanical=0, tFlushTint} 	TFlushType;

class CMatrixCell
{
private:
	int m_longtube_mechanical;
	int m_shorttube_mechanical;
	int m_longtube_tint;
	int m_shorttube_tint;

public:
	CMatrixCell(int a, int b, int c, int d) :
		   m_longtube_mechanical(a)
		  ,m_shorttube_mechanical(b)
		  ,m_longtube_tint(c)
		  ,m_shorttube_tint(d)
		  {}

	int GetCycles(TTubeType tubetype, TFlushType flushtype) const
	{
		switch(tubetype)
		{
			case tLongTube:
			switch(flushtype)
			{
				case tFlushMechanical: 	return m_longtube_mechanical;
				case tFlushTint:		return m_longtube_tint;
				default: throw EQException("XML : Wrong flush type");
			}
			case tShortTube:
			switch(flushtype)
			{
				case tFlushMechanical: 	return m_shorttube_mechanical;
				case tFlushTint:		return m_shorttube_tint;
				default: throw EQException("XML : Wrong flush type");
			}
			default: throw EQException("XML : Wrong tube type");;
		}
	}  
};

class CMaterialsMatrix
{
private:
	bool m_initialized;
	typedef std::map <AnsiString, CMatrixCell*> TMatrixMap;
	TMatrixMap m_theMatrix;

	AnsiString Hash(AnsiString& src)
	{
		return src.LowerCase();
	}

	void SetCell(AnsiString &from, AnsiString &to, _di_IXMLNode &Node)
	{
		AnsiString key;
		TMatrixMap::const_iterator iter;

		AnsiString fromLower = Hash(from);
		AnsiString toLower = Hash(to);
		GenKey(fromLower, toLower, key);

		// Check if this entry already exists. (XML has a duplicate entry?)
		if ((iter = m_theMatrix.find(key)) != m_theMatrix.end())
			throw EQException("XML entry does not exist");

		// Create a new cell with Node's properties.
		CMatrixCell *cellptr = new CMatrixCell (
					 StrToInt(Node->Attributes[MATRIX_ATTR_LONG_MECHANICAL])
					,StrToInt(Node->Attributes[MATRIX_ATTR_SHORT_MECHANICAL])
					,StrToInt(Node->Attributes[MATRIX_ATTR_LONG_TINT])
					,StrToInt(Node->Attributes[MATRIX_ATTR_SHORT_TINT]));
		m_theMatrix[key] = cellptr;
	}

	void GenKey(AnsiString &from, AnsiString &to,  AnsiString &key)
	{
		key = AnsiString("@FROM@") + Hash(from) + "@TO@" + Hash(to);
	}

	//Support resin names that are not xml-valid
	QString Normalize(QString str)
	{
		//Replace "Plus" with special character '+'
		size_t len = str.length();
		size_t found;

		found=str.rfind("Plus");
		//if key is suffix of str, replace with
		if (len == found + 4)
			str = str.replace(found, 4, "+");

		//Replace all "_" with " "
		found = str.find("_");
		while (string::npos != found){
			str.replace( found, 1, " ");
			found = str.find("_", found + 1);
		};

		return str;
	}
	
	AnsiString Normalize(const wchar_t* str)
	{	//Convert the wide-string to char*, normalize it, and reconvert to wide-string
		try {

			char buf[64];
			WideCharToMultiByte(
			  CP_UTF8, /* Code page of the XML */
			  0, /* dwFlags */
			  str, /* input */
			  -1, /* length of the string - set -1 to indicate it is null terminated */
			  buf, /* output */
			  _COUNTOF(buf),
				/* size of the buffer in bytes -
				   if you leave it zero the return value is the
				   LENGTH required for the output buffer, and not the converted string itself */
			  NULL, NULL /* with UTF-8, lpDefaultChar=lpUsedDefaultChar=NULL */
			);

			return AnsiString(Normalize(QString(buf)).c_str());
		} catch (...) {
            return AnsiString(str);
        }
	}

public:
	int GetCycles(AnsiString &from, AnsiString &to, TTubeType tubetype, TFlushType flushtype)
	{
		if (!m_initialized)
			throw EQException("XML : Uninitialized matrix");

		AnsiString key;
		TMatrixMap::iterator iter;

		GenKey(from, to, key);
		if ((iter = m_theMatrix.find(key)) == m_theMatrix.end())
		{
			throw EQException(
				QFormatStr("Key (%s,%s) wasn't found. XML is incomplete or corrupted.",
				from.c_str(), to.c_str()));
		}

		return iter->second->GetCycles(tubetype, flushtype);
	}

	CMaterialsMatrix() : m_initialized(false) {}

	void Init(AnsiString &XMLFile)
	{
		if (m_initialized)
			throw EQException("XML : Cannot re-initialized matrix");

		
		_di_IXMLDocument 	XMLDoc = NewXMLDocument();
		_di_IXMLNodeList 	FromMaterials;
		_di_IXMLNodeList 	ToMaterials;
		_di_IXMLNode 		Node;  // Just a Nod.

		try
		{
			XMLDoc->LoadFromFile(XMLFile);
			Node = XMLDoc->GetChildNodes()->FindNode(MATRIX_NODE_ROOT_NAME);
			FromMaterials = Node->GetChildNodes();

			for (int ii = 0; ii < FromMaterials->GetCount(); ii++)
			{
				Node = FromMaterials->Get(ii);
				AnsiString FromStr = Normalize(Node->GetNodeName());
				ToMaterials = Node->GetChildNodes();

				for (int jj = 0; jj < ToMaterials->GetCount() ; jj++)
				{
					Node = ToMaterials->Get(jj);
					AnsiString ToStr = Normalize(Node->GetNodeName());
					SetCell(FromStr, ToStr, Node);
				}
			}
			
			m_initialized = true;
		}
		catch(...)
		{
			//Raised if the XML file isn't available or its structure is damaged.
			throw EQException("XML : Unknown error during init");
		}
	}

	~CMaterialsMatrix()
	{
		for( TMatrixMap::const_iterator iter = m_theMatrix.begin(); iter != m_theMatrix.end(); ++iter )
		{
			if (NULL != iter->second)
				delete iter->second;
		}	
		m_theMatrix.clear();	
	}
};

#endif
