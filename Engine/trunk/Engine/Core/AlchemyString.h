#pragma once

#include "AlchemyVector.h"

namespace alchemy
{
#define ALCHEMY_STRING_LENGTH         StringLength
#define ALCHEMY_STRING_FIND           CURRENTBASICSTRINGTYPE::StringFind
#define ALCHEMY_STRING_FLOAT          CURRENTBASICSTRINGTYPE::StringFloat
#define ALCHEMY_STRING_INTEGER        CURRENTBASICSTRINGTYPE::StringInteger
#define ALCHEMY_STRING_UNSIGN_INTEGER CURRENTBASICSTRINGTYPE::StringUnsignInteger
	
	template<typename CHARACTER>
	UINT StringLength(const CHARACTER* pcString)
	{	
		UINT uLength = 0;
		while( *(pcString ++) )
			++ uLength;
		
		return uLength;
	}
	
	//UINT StringLength(const CHAR* pcString);
	//bool StringFind(UINT& uOutputIndex, const CHAR* pcString, CHAR cCharacter);
	//FLOAT StringFloat(const CHAR* pcString);
	//INT StringInteger(const CHAR* pcString);
	//UINT StringUnsignInteger(const CHAR* pcString);
	
	template<typename CHARACTER>
	class CString :
		private CVector<CHARACTER>
	{
		typedef typename CVector<CHARACTER>::CConstantIterator ITERATOR;
	public:
		CString(void) :
		  m_pcBuffer(ALCHEMY_NULL),
			  m_uBufferLength(0),
			  m_bDirty(false)
		{
		}

		  CString(const CString& String) : 
		m_pcBuffer(ALCHEMY_NULL),
			m_uBufferLength(0),
			m_bDirty(false)
		{
			*this = String;
		}

		  CString(const CHARACTER* pcString) : 
		m_pcBuffer(ALCHEMY_NULL),
			  m_uBufferLength(0),
			  m_bDirty(false)
		{
			*this = pcString;
		}

		 CString(const CHARACTER cChar) :
		 m_pcBuffer(ALCHEMY_NULL),
			  m_uBufferLength(0),
			  m_bDirty(false)
		{
			*this = cChar;
		}

		~CString(void)
		{
			ALCHEMY_DEBUG_DELETE_ARRAY(m_pcBuffer);
		}

		ALCHEMY_INLINE UINT GetLength()const
		{
			return CVector<CHARACTER>::GetLength();
		}
		
		ALCHEMY_INLINE const CHARACTER& operator[](UINT uIndex)const
		{
			return ( *static_cast<const CVector<CHARACTER>* const> (this) )[uIndex];
		}
		
		ALCHEMY_INLINE CHARACTER& operator[](UINT uIndex)
		{
			return ( *static_cast<CVector<CHARACTER>* const> (this) )[uIndex];
		}

		CString<CHARACTER>& operator+=(const CString& String)
		{
			if(String.GetLength() == 0)
				return *this;

			m_bDirty = true;

			typename CVector<CHARACTER>::CConstantIterator Iterator = String.GetIterator(0);

			while( CVector<CHARACTER>::IsValuable(Iterator) )
			{
				CVector<CHARACTER>::Add(*Iterator);

				++ Iterator;
			}

			return *this;
		}

		CString<CHARACTER>& operator+=(const CHARACTER* pcString)
		{
			if(!pcString)
				return *this;

			m_bDirty = true;

			while(*pcString)
			{
				CVector<CHARACTER>::Add(*pcString);

				pcString ++;
			}

			return *this;
		}

		CString<CHARACTER>& operator+=(const CHARACTER cChar)
		{
			if(!cChar)
				return *this;

			m_bDirty = true;

			CVector<CHARACTER>::Add(cChar);

			return *this;
		}
		
		CString<CHARACTER>& operator-=(const CString& String)
		{
			UINT uIndex;
			if( GetIndexOf(uIndex, String) )
				CVector<CHARACTER>::Remove( uIndex, String.GetLength() );
			
			return *this;
		}
		
		CString<CHARACTER>& operator-=(const CHARACTER* pcString)
		{
			UINT uIndex, uLength = ALCHEMY_STRING_LENGTH(pcString);
			if( CVector<CHARACTER>::GetIndexOf(uIndex, pcString, uLength) )
				CVector<CHARACTER>::Remove(uIndex, uLength);
			
			return *this;
		}
		
		CString<CHARACTER>& operator-=(const CHARACTER cChar)
		{
			UINT uIndex;
			if( GetIndexOf(uIndex, cChar) )
				CVector<CHARACTER>::Remove(uIndex, 1);
			
			return *this;
		}

		CString<CHARACTER>& operator=(const CString& String)
		{
			CVector<CHARACTER>::Reset();
			return (*this) += String;
		}

		CString<CHARACTER>& operator=(const CHARACTER* pcString)
		{
			CVector<CHARACTER>::Reset();
			return (*this) += pcString;
		}

		CString<CHARACTER>& operator=(const CHARACTER cChar)
		{
			CVector<CHARACTER>::Reset();
			return (*this) += cChar;
		}

		CString<CHARACTER> operator+(const CString& String)const
		{
			CString Result = *this;

			return Result += String;
		}

		CString<CHARACTER> operator+(const CHARACTER* pcString)const
		{
			CString Result = *this;

			return Result += pcString;
		}

		CString<CHARACTER> operator+(const CHARACTER cChar)const
		{
			CString Result = *this;

			return Result += cChar;
		}

		bool operator==(const CString& String)const
		{
			if( CVector<CHARACTER>::GetLength() != String.GetLength() )
				return false;

			typename CVector<CHARACTER>::CConstantIterator SourceIterator = CVector<CHARACTER>::GetIterator(0), TargetIterator = String.GetIterator(0);

			while( CVector<CHARACTER>::IsValuable(SourceIterator) && CVector<CHARACTER>::IsValuable(TargetIterator) )
			{
				if(*SourceIterator != *TargetIterator)
					return false;

				++ SourceIterator;
				++ TargetIterator;
			}

			return !( CVector<CHARACTER>::IsValuable(SourceIterator) || CVector<CHARACTER>::IsValuable(TargetIterator) );
		}

		bool operator==(const CHARACTER* pcString)const
		{
			if(!pcString)
				return false;

			typename CVector<CHARACTER>::CConstantIterator Iterator = CVector<CHARACTER>::GetIterator(0);

			while( CVector<CHARACTER>::IsValuable(Iterator) && *pcString)
			{
				if(*Iterator != *pcString ++)
					return false;

				++ Iterator;
			}

			return !(CVector<CHARACTER>::IsValuable(Iterator) || *pcString);
		}

		bool operator==(const CHARACTER cChar)const
		{
			CHARACTER* pChar = CVector<CHARACTER>::Get(0);

			return pChar && *pChar == cChar && CVector<CHARACTER>::GetLength() == 1;
		}

		operator const CHARACTER*()
		{
			if(m_bDirty)
			{
				UINT uLength = CVector<CHARACTER>::GetLength() + 1;
				if(m_uBufferLength < uLength)
				{
					m_uBufferLength = uLength;

					ALCHEMY_DEBUG_DELETE_ARRAY(m_pcBuffer);

					ALCHEMY_DEBUG_NEW(m_pcBuffer, CHARACTER[m_uBufferLength]);
				}

				ITERATOR Iterator = CVector<CHARACTER>::GetIterator(0);

				uLength = 0;
				while( CVector<CHARACTER>::IsValuable(Iterator) )
				{
					m_pcBuffer[uLength ++] = *Iterator;

					++ Iterator;
				}

				m_pcBuffer[uLength] = 0;
			}

			return m_pcBuffer ? m_pcBuffer : ALCHEMY_NULL;
		}
		
		bool IsEqual(const CString& String, UINT uSourceFrom, UINT uTargetFrom, UINT uLength = 0)const
		{
			if( CVector<CHARACTER>::GetLength() < (uSourceFrom + uLength) || String.GetLength() < (uTargetFrom + uLength) )
				return false;
			
			uLength = uLength ? uLength : String.GetLength();
			typename CVector<CHARACTER>::CConstantIterator SourceIterator = CVector<CHARACTER>::GetIterator(uSourceFrom), TargetIterator = String.GetIterator(uTargetFrom);
			
			for(UINT i = 0; i < uLength; ++ i)
			{
				if(*SourceIterator != *TargetIterator)
					return false;
				
				++ SourceIterator;
				++ TargetIterator;
			}
			
			return true;
		}
		
		bool IsEqual(const CHARACTER* pcString, UINT uSourceFrom, UINT uLength = 0)const
		{
			if( CVector<CHARACTER>::GetLength() < (uSourceFrom + uLength) )
				return false;
			
			uLength = uLength ? uLength : (CVector<CHARACTER>::GetLength() - uSourceFrom);
			typename CVector<CHARACTER>::CConstantIterator SourceIterator = CVector<CHARACTER>::GetIterator(uSourceFrom);
			
			for(UINT i = 0; i < uLength; ++ i)
			{
				if( *SourceIterator != *pcString && !(*pcString) )
					return false;
				
				++ SourceIterator;
				++ pcString;
			}
			
			return true;
		}
		
		/*template<typename T>
		bool IsEqual(const T* pcString, UINT uSourceFrom, UINT uTargetFrom, UINT uLength)const
		{
			if( CVector<CHARACTER>::GetLength() < (uSourceFrom + uLength) )
				return false;
			
			typename CVector<CHARACTER>::CConstantIterator SourceIterator = CVector<CHARACTER>::GetIterator(uSourceFrom);
			
			pcString += uTargetFrom;
			
			for(UINT i = 0; i < uLength; ++ i)
			{
				if( *SourceIterator != static_cast<CHAR> (*pcString) && !(*pcString) )
					return false;
				
				++ SourceIterator;
				++ pcString;
			}
			
			return true;
		}*/

		void Assign(const CHARACTER* pcString, UINT uFrom, UINT uLength)
		{
			CVector<CHARACTER>::Reset();
			Append(pcString, uFrom, uLength);
		}

		void Assign(const CString& String, UINT uFrom, UINT uLength)
		{
			CVector<CHARACTER>::Reset();
			Append(String, uFrom, uLength);
		}

		void Append(const CHARACTER* pcString, UINT uFrom, UINT uLength)
		{
			m_bDirty = true;

			pcString += uFrom;

			if(uLength)
			{
				while(*pcString && uLength --)
				{
					CVector<CHARACTER>::Add(*pcString);

					pcString ++;
				}
			}
			else
			{
				while(*pcString)
				{
					CVector<CHARACTER>::Add(*pcString);

					pcString ++;
				}
			}
		}

		void Append(const CString& String, UINT uFrom, UINT uLength)
		{
			m_bDirty = true;

			typename CVector<CHARACTER>::CConstantIterator Iterator = String.GetIterator(uFrom);

			if(uLength)
			{
				while(CVector<CHARACTER>::IsValuable(Iterator) && uLength --)
				{
					CVector<CHARACTER>::Add(*Iterator);

					++ Iterator;
				}
			}
			else
			{
				while( CVector<CHARACTER>::IsValuable(Iterator) )
				{
					CVector<CHARACTER>::Add(*Iterator);

					++ Iterator;
				}
			}
		}

		CString<CHARACTER> GetSubString(UINT uStart, UINT uLength = 0) const
		{
			uLength = uLength ? uLength : CVector<CHARACTER>::GetLength();
			CString<CHARACTER> String;
			String.Append(*this, uStart, uLength);
			return String;
		}

		UINT GetLastIndexOf(CHARACTER cCharacter)const
		{
			for (INT i=CVector<CHARACTER>::GetLength()-1; i>=0; --i)
			{
				if( cCharacter ==  *CVector<CHARACTER>::Get(i) )
					return i;
			}

			return CVector<CHARACTER>::GetLength();
		}
		
		UINT GetIndexOf(const CHARACTER cCharacter, UINT uFrom = 0, UINT uTo = 0)const
		{
			uTo = uTo ? uTo : CVector<CHARACTER>::GetLength();
			
			typename CVector<CHARACTER>::CConstantIterator Iterator = CVector<CHARACTER>::GetIterator(uFrom);
			
			while(CVector<CHARACTER>::IsValuable(Iterator) && Iterator.GetIndex() < uTo)
			{
				if(cCharacter == *Iterator)
					return Iterator.GetIndex();
				
				++ Iterator;
			}
			
			return uTo;
		}

		UINT GetIndexOf(const CHARACTER* pcString, UINT uFrom = 0, UINT uTo = 0)const
		{
			uTo = uTo ? uTo : CVector<CHARACTER>::GetLength();
			if( !pcString || !(*pcString) )
				return uTo;

			typename CVector<CHARACTER>::CConstantIterator Iterator = CVector<CHARACTER>::GetIterator(uFrom), CurrentIterator;
			const CHARACTER* pcCharacter;

			while(CVector<CHARACTER>::IsValuable(Iterator) && Iterator.GetIndex() < uTo)
			{
				if(*pcString == *Iterator)
				{
					CurrentIterator = Iterator;
					pcCharacter  = pcString;
					while( CVector<CHARACTER>::IsValuable(++ CurrentIterator) && * (++ pcCharacter) )
					{
						if(*pcCharacter != *CurrentIterator)
							break;
					}

					if( !(*pcCharacter) )
						return Iterator.GetIndex();
				}

				++ Iterator;
			}

			return uTo;
		}

		UINT GetIndexOf(const CString& String, UINT uFrom = 0, UINT uTo = 0)const
		{
			uTo = uTo ? uTo : CVector<CHARACTER>::GetLength();
			if( !String.GetLength() )
				return uTo;

			typename CVector<CHARACTER>::CConstantIterator Iterator = CVector<CHARACTER>::GetIterator(uFrom), CurrentIterator, TargetIterator;
			const CHARACTER cFistCharacter = String[0];

			while(CVector<CHARACTER>::IsValuable(Iterator) && Iterator.GetIndex() < uTo)
			{
				if(cFistCharacter == *Iterator)
				{
					CurrentIterator = Iterator;
					TargetIterator  = String.GetIterator(0);
					while( CVector<CHARACTER>::IsValuable(++ CurrentIterator) && CVector<CHARACTER>::IsValuable(++ TargetIterator) )
					{
						if(*TargetIterator != *CurrentIterator)
							break;
					}

					if( !CVector<CHARACTER>::IsValuable(TargetIterator) )
						return Iterator.GetIndex();
				}

				++ Iterator;
			}

			return uTo;
		}

		void Replace(UINT uSourceFrom, UINT uSourceLength, const CHARACTER* pcDestination)
		{
			UINT uLength = GetLength();
			if(uSourceFrom < uLength)
			{
				UINT uCurrentIndex = uSourceFrom;
				UINT uEnd = ALCHEMY_MININUM(uSourceFrom + uSourceLength, uLength);
				while(uCurrentIndex < uEnd && *pcDestination)
				{
					CVector<CHARACTER>::Set(*pcDestination, uCurrentIndex ++);

					if(uCurrentIndex == uLength)
						return;

					++ pcDestination;
				}

				if(uCurrentIndex < uEnd)
				{
					UINT uCurrentLength = uEnd - uCurrentIndex;

					for(UINT i = uEnd; i < uLength; ++ i)
						CVector<CHARACTER>::Set( (*this)[i], i - uCurrentLength );

					CVector<CHARACTER>::Remove(uLength - uCurrentLength, uCurrentLength);
				}
				else if(*pcDestination)
				{
					UINT uCurrentLength = 0;
					const CHARACTER* pcString = pcDestination;
					while( *(pcString ++) )
						++ uCurrentLength;

					UINT uFrom   = uLength - uCurrentLength;
					UINT i;
					for(i = uFrom; i < uLength; ++ i)
						CVector<CHARACTER>::Add( (*this)[i] );

					for(i = uFrom; i >= uCurrentIndex; -- i)
						CVector<CHARACTER>::Set( (*this)[i], i + uCurrentLength );

					pcString = pcDestination;
					for(i = uCurrentIndex; i < uCurrentLength; ++ i)
						CVector<CHARACTER>::Set(pcString[i], i);
				}
			}
		}

		void Replace(UINT uSourceFrom, UINT uSourceLength, const CString& Destination)
		{
			UINT uLength = GetLength();
			if(uSourceFrom < uLength)
			{
				UINT uCurrentIndex = uSourceFrom;
				UINT uEnd = ALCHEMY_MININUM(uSourceFrom + uSourceLength, uLength);
				typename CVector<CHARACTER>::CConstantIterator Iterator = Destination.GetIterator(0);
				while( uCurrentIndex < uEnd && CVector<CHARACTER>::IsValuable(Iterator) )
				{
					CVector<CHARACTER>::Set(*Iterator, uCurrentIndex ++);

					if(uCurrentIndex == uLength)
						return;

					++ Iterator;
				}

				if(uCurrentIndex < uEnd)
				{
					UINT uCurrentLength = uEnd - uCurrentIndex;

					for(UINT i = uEnd; i < uLength; ++ i)
						CVector<CHARACTER>::Set( (*this)[i], i - uCurrentLength );

					CVector<CHARACTER>::Remove(uLength - uCurrentLength, uCurrentLength);
				}
				else if( CVector<CHARACTER>::IsValuable(Iterator) )
				{
					UINT uCurrentLength = Destination.GetLength() - Iterator.GetIndex();

					UINT uFrom   = uLength - uCurrentLength;
					UINT i;
					for(i = uFrom; i < uLength; ++ i)
						CVector<CHARACTER>::Add( (*this)[i] );

					for(i = uFrom; i >= uCurrentIndex; -- i)
						CVector<CHARACTER>::Set( (*this)[i], i + uCurrentLength );

					for(i = 0; i < uCurrentLength; ++ i)
					{
						CVector<CHARACTER>::Set(*Iterator, i + uCurrentIndex);

						++ Iterator;
					}
				}
			}
		}

		UINT Replace(const CHARACTER* pcSource, const CHARACTER* pcDestination, UINT uFrom = 0, UINT uTo = 0)
		{
			UINT uIndex = GetIndexOf(pcSource, uFrom, uTo), uLength = uFrom < uTo ? ALCHEMY_MININUM(GetLength(), uTo) : GetLength();
			if(uIndex < uLength)
			{
				UINT uCurrentIndex = uIndex;
				while(*pcSource && *pcDestination)
				{
					CVector<CHARACTER>::Set(*pcDestination, uCurrentIndex ++);

					++ pcSource;
					++ pcDestination;
				}

				if(*pcSource)
				{
					UINT uCurrentLength = 0;
					const CHARACTER* pcString = pcSource;
					while( *(pcString ++) )
						++ uCurrentLength;

					uLength = GetLength();
					for(UINT i = uCurrentIndex + uCurrentLength; i < uLength; ++ i)
						CVector<CHARACTER>::Set( (*this)[i], i - uCurrentLength );

					CVector<CHARACTER>::Remove(uLength - uCurrentLength, uCurrentLength);
				}
				else if(*pcDestination)
				{
					UINT uCurrentLength = 0;
					const CHARACTER* pcString = pcDestination;
					while( *(pcString ++) )
						++ uCurrentLength;

					uLength = GetLength();
					uFrom   = uLength - uCurrentLength;
					UINT i;
					for(i = uFrom; i < uLength; ++ i)
						CVector<CHARACTER>::Add( (*this)[i] );

					for(i = uFrom; i >= uCurrentIndex; -- i)
						CVector<CHARACTER>::Set( (*this)[i], i + uCurrentLength );

					pcString = pcDestination;
					for(i = 0; i < uCurrentLength; ++ i)
						CVector<CHARACTER>::Set(pcString[i], i + uCurrentIndex);
				}

				return uIndex;
			}

			return uLength;
		}

		UINT Replace(const CString& Source, const CString& Destination, UINT uFrom = 0, UINT uTo = 0)
		{
			UINT uIndex = GetIndexOf(Source, uFrom, uTo), uLength = uFrom < uTo ? ALCHEMY_MININUM(GetLength(), uTo) : GetLength();
			if(uIndex < uLength)
			{
				UINT uCurrentIndex = uIndex;
				typename CVector<CHARACTER>::CConstantIterator SourceIterator = Source.GetIterator(0), DestinationIterator = Destination.GetIterator(0);
				while( CVector<CHARACTER>::IsValuable(SourceIterator) && CVector<CHARACTER>::IsValuable(DestinationIterator) )
				{
					CVector<CHARACTER>::Set(*DestinationIterator, uCurrentIndex ++);

					++ SourceIterator;
					++ DestinationIterator;
				}

				if( CVector<CHARACTER>::IsValuable(SourceIterator) )
				{
					UINT uCurrentLength = Source.GetLength() - SourceIterator.GetIndex();

					uLength = GetLength();
					for(UINT i = uCurrentIndex + uCurrentLength; i < uLength; ++ i)
						CVector<CHARACTER>::Set( (*this)[i], i - uCurrentLength );

					CVector<CHARACTER>::Remove(uLength - uCurrentLength, uCurrentLength);
				}
				else if( CVector<CHARACTER>::IsValuable(DestinationIterator) )
				{
					UINT uCurrentLength = Destination.GetLength() - DestinationIterator.GetIndex();

					uLength = GetLength();
					uFrom   = uLength - uCurrentLength;
					UINT i;
					for(i = uFrom; i < uLength; ++ i)
						CVector<CHARACTER>::Add( (*this)[i] );

					for(i = uFrom; i >= uCurrentIndex; -- i)
						CVector<CHARACTER>::Set( (*this)[i], i + uCurrentLength );

					for(i = 0; i < uCurrentLength; ++ i)
					{
						CVector<CHARACTER>::Set(*DestinationIterator, i + uCurrentIndex);

						++ DestinationIterator;
					}
				}

				return uIndex;
			}

			return uLength;
		}
		
		void Split(const CString& Limit, CVector<CString>& Output, CVector<UINT>* pOutputIndices = ALCHEMY_NULL)const
		{
			UINT uEnd, uBegin = 0, uIndex = GetIndexOf(Limit, 0), uLength = CVector<CHARACTER>::GetLength(), uLimitLength = Limit.GetLength();
			CString String;
			if(pOutputIndices)
			{
				while(uIndex < uLength)
				{
					uEnd = uIndex - uBegin;
					if( !IsEqual(Limit, uBegin, 0, uEnd) )
					{
						String.Assign(*this, uBegin, uEnd);
					
						Output.Add(String);
						pOutputIndices->Add(uIndex);
					}
					
					uBegin = uIndex + uLimitLength;
					uIndex = GetIndexOf(Limit, uBegin);
				}
			}
			else
			{
				while(uIndex < uLength)
				{
					uEnd = uIndex - uBegin;
					if( !IsEqual(Limit, uBegin, 0, uEnd) )
					{
						String.Assign(*this, uBegin, uEnd);
						Output.Add(String);
					}
					
					uBegin = uIndex + uLimitLength;
					uIndex = GetIndexOf(Limit, uBegin);
				}
			}
			
			uEnd = uLength - uBegin;
			if( !IsEqual(Limit, uBegin, 0, uEnd) )
			{
				String.Assign(*this, uBegin, uEnd);
				Output.Add(String);
			}
		}
		
		void Join(const CVector<CString>& SubStrings, const CString& Limit)
		{
			typename CVector<CString>::CConstantIterator Iterator = SubStrings.GetIterator(0);
			if( CVector<CString>::IsValuable(Iterator) )
			{
				while(true)
				{
					*this += *Iterator;
					
					++ Iterator;
					
					if( CVector<CString>::IsValuable(Iterator) )
						*this += Limit;
					else
						break;
				}
			}
		}
		
		//template<typename T>
		void CopyTo(CHARACTER* pcString)const
		{
			typename CVector<CHARACTER>::CConstantIterator Iterator = CVector<CHARACTER>::GetIterator(0);
			while( CVector<CHARACTER>::IsValuable(Iterator) )
			{
				*(pcString ++) = *Iterator;
				
				++ Iterator;
			}
			
			*pcString = 0;
		}
	private:
		CHARACTER* m_pcBuffer;
		UINT m_uBufferLength;
		bool m_bDirty;
	};
	
