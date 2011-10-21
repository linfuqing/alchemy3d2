#pragma once

#include "AlchemyVector.h"

namespace alchemy
{
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

		CString<CHARACTER>& operator+=(const CString& String)
		{
			if(String.GetLength() == 0)
				return *this;

			m_bDirty = true;

			typename CVector<CHARACTER>::CConstantIterator Iterator = String.GetBeginIterator();

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

			typename CVector<CHARACTER>::CConstantIterator SourceIterator = CVector<CHARACTER>::GetBeginIterator(), TargetIterator = String.GetBeginIterator();

			while( CVector<CHARACTER>::IsValuable(SourceIterator) && CVector<CHARACTER>::IsValuable(TargetIterator) )
			{
				if(*SourceIterator != *TargetIterator)
					return false;

				++ SourceIterator;
				++ TargetIterator;
			}

			return !(CVector<CHARACTER>::IsValuable(SourceIterator) || CVector<CHARACTER>::IsValuable(TargetIterator));
		}

		bool operator==(const CHARACTER* pcString)const
		{
			if(!pcString)
				return false;

			typename CVector<CHARACTER>::CConstantIterator Iterator = CVector<CHARACTER>::GetBeginIterator();

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

		/*void Assign(const CHARACTER* pcString)
		{
			CPool<CHARACTER>::Reset();
			Append(pcString);
		}

		void Assign(const CHARACTER cChar)
		{
			CPool<CHARACTER>::Reset();
			Append(cChar);
		}

		void Append(const CHARACTER* pcString)
		{
			m_bDirty = true;

			while(*pcString)
			{
				CPool<CHARACTER>::Add(*pcString);

				pcString ++;
			}
		}

		void Append(const CHARACTER cChar)
		{
			m_bDirty = true;

			CPool<CHARACTER>::Add(cChar);
		}*/

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

				ITERATOR Iterator = CVector<CHARACTER>::GetBeginIterator();

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
	private:
		CHARACTER* m_pcBuffer;
		UINT m_uBufferLength;
		bool m_bDirty;
	};

	typedef CString<CHAR> STRING;

	STRING& operator+=(STRING& String, UINT uNumber);
	STRING& operator+=(STRING& String, INT  nNumber);
}