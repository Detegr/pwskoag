#pragma once
#include <cstdio>
#include <sstream>

template <class T>
class C_RingBuffer
{
	private:
		size_t m_Size;
		T* m_Buffer;
		T* m_Cursor;
	public:
		C_RingBuffer(size_t size=5);
		~C_RingBuffer();
		void M_Add(const T& val);
		T M_Median() const;

		friend std::ostream& operator<<(std::ostream& o, const C_RingBuffer& rb)
		{
			o << "RING[ ";
			for(int i=0; i<rb.m_Size; ++i)
			{
				bool cursor=false;
				if(&rb.m_Buffer[i] == rb.m_Cursor)
				{
					o << "(";
					cursor=true;
				}
				if(rb.m_Buffer[i]) o << rb.m_Buffer[i];
				else o << "~0";
				if(cursor) o << ")";
				o << " ";
			}
			o << " ]";
			return o;
		}
};

template <class T>
C_RingBuffer<T>::C_RingBuffer(size_t size) : m_Size(size), m_Buffer(NULL), m_Cursor(NULL)
{
	m_Buffer = new T[m_Size];
	m_Cursor=m_Buffer;
}

template <class T>
C_RingBuffer<T>::~C_RingBuffer()
{
	delete[] m_Buffer;
}

template <class T>
void C_RingBuffer<T>::M_Add(const T& val)
{
	*m_Cursor=val;
	m_Cursor++;
	if(m_Cursor-m_Buffer+1 > m_Size) m_Cursor=m_Buffer;
}

template <class T>
T C_RingBuffer<T>::M_Median() const
{
	T ret;
	T* c=m_Buffer;
	size_t s=0;
	for(size_t i=0; i<m_Size; ++i)
	{
		if(m_Buffer[i])
		{
			ret+=m_Buffer[i];
			s++;
		}
	}
	return ret/s;
}