	template<bool bIsBasicChar, typename TrueType, typename FalseType> 
	struct BASICSTRINGTYPE
	{
		typedef CString<TrueType>  BASICSTRING; 
		typedef BASICSTRING        STRING     ;
		
		friend BASICSTRING& operator+=(BASICSTRING& String, UINT uNumber)
		{
			if(uNumber)
			{
				UINT uDigital = Digital(uNumber), uTemp = 0, uCurrent, uMedian, i, j;
				
				for(i = uDigital; i > 0; -- i)
				{
					uMedian = 1;
					for(j = 1; j < i; ++ j)
						uMedian *= 10;
					
					uCurrent = (uNumber - uTemp) / uMedian;
					String += static_cast<const char>(uCurrent + '0');
					
					uTemp += uCurrent * uMedian;
				}
			}
			else
				String += ALCHEMY_TEXT('0');
			
			return String;
		}
		
		friend BASICSTRING& operator+=(BASICSTRING& String, INT  nNumber)
		{
			if(nNumber)
			{
				if(nNumber < 0)
				{
					String += '-';
					
					nNumber = - nNumber;
				}
				
				UINT uDigital = Digital(nNumber), uTemp = 0, uCurrent, uMedian, i, j;
				
				for(i = uDigital; i > 0; -- i)
				{
					uMedian = 1;
					for(j = 1; j < i; ++ j)
						uMedian *= 10;
					
					uCurrent = (nNumber - uTemp) / uMedian;
					String += static_cast<const char>(uCurrent + '0');
					
					uTemp += uCurrent * uMedian;
				}
			}
			else
				String += ALCHEMY_TEXT('0');
			
			return String;
		}
		
