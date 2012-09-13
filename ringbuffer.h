#pragma once
#include <cstdio>
#include <sstream>
#include <iostream>

template <class T>
class C_RingBuffer
{
	protected:
		size_t m_Size;
		size_t m_Used;
		T* m_Buffer;
		T* m_Cursor;
		bool m_Full;
	public:
		C_RingBuffer(size_t size=3);
		~C_RingBuffer();
		void M_Add(const T& val);
		bool M_Full() const
		{
			return m_Full;
		}
		T M_Median() const;

		friend std::ostream& operator<<(std::ostream& o, const C_RingBuffer& rb)
		{
			o << "RING[ ";
			for(size_t i=0; i<rb.m_Size; ++i)
			{
				bool cursor=false;
				if(&rb.m_Buffer[i] == rb.m_Cursor)
				{
					o << "(";
					cursor=true;
				}
				if(rb.m_Buffer[i]) o << rb.m_Buffer[i];
				else o << "~";
				if(cursor) o << ")";
				o << " ";
			}
			o << " ]";
			return o;
		}
};

template <class T>
C_RingBuffer<T>::C_RingBuffer(size_t size) : m_Size(size), m_Used(0), m_Buffer(NULL), m_Cursor(NULL)
{
	m_Buffer = new T[m_Size];
}

template <class T>
C_RingBuffer<T>::~C_RingBuffer()
{
	delete[] m_Buffer;
}

template <class T>
void C_RingBuffer<T>::M_Add(const T& val)
{
	if(!m_Cursor)
	{
		m_Cursor=m_Buffer;
		*m_Cursor=val;
		if(m_Used!=m_Size) m_Used++;
	}
	else
	{
		if(m_Used!=m_Size) m_Used++;
		m_Cursor++;
		if(m_Cursor-m_Buffer >= (int)m_Size)
		{
			m_Cursor=m_Buffer;
			m_Full=true;
		}
		*m_Cursor=val;
	}
}

template <class T>
T C_RingBuffer<T>::M_Median() const
{
	T ret;
	memset(&ret, 0, sizeof(T));
	for(size_t i=0; i<m_Used; ++i)
	{
		ret+=m_Buffer[i];
	}
	if(m_Used==0) return 0;
	return ret/m_Used;
}

template <class T>
class C_Extrapolator : public C_RingBuffer<T>
{
	private:
		using C_RingBuffer<T>::m_Buffer;
		using C_RingBuffer<T>::m_Size;
		using C_RingBuffer<T>::m_Cursor;
		bool m_Prev;
		T m_PrevVal;
		T m_Current;
	public:
		C_RingBuffer<T> m_Hops;
		C_Extrapolator(size_t size=4) : C_RingBuffer<T>(size), m_Prev(false), m_Hops(size-1) {}
		void M_Add(const T& val);
		T M_ExtrapolateValue() const;
		T M_Current() const { return m_Current; }
};

template <class T>
void C_Extrapolator<T>::M_Add(const T& val)
{
	m_Current=val;
	C_RingBuffer<T>::M_Add(val);
	if(m_Prev) m_Hops.M_Add(val - m_PrevVal);
	m_Prev=true;
	m_PrevVal=val;
}

template <class T>
T C_Extrapolator<T>::M_ExtrapolateValue() const
{
	return *m_Cursor + m_Hops.M_Median();
}