		static bool StringFind(UINT& uOutputIndex, const TrueType* pcString, TrueType cCharacter)
		{
			UINT uIndex = 0;
			while(*pcString)
			{
				if(*pcString == cCharacter)
				{
					uOutputIndex = uIndex;
					
					return true;
				}
				
				++ uIndex;
				++ pcString;
			}
			
			uOutputIndex = uIndex;
			
			return false;
		}
		
		static FLOAT StringFloat(const TrueType* pcString, const TrueType* pcStepCharacters = ALCHEMY_NULL, const TrueType** ppcEnd = ALCHEMY_NULL)
		{
			FLOAT fResult = 0.0f, fDigital = 10.0f, fIsPositive = 1;
			CHAR cTemp, cStepCharacter;
			UINT i;
			
			while( (cTemp = *pcString) != 0 && (cTemp > '9' || cTemp < '0') )
			{
				if(cTemp == '-')
					fIsPositive = - 1;
				else if(!pcStepCharacters)
					fIsPositive = 1;
				else
				{
					for(i = 0; cStepCharacter = pcStepCharacters[i]; ++ i)
					{
						if(cTemp == cStepCharacter)
							break;
					}
					
					if(!cStepCharacter)
						fIsPositive = 1;
				}
					
				++ pcString;
			}
			
			while( (cTemp = *pcString) != 0 )
			{
				if(cTemp <= '9' && cTemp >= '0')
				{
					if(fDigital > 1.0f)
					{
						fResult *= fDigital;
						fResult += (cTemp - '0');
					}
					else
						fResult += (cTemp - '0') * fDigital;
				}
				else if(cTemp == '.' && fDigital > 1.0f)
					fDigital = 1.0f;
				else if(!pcStepCharacters)
					break;
				else
				{
					for(i = 0; cStepCharacter = pcStepCharacters[i]; ++ i)
					{
						if(cTemp == cStepCharacter)
							break;
					}
					
					if(!cStepCharacter)
						break;
				}
				
				if(fDigital < 10.0f)
					fDigital *= 0.1f;
				
				++ pcString;
			}
			
			if(ppcEnd)
				*ppcEnd = pcString;
			
			return fResult * fIsPositive;
		}
		
		static INT StringInteger(const TrueType* pcString)
		{
			INT nResult = 0, nDigital = 1, nIsPositive = 1;
			UINT uIndex;
			CHAR cTemp;
			if( StringFind(uIndex, pcString, '-') )
			{
				pcString += uIndex;
				
				nIsPositive = - 1;
			}
			
			for(UINT i = ALCHEMY_STRING_LENGTH(pcString) - 1; i > 0; -- i)
			{
				cTemp = pcString[i];
				if(cTemp >= '0' && cTemp <= '9')
				{
					nResult += nDigital * (cTemp - '0');
					
					nDigital *= 10;
				}
			}
			
			cTemp = *pcString;
			if(cTemp >= '0' && cTemp <= '9')
				nResult += nDigital * (cTemp - '0');
			
			return nResult * nIsPositive;
		}
		
		static UINT StringUnsignInteger(const TrueType* pcString)
		{
			UINT uResult = 0, uDigital = 1;
			CHAR cTemp;
			
			for(UINT i = ALCHEMY_STRING_LENGTH(pcString) - 1; i > 0; -- i)
			{
				cTemp = pcString[i];
				if(cTemp >= '0' && cTemp <= '9')
				{
					uResult += uDigital * (cTemp - '0');
					
					uDigital *= 10;
				}
			}
			
			cTemp = *pcString;
			if(cTemp >= '0' && cTemp <= '9')
				uResult += uDigital * (cTemp - '0');
			
			return uResult;
		}
		
		static FLOAT StringFloat(const BASICSTRING& String)
		{
			FLOAT fResult = 0.0f, fDigital = 1.0f, fIsPositive = 1.0f;
			CHAR cTemp;
			UINT uIndex = String.GetIndexOf('-'), uLength = String.GetLength(), i;
			
			if(uIndex < uLength)
				fIsPositive = - 1.0f;
			else 
				uIndex = 0;
			
			uIndex = uIndex + String.GetIndexOf('.');
			if(uIndex < uLength)
			{
				for(i = uIndex - 1; i > 0; -- i)
				{
					cTemp = String[i];
					if(cTemp >= '0' && cTemp <= '9')
					{
						fResult += fDigital * (cTemp - '0');
						
						fDigital *= 10.0f;
					}
				}
				
				cTemp = String[0];
				if(cTemp >= '0' && cTemp <= '9')
					fResult += fDigital * (cTemp - '0');
				
				fDigital = 0.1f;
				
				for(i = uIndex + 1; i < uLength; ++ i)
				{
					cTemp = String[i];
					if(cTemp >= '0' && cTemp <= '9')
					{
						fResult += fDigital * (cTemp - '0');
						
						fDigital *= 0.1f;
					}
				}
			}
			else
			{
				for(i = uIndex - 1; i > 0; -- i)
				{
					cTemp = String[i];
					if(cTemp >= '0' && cTemp <= '9')
					{
						fResult += fDigital * (cTemp - '0');
						
						fDigital *= 10.0f;
					}
				}
				
				cTemp = String[0];
				if( cTemp >= '0' && cTemp <= '9')
					fResult += fDigital * (cTemp - '0');
			}
			
			return fResult * fIsPositive;
			
		}
		
		static INT StringInteger(const BASICSTRING& String)
		{
			INT nResult = 0, nDigital = 1, nIsPositive = 1;
			UINT uIndex = String.GetIndexOf('-'), uLength = String.GetLength();
			CHAR cTemp;
			if(uIndex < uLength)
				nIsPositive = - 1;
			else
				uIndex = 0;
			
			for(UINT i = uLength - uIndex - 1; i > 0; -- i)
			{
				cTemp = String[i];
				if(cTemp >= '0' && cTemp <= '9')
				{
					nResult += nDigital * (cTemp - '0');
					
					nDigital *= 10;
				}
			}
			
			cTemp = String[0];
			if(cTemp >= '0' && cTemp <= '9')
				nResult += nDigital * (cTemp - '0');
			
			return nResult * nIsPositive;
		}
		
		static UINT StringUnsignInteger(const BASICSTRING& String)
		{
			UINT uResult = 0, uDigital = 1;
			CHAR cTemp;
			
			for(UINT i = String.GetLength() - 1; i > 0; -- i)
			{
				cTemp = String[i];
				if(cTemp >= '0' && cTemp <= '9')
				{
					uResult += uDigital * (cTemp - '0');
					
					uDigital *= 10;
				}
			}
			
			cTemp = String[0];
			if(cTemp >= '0' && cTemp <= '9')
				uResult += uDigital * (cTemp - '0');
			
			return uResult;
		}
	};
	
	template<typename TrueType, typename FalseType> 
	struct BASICSTRINGTYPE<false, TrueType, FalseType>
	{
		typedef BASICSTRINGTYPE<true, TrueType, FalseType> TRUEBASICSTRINGTYPE;
		typedef typename TRUEBASICSTRINGTYPE::BASICSTRING  BASICSTRING;
		typedef CString<FalseType>                         STRING     ;

		ALCHEMY_INLINE static FLOAT StringFloat(const TrueType* pcString, const TrueType* pcStepCharacters, const TrueType** ppcEnd)
		{
			return TRUEBASICSTRINGTYPE::StringFloat(pcString, pcStepCharacters, ppcEnd);
		}

		ALCHEMY_INLINE static FLOAT StringFloat(const BASICSTRING& String)
		{
			return TRUEBASICSTRINGTYPE::StringFloat(String);
		}

		ALCHEMY_INLINE static INT StringInteger(const BASICSTRING& String)
		{
			return TRUEBASICSTRINGTYPE::StringInteger(String);
		}

		ALCHEMY_INLINE static UINT StringUnsignInteger(const BASICSTRING& String)
		{
			return TRUEBASICSTRINGTYPE::StringUnsignInteger(String);
		}
		
		friend STRING& operator+=(STRING& String, UINT uNumber)
		{
			if(uNumber)
			{
				UINT uDigital = Digital(uNumber), uTemp = 0, uCurrent, uMedian, i, j;
				
				for(i = uDigital; i > 0; -- i)
				{
					uMedian = 1;
					for(j = 1; j < i; ++ j)
						uMedian *= 10;
					
					uCurrent = (uNumber - uTemp) / uMedian;
					String += static_cast<const CHAR>( uCurrent + ALCHEMY_TEXT('0') );
					
					uTemp += uCurrent * uMedian;
				}
			}
			else
				String += ALCHEMY_TEXT('0');
			
			return String;
		}
		
		friend STRING& operator+=(STRING& String, INT  nNumber)
		{
			if(nNumber)
			{
				if(nNumber < 0)
				{
					String += ALCHEMY_TEXT('-');
					
					nNumber = - nNumber;
				}
				
				UINT uDigital = Digital(nNumber), uTemp = 0, uCurrent, uMedian, i, j;
				
				for(i = uDigital; i > 0; -- i)
				{
					uMedian = 1;
					for(j = 1; j < i; ++ j)
						uMedian *= 10;
					
					uCurrent = (nNumber - uTemp) / uMedian;
					String += static_cast<const CHAR>( uCurrent + ALCHEMY_TEXT('0') );
					
					uTemp += uCurrent * uMedian;
				}
			}
			else
				String += ALCHEMY_TEXT('0');
			
			return String;
		}
		
		static bool StringFind(UINT& uOutputIndex, const FalseType* pcString, FalseType cCharacter)
		{
			UINT uIndex = 0;
			while(*pcString)
			{
				if(*pcString == cCharacter)
				{
					uOutputIndex = uIndex;
					
					return true;
				}
				
				++ uIndex;
				++ pcString;
			}
			
			uOutputIndex = uIndex;
			
			return false;
		}
		
		static FLOAT StringFloat(const FalseType* pcString, const FalseType* pcStepCharacters, const FalseType** ppcEnd)
		{
			FLOAT fResult = 0.0f, fDigital = 10.0f, fIsPositive = 1;
			CHAR cTemp, cStepCharacter;
			UINT i;
			
			while( (cTemp = *pcString) != 0 && ( cTemp > ALCHEMY_TEXT('9') || cTemp < ALCHEMY_TEXT('0') ) )
			{
				if(cTemp == '-')
					fIsPositive = - 1;
				else if(!pcStepCharacters)
					fIsPositive = 1;
				else
				{
					for(i = 0; cStepCharacter = pcStepCharacters[i]; ++ i)
					{
						if(cTemp == cStepCharacter)
							break;
					}
					
					if(!cStepCharacter)
						fIsPositive = 1;
				}
				
				++ pcString;
			}
			
			while( (cTemp = *pcString) != 0 )
			{
				if( cTemp <= ALCHEMY_TEXT('9') && cTemp >= ALCHEMY_TEXT('0') )
				{
					if(fDigital > 1.0f)
					{
						fResult *= fDigital;
						fResult += ( cTemp - ALCHEMY_TEXT('0') );
					}
					else
						fResult += ( cTemp - ALCHEMY_TEXT('0') ) * fDigital;
				}
				else if(cTemp == ALCHEMY_TEXT('.') && fDigital > 1.0f)
					fDigital = 1.0f;
				else if(!pcStepCharacters)
					break;
				else
				{
					for(i = 0; cStepCharacter = pcStepCharacters[i]; ++ i)
					{
						if(cTemp == cStepCharacter)
							break;
					}
					
					if(!cStepCharacter)
						break;
				}
				
				if(fDigital < 10.0f)
					fDigital *= 0.1f;
				
				++ pcString;
			}
			
			if(ppcEnd)
				*ppcEnd = pcString;
			
			return fResult * fIsPositive;			
		}
		
		static INT StringInteger(const FalseType* pcString)
		{
			INT nResult = 0, nDigital = 1, nIsPositive = 1;
			UINT uIndex;
			CHAR cTemp;
			if( StringFind( uIndex, pcString, ALCHEMY_TEXT('-') ) )
			{
				pcString += uIndex;
				
				nIsPositive = - 1;
			}
			
			for(UINT i = ALCHEMY_STRING_LENGTH(pcString) - 1; i > 0; -- i)
			{
				cTemp = pcString[i];
				if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
				{
					nResult += nDigital * ( cTemp - ALCHEMY_TEXT('0') );
					
					nDigital *= 10;
				}
			}
			
			cTemp = *pcString;
			if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
				nResult += nDigital * ( cTemp - ALCHEMY_TEXT('0') );
			
			return nResult * nIsPositive;
		}
		
		static UINT StringUnsignInteger(const FalseType* pcString)
		{
			UINT uResult = 0, uDigital = 1;
			CHAR cTemp;
			
			for(UINT i = ALCHEMY_STRING_LENGTH(pcString) - 1; i > 0; -- i)
			{
				cTemp = pcString[i];
				if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
				{
					uResult += uDigital * ( cTemp - ALCHEMY_TEXT('0') );
					
					uDigital *= 10;
				}
			}
			
			cTemp = *pcString;
			if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
				uResult += uDigital * ( cTemp - ALCHEMY_TEXT('0') );
			
			return uResult;
		}
		
		static FLOAT StringFloat(const STRING& String)
		{
			FLOAT fResult = 0.0f, fDigital = 1.0f, fIsPositive = 1.0f;
			CHAR cTemp;
			UINT uIndex = String.GetIndexOf( ALCHEMY_TEXT('-') ), uLength = String.GetLength(), i;
			
			if(uIndex < uLength)
				fIsPositive = - 1.0f;
			else 
				uIndex = 0;
			
			uIndex = uIndex + String.GetIndexOf( ALCHEMY_TEXT('.') );
			if(uIndex < uLength)
			{
				for(i = uIndex - 1; i > 0; -- i)
				{
					cTemp = String[i];
					if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
					{
						fResult += fDigital * (cTemp - ALCHEMY_TEXT('0') );
						
						fDigital *= 10.0f;
					}
				}
				
				cTemp = String[0];
				if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
					fResult += fDigital * ( cTemp - ALCHEMY_TEXT('0') );
				
				fDigital = 0.1f;
				
				for(i = uIndex + 1; i < uLength; ++ i)
				{
					cTemp = String[i];
					if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
					{
						fResult += fDigital * ( cTemp - ALCHEMY_TEXT('0') );
						
						fDigital *= 0.1f;
					}
				}
			}
			else
			{
				for(i = uIndex - 1; i > 0; -- i)
				{
					cTemp = String[i];
					if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
					{
						fResult += fDigital * ( cTemp - ALCHEMY_TEXT('0') );
						
						fDigital *= 10.0f;
					}
				}
				
				cTemp = String[0];
				if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
					fResult += fDigital * ( cTemp - ALCHEMY_TEXT('0') );
			}
			
			return fResult * fIsPositive;
			
		}
		
		static INT StringInteger(const STRING& String)
		{
			INT nResult = 0, nDigital = 1, nIsPositive = 1;
			UINT uIndex = String.GetIndexOf( ALCHEMY_TEXT('-') ), uLength = String.GetLength();
			CHAR cTemp;
			if(uIndex < uLength)
				nIsPositive = - 1;
			else
				uIndex = 0;
			
			for(UINT i = uLength - uIndex - 1; i > 0; -- i)
			{
				cTemp = String[i];
				if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
				{
					nResult += nDigital * ( cTemp - ALCHEMY_TEXT('0') );
					
					nDigital *= 10;
				}
			}
			
			cTemp = String[0];
			if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
				nResult += nDigital * ( cTemp - ALCHEMY_TEXT('0') );
			
			return nResult * nIsPositive;
		}
		
		static UINT StringUnsignInteger(const STRING& String)
		{
			UINT uResult = 0, uDigital = 1;
			CHAR cTemp;
			
			for(UINT i = String.GetLength() - 1; i > 0; -- i)
			{
				cTemp = String[i];
				if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
				{
					uResult += uDigital * ( cTemp - ALCHEMY_TEXT('0') );
					
					uDigital *= 10;
				}
			}
			
			cTemp = String[0];
			if( cTemp >= ALCHEMY_TEXT('0') && cTemp <= ALCHEMY_TEXT('9') )
				uResult += uDigital * ( cTemp - ALCHEMY_TEXT('0') );
			
			return uResult;
		}
	};
	
	typedef BASICSTRINGTYPE<ALCHEMY_CONSTANT_IS_SAME_TYPE(char, CHAR), char, CHAR> CURRENTBASICSTRINGTYPE;
	typedef CURRENTBASICSTRINGTYPE::BASICSTRING                                    BASICSTRING, * LPBASICSTRING;
	typedef CURRENTBASICSTRINGTYPE::STRING                                         STRING     , * LPSTRING     ;
	
	BASICSTRING& operator+=(BASICSTRING& String, UINT uNumber);
	BASICSTRING& operator+=(BASICSTRING& String, INT  nNumber);
	
	STRING& operator+=(STRING& String, UINT uNumber);
	STRING& operator+=(STRING& String, INT  nNumber);

	struct STRINGLESS
	{
		bool operator()(const STRING& Left, const STRING& Right)const
		{
			UINT uLeftLength = Left.GetLength();
			UINT uRightLength = Right.GetLength();

			UINT uLen = uLeftLength > uRightLength ? uRightLength : uLeftLength;
			for (UINT i=0; i<uLen; ++i)
			{
				if( Left[i] > Right[i] )
					return true;
				else if( Left[i] < Right[i] )
					return false;
			}

			return uLen == uRightLength;
		}
	};
	
	//STRING& operator+=(STRING& String, UINT uNumber);
	//STRING& operator+=(STRING& String, INT  nNumber);
	
	//STRING& StringAppend(STRING& String, const char* pcString, UINT uFrom, UINT uLength);
	//STRING& StringAssign(STRING& String, const char* pcString, UINT uFrom, UINT uLength);
	
	//FLOAT StringFloat(const STRING& String);
	//INT StringInteger(const STRING& String);
	//UINT StringUnsignInteger(const STRING& String);
}