#include <Winsock2.h>
#include <winsock.h>
// ByteStream.h: interface for the CByteStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BYTESTREAM_H__B009F0DA_3478_4652_B154_260EA0EE14C1__INCLUDED_)
#define AFX_BYTESTREAM_H__B009F0DA_3478_4652_B154_260EA0EE14C1__INCLUDED_

typedef long long int64;
typedef unsigned long long uint64;
typedef wchar_t uchar_t;
typedef std::basic_string<uchar_t> ustring;


enum STREAM_ERROR_CODE
{
	SEC_NO_ERROR = 0,
	SEC_OUT_OF_RANGE,
	SEC_INVALID_STRING,
	SEC_FIXED_STREAM_OVERFLOW,
	SEC_INVALID_CHAR,
    SEC_CHECKSUM_ERROR,
};

struct stream_error
{
	int code;

	stream_error(int c) : code(c)
	{
	}
};

class CBasicStreamBuf
{
protected:
	char*	m_bytes;
	size_t	m_maxlen;
	size_t	m_deflen;
public:
	CBasicStreamBuf()
		:m_bytes(0), m_maxlen(1), m_deflen(0)
	{
	}
	virtual ~CBasicStreamBuf()
	{
	}
	char* GetBytes()const
	{
		return m_bytes;
	}
	size_t Size()const
	{
		return m_deflen;
	}
	size_t Capacity()const
	{
		return m_maxlen;
	}
	size_t SpaceLeft()const
	{
		return m_maxlen - m_deflen;
	}

	void Clear()
	{
		m_deflen=0;
	}
	char* GetBufRange(size_t pos, size_t n)
	{
        size_t np = pos + n;
        if((pos > m_deflen) || (n > m_deflen) || (np > m_deflen))
		{
			return 0;
		}
		else
		{
			return m_bytes+pos;
		}
	}

	virtual void ReAlloc(size_t size) = 0;
	virtual void Free() = 0;

	size_t ExpandTo(size_t size)
	{
		if(size > m_maxlen)
		{
			ReAlloc(size);
		}

		if(m_deflen < size)
			m_deflen = size;

		return size;
	}

	void ChangeSize(size_t size)	//反外挂解密之后长度可能变小，不敢改ExpandTo的实现，新搞一个ChangeSize
	{
		if(size >= m_deflen)
			ExpandTo(size);
		else
			m_deflen = size;
	}

	size_t Get(void* p, size_t pos, size_t n)
	{
		if(n>m_deflen-pos)
		{
			throw stream_error(SEC_OUT_OF_RANGE);
		}
		memcpy(p, m_bytes+pos, n);
		return n;
	}
	size_t GetNoThrow(void* p, size_t pos, size_t n)
	{
		if(n>m_deflen-pos)
		{
			n = m_deflen-pos;
		}
		memcpy(p, m_bytes+pos, n);
		return n;
	}
	size_t Put(const void* p, size_t pos, size_t n)
	{
		size_t total = pos+n;
		ExpandTo(total);
		memcpy(m_bytes+pos, p, n);
		return n;
	}

	size_t Append(const void* p, size_t n)
	{
		size_t len = Size();
		size_t total = len + n;
		ExpandTo(total);
		memcpy(m_bytes + len, p, n);
		return n;
	}

	void CopyFrom(const CBasicStreamBuf& src)
	{
		if(this == &src)
			return;

		Clear();
		Put(src.GetBytes(), 0, src.Size());
	}

	CBasicStreamBuf& operator=(const CBasicStreamBuf& buf)
	{
		CopyFrom(buf);
		return *this;
	}

    bool Seek(size_t pos)
    {
        if(pos <= m_maxlen)
        {
            m_deflen = pos;
            return( true );
        }
        return( false );
    }
};

template<DWORD BUF_SIZE>
class CFixedStreamBuf : public CBasicStreamBuf
{
protected:
	char buf[BUF_SIZE];
public:
	CFixedStreamBuf()
	{
		m_bytes = buf;
		m_maxlen = BUF_SIZE;
	}
	CFixedStreamBuf(const CFixedStreamBuf& input):CBasicStreamBuf()
	{
		m_bytes = buf;
		m_maxlen = BUF_SIZE;

		Put(input.GetBytes(), 0, input.Size());
	}
	CFixedStreamBuf& operator=(const CFixedStreamBuf& buf)
	{
		CopyFrom(buf);
		return *this;
	}
	CFixedStreamBuf& operator=(const CBasicStreamBuf& buf)
	{
		CopyFrom(buf);
		return *this;
	}

	virtual void ReAlloc(size_t size)
	{
		throw stream_error(SEC_FIXED_STREAM_OVERFLOW);
	}
	virtual void Free()
	{
	}
};

class CDynamicStreamBuf : public CBasicStreamBuf
{
public:
	CDynamicStreamBuf(size_t n=128)
	{
		if(n<16)
			n=16;
		ReAlloc(n);
	}
	CDynamicStreamBuf(const CDynamicStreamBuf& input)
		:CBasicStreamBuf()
	{
		size_t n = input.Size();
		if (n < 16)
			n = 16;
		ReAlloc(n);
		Put(input.GetBytes(), 0, input.Size());
	}
	CDynamicStreamBuf& operator=(const CDynamicStreamBuf& buf)
	{
		CopyFrom(buf);
		return *this;
	}
	CDynamicStreamBuf& operator=(const CBasicStreamBuf& buf)
	{
		CopyFrom(buf);
		return *this;
	}
	virtual ~CDynamicStreamBuf()
	{
		Free();
	}
	virtual void ReAlloc(size_t size)
	{
		if(size <= m_maxlen)
			return;

		size_t oldlen = m_maxlen;
		do
		{
			m_maxlen <<= 1;
		}while(size > m_maxlen);
		char* nbuf = new char[m_maxlen];
		if(m_bytes)
		{
			memcpy(nbuf, m_bytes, oldlen);
			delete []m_bytes;
		}
		m_bytes = nbuf;
	}
	virtual void Free()
	{
		delete []m_bytes;
	}
    virtual void Swap(CDynamicStreamBuf &rhs)
    {
        std::swap(m_bytes, rhs.m_bytes);
        std::swap(m_deflen, rhs.m_deflen);
        std::swap(m_maxlen, rhs.m_maxlen);
    }
    bool WriteToFile( const char* file_name )
    {
        if( !file_name )
            return false;
        FILE* fh = fopen( file_name, "w+b" );
        if( !fh )
            return false;
        size_t wcb = fwrite( m_bytes, 1, m_deflen, fh );
        if( wcb != m_deflen )
        {
            fclose(fh);
            return false;
        }
        fclose(fh);
        return true;
    }
    bool ReadFromFile( const char* file_name ,bool only_read = false)
    {
		FILE* fh = NULL;
		if (only_read)
			fh = fopen(file_name, "rb");
		else
			fh = fopen(file_name, "r+b");

        if( !fh )
            return false;
        if( fseek( fh, 0, SEEK_END ) != 0 )
        {
            fclose(fh);
            return false;
        }
        size_t file_size = ftell( fh );
        ExpandTo( file_size );
        m_deflen = file_size;
        if( fseek( fh, 0, SEEK_SET ) != 0 )
        {
            fclose(fh);
            return false;
        }
        size_t rcb = fread( m_bytes, 1, file_size, fh );
        if( rcb != file_size )
        {
            fclose(fh);
            return false;
        }
        fclose(fh);
        return true;
    }
};

class CBorrowedStreamBuf : public CBasicStreamBuf
{
public:
	CBorrowedStreamBuf(char* bytes, size_t len)
	{
		m_bytes = bytes;
		m_maxlen = len;
		m_deflen = len;
	}
protected:
	virtual void ReAlloc(size_t size)
	{
		throw stream_error(SEC_FIXED_STREAM_OVERFLOW);
	}
	virtual void Free()
	{
	}
};


// ------------- stream classes -----------------

class CStreamBase
{
protected:
	CBasicStreamBuf* m_streambuf;
	size_t m_pos;
public:
	CStreamBase(CBasicStreamBuf *buf)
	{
		m_streambuf = buf;
		m_pos = 0;
	}
	virtual ~CStreamBase()
	{
	}
	size_t Pos()
	{
		return m_pos;
	}
	CBasicStreamBuf* GetBuf()const
	{
		return m_streambuf;
	}
	size_t Seek(size_t n)
	{
		m_pos = n;
		return m_pos;
	}
};

struct network_endian_filter
{
	static short N2H(const short& v){return ntohs(v);}
	static short H2N(const short& v){return htons(v);}
	//------------------------------------
	static unsigned short N2H(const unsigned short& v){return ntohs(v);}
	static unsigned short H2N(const unsigned short& v){return htons(v);}
	//------------------------------------
	static int N2H(const int& v){return ntohl(v);}
	static int H2N(const int& v){return htonl(v);}
	//------------------------------------
	static unsigned int N2H(const unsigned int& v){return ntohl(v);}
	static unsigned int H2N(const unsigned int& v){return htonl(v);}
	//------------------------------------
	static long N2H(const long& v){return ntohl(v);}
	static long H2N(const long& v){return htonl(v);}
	//------------------------------------
	static unsigned long N2H(const unsigned long& v){return ntohl(v);}
	static unsigned long H2N(const unsigned long& v){return htonl(v);}
	//------------------------------------
	static int64 N2H(const int64& v)
	{
		int64 hi=N2H((DWORD)((v>>32)&0xffffffff));
		int64 lo=N2H((DWORD)(v&0xffffffff));

		return (hi | (lo<<32));
	}
	static int64 H2N(const int64& v)
	{
		int64 hi=H2N((DWORD)((v>>32)&0xffffffff));
		int64 lo=H2N((DWORD)(v&0xffffffff));

		return (hi | (lo<<32));
	}
	static uint64 N2H(const uint64& v)
	{
		uint64 hi=N2H((DWORD)((v>>32)&0xffffffff));
		uint64 lo=N2H((DWORD)(v&0xffffffff));

		return (hi | (lo<<32));
	}
	static uint64 H2N(const uint64& v)
	{
		uint64 hi=H2N((DWORD)((v>>32)&0xffffffff));
		uint64 lo=H2N((DWORD)(v&0xffffffff));

		return (hi | (lo<<32));
	}
};

struct default_endian_filter
{
	static const short& N2H(const short& v){return v;}
	static const short& H2N(const short& v){return v;}
	//------------------------------------
	static const unsigned short& N2H(const unsigned short& v){return v;}
	static const unsigned short& H2N(const unsigned short& v){return v;}
	//------------------------------------
	static const int& N2H(const int& v){return v;}
	static const int& H2N(const int& v){return v;}
	//------------------------------------
	static const unsigned int& N2H(const unsigned int& v){return v;}
	static const unsigned int& H2N(const unsigned int& v){return v;}
	//------------------------------------
	static const long& N2H(const long& v){return v;}
	static const long& H2N(const long& v){return v;}
	//------------------------------------
	static const unsigned long& N2H(const unsigned long& v){return v;}
	static const unsigned long& H2N(const unsigned long& v){return v;}
	//------------------------------------
	static const int64& N2H(const int64& v){return v;}
	static const int64& H2N(const int64& v){return v;}

	static const uint64& N2H(const uint64& v){return v;}
	static const uint64& H2N(const uint64& v){return v;}
};

class CStreamDefaultEndian : public CStreamBase
{
public:
	typedef default_endian_filter TF;
	CStreamDefaultEndian(CBasicStreamBuf* buf) : CStreamBase(buf){}
	CStreamDefaultEndian(CBasicStreamBuf& buf) : CStreamBase(&buf){}
};
class CStreamNetworkEndian : public CStreamBase
{
public:
	typedef network_endian_filter TF;
	CStreamNetworkEndian(CBasicStreamBuf* buf) : CStreamBase(buf){}
	CStreamNetworkEndian(CBasicStreamBuf& buf) : CStreamBase(&buf){}
};

template<class Base>
class CIStreamGeneric : public Base
{
public:
	CIStreamGeneric(const CBasicStreamBuf* buf) : Base(const_cast<CBasicStreamBuf*>(buf)){}
	CIStreamGeneric(const CBasicStreamBuf& buf) : Base(const_cast<CBasicStreamBuf*>(&buf)){}
	~CIStreamGeneric(){}

	int BytesLeft()
	{
		//int temp = Base::m_streambuf->size();
		//int temp2 = Base::m_pos;
		//return temp-temp2;
		return (int)Base::m_streambuf->Size() - (int)Base::m_pos;
	}
	operator bool()
	{
		return BytesLeft() > 0;
	}
	char* Skip(size_t n)
	{

		char* ret = Base::m_streambuf->GetBufRange(Base::m_pos, n);

		if(ret)
		{
			Base::m_pos += n;
		}
		else
		{
			throw stream_error(SEC_OUT_OF_RANGE);
		}
		return ret;
	}
	size_t Read(void* p, size_t n)
	{
		size_t res = Base::m_streambuf->Get(p, Base::m_pos, n);
		Base::m_pos += res;
		return res;
	}
	size_t ReadNoThrow(void* p, size_t n)
	{
		size_t res = Base::m_streambuf->GetNoThrow(p, Base::m_pos, n);
		Base::m_pos += res;
		return res;
	}
	//////////////////////////////////////////////////
	CIStreamGeneric& operator >>(CBasicStreamBuf &buf)
	{
		int size;
		(*this)>>size;
		buf.Put(Skip(size), 0, size);
		return *this;
	}
	///////////////////////////////////////////////////////////
	CIStreamGeneric& operator >>(bool &v)
	{
		Base::m_pos += Base::m_streambuf->Get(&v,Base::m_pos,sizeof(v));
		return *this;
	}
	///////////////////////////////////////////////////////////
	CIStreamGeneric& operator >>(char &v)
	{
		Base::m_pos += Base::m_streambuf->Get(&v,Base::m_pos,sizeof(v));
		return *this;
	}
    CIStreamGeneric& operator >>(wchar_t &v)
    {
        unsigned short us;
        Base::m_pos += Base::m_streambuf->Get(&us,Base::m_pos,sizeof(us));
        v = (wchar_t)us;
        return *this;
    }
	CIStreamGeneric& operator >>(unsigned char &v)
	{
		Base::m_pos += Base::m_streambuf->Get(&v,Base::m_pos,sizeof(v));
		return *this;
	}
	///////////////////////////////////////////////////////////
	CIStreamGeneric& operator >>(int &v)
	{
		Base::m_pos += Base::m_streambuf->Get(&v,Base::m_pos,sizeof(v));
		v = Base::TF::N2H(v);
		return *this;
	}
	CIStreamGeneric& operator >>(unsigned int &v)
	{
		Base::m_pos += Base::m_streambuf->Get(&v,Base::m_pos,sizeof(v));
		v = Base::TF::N2H(v);
		return *this;
	}
	///////////////////////////////////////////////////////////
	CIStreamGeneric& operator >>(short &v)
	{
		Base::m_pos += Base::m_streambuf->Get(&v,Base::m_pos,sizeof(v));
		v = Base::TF::N2H(v);
		return *this;
	}
	CIStreamGeneric& operator >>(unsigned short &v)
	{
		Base::m_pos += Base::m_streambuf->Get(&v,Base::m_pos,sizeof(v));
		v = Base::TF::N2H(v);
		return *this;
	}
	///////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////
	CIStreamGeneric& operator >>(float &v)
	{
		Base::m_pos += Base::m_streambuf->Get(&v,Base::m_pos,sizeof(v));
		return *this;
	}
	///////////////////////////////////////////////////////////
	CIStreamGeneric& operator >>(double &v)
	{
		Base::m_pos += Base::m_streambuf->Get(&v,Base::m_pos,sizeof(v));
		return *this;
	}
	///////////////////////////////////////////////////////////
	CIStreamGeneric &operator >>(int64 &v)
	{
		Base::m_pos += Base::m_streambuf->Get(&v,Base::m_pos,sizeof(v));
		v = Base::TF::N2H(v);
		return *this;
	}
	///////////////////////////////////////////////////////////
	CIStreamGeneric &operator >>(uint64 &v)
	{
		Base::m_pos += Base::m_streambuf->Get(&v,Base::m_pos,sizeof(v));
		v = Base::TF::N2H(v);
		return *this;
	}
	///////////////////////////////////////////////////////////

	CIStreamGeneric& ReadString(char* &str, size_t maxlen)
	{
		unsigned int len;
		(*this)>>len;
		if(len>maxlen-1)
			throw stream_error(SEC_INVALID_STRING);
		str = Base::GetBuf()->GetBytes()+Base::Pos();
		Skip(len+sizeof(char));

		if(str[len]!=0)
		{
			throw stream_error(SEC_INVALID_STRING);
		}
		return *this;
	}
};

template<class Base>
CIStreamGeneric<Base>& operator >>(CIStreamGeneric<Base>&is, long &v)
{
/*
there may be some differences between g++ and vc about template instantiation
in g++,even a template-func is not called,it also will be instanced
so the BOOST_STATIC_ASSERT always cause compile-error by g++
*/
#if _PLATFORM_WIN32_	
	X5_BOOST_STATIC_ASSERT(false);
#endif
	return is;
}
template<class Base>
CIStreamGeneric<Base>& operator >>(CIStreamGeneric<Base>& is, unsigned long &v)
{
#if _PLATFORM_WIN32_
	X5_BOOST_STATIC_ASSERT(false);
#endif
	return is;
}

template<class Base>
class COStreamGeneric : public Base
{
public:
	COStreamGeneric(CBasicStreamBuf *buf) : Base(buf){}
	COStreamGeneric(CBasicStreamBuf &buf) : Base(&buf){}
	~COStreamGeneric(){}
	size_t Write(const void* p, size_t n)
	{
		size_t res = Base::m_streambuf->Put(p, Base::m_pos, n);
		Base::m_pos += res;
		return res;
	}
	size_t Write(CIStreamGeneric<Base> &is)
	{
		size_t size = is.GetBuf()->Size() - is.Pos();
		return Write(is.Skip(size), size);
	}
	char* Skip(size_t n)
	{
		size_t oldp = Base::m_pos;
		Base::m_pos += n;
		Base::m_streambuf->ExpandTo(Base::m_pos);
		return Base::m_streambuf->GetBytes() + oldp;
	}
	//////////////////////////////////////////////////
	COStreamGeneric& operator <<(const CBasicStreamBuf &buf)
	{
		int size = (int)buf.Size();
		(*this)<<size;
		Write(buf.GetBytes(), size);
		return *this;
	}
	//////////////////////////////////////////////////
	COStreamGeneric& operator <<(const bool &v)
	{
		Base::m_pos += Base::m_streambuf->Put(&v,Base::m_pos,sizeof(v));
		return *this;
	}
	//////////////////////////////////////////////////
	COStreamGeneric& operator <<(const char &v)
	{
		Base::m_pos += Base::m_streambuf->Put(&v,Base::m_pos,sizeof(v));
		return *this;
	}
    COStreamGeneric& operator <<(const wchar_t &v)
    {
        unsigned short us = (unsigned short)v;
        Base::m_pos += Base::m_streambuf->Put(&us,Base::m_pos,sizeof(us));
        return *this;
    }
	COStreamGeneric& operator <<(const unsigned char &v)
	{
		Base::m_pos += Base::m_streambuf->Put(&v,Base::m_pos,sizeof(v));
		return *this;
	}
	//////////////////////////////////////////////////
	COStreamGeneric& operator <<(const int &v)
	{
		int var = Base::TF::H2N(v);
		Base::m_pos += Base::m_streambuf->Put(&var,Base::m_pos,sizeof(v));
		return *this;
	}
	COStreamGeneric& operator <<(const unsigned int &v)
	{
		unsigned int var = Base::TF::H2N(v);
		Base::m_pos += Base::m_streambuf->Put(&var,Base::m_pos,sizeof(v));
		return *this;
	}
	//////////////////////////////////////////////////
	COStreamGeneric& operator <<(const short &v)
	{
		short var = Base::TF::H2N(v);
		Base::m_pos += Base::m_streambuf->Put(&var,Base::m_pos,sizeof(v));
		return *this;
	}
	COStreamGeneric& operator <<(const unsigned short &v)
	{
		unsigned short var = Base::TF::H2N(v);
		Base::m_pos += Base::m_streambuf->Put(&var,Base::m_pos,sizeof(v));
		return *this;
	}
	//////////////////////////////////////////////////
	COStreamGeneric& operator <<(const float &v)
	{
		Base::m_pos += Base::m_streambuf->Put(&v,Base::m_pos,sizeof(v));
		return *this;
	}
	//////////////////////////////////////////////////
	COStreamGeneric& operator <<(const double &v)
	{
		Base::m_pos += Base::m_streambuf->Put(&v,Base::m_pos,sizeof(v));
		return *this;
	}
	//////////////////////////////////////////////////
	COStreamGeneric &operator <<(const int64 &v)
	{
		int64 var = Base::TF::H2N(v);
		Base::m_pos += Base::m_streambuf->Put(&var,Base::m_pos,sizeof(v));
		return *this;
	}
	//////////////////////////////////////////////////
	COStreamGeneric &operator <<(const uint64 &v)
	{
		uint64 var = Base::TF::H2N(v);
		Base::m_pos += Base::m_streambuf->Put(&var,Base::m_pos,sizeof(v));
		return *this;
	}
};


//////////////////////////////////////////////////
template<class Base>
COStreamGeneric<Base>& operator <<(COStreamGeneric<Base>& os, const long &v)
{
#if _PLATFORM_WIN32_	//see operator >>(long&) for reason
	X5_BOOST_STATIC_ASSERT(false);
#endif
	return os;
}
template<class Base>
COStreamGeneric<Base>& operator <<(COStreamGeneric<Base>& os, const unsigned long &v)
{
#if _PLATFORM_WIN32_
	X5_BOOST_STATIC_ASSERT(false);
#endif
	return os;
}


template<class ST>
inline COStreamGeneric<ST> &operator <<(COStreamGeneric<ST> &os, const char* str)
{
	unsigned int len = (unsigned int)strlen(str);
	os<<len;
	os.Write(str, len);
	char eos=0;
	os<<eos;
	return os;
}
template<class ST>
inline CIStreamGeneric<ST> &operator >>(CIStreamGeneric<ST> &is, const char* &str)
{
	unsigned int len;
	is>>len;
	str = is.GetBuf()->GetBytes()+is.Pos();
	is.Skip(len+sizeof(char));

	if(str[len]!=0)
	{
		throw stream_error(SEC_INVALID_STRING);
	}
	return is;
}

template<class ST, int L>
inline CIStreamGeneric<ST> &operator >>(CIStreamGeneric<ST> &is, char (&str)[L])
{
	char* src;
	is.ReadString(src, L);
	strcpy(str, src);//不应该用memcpy，因为存储的时候用的是strcpy
	return is;
}

template<class ST>
inline COStreamGeneric<ST> &operator <<(COStreamGeneric<ST> &os, const std::string &str)
{
	return os<<str.c_str();
}
template<class ST>
inline CIStreamGeneric<ST> &operator >>(CIStreamGeneric<ST> &is, std::string &str)
{
	const char* s;
	is>>s;
	str = s;
	return is;
}

template<class ST>
inline COStreamGeneric<ST> &operator <<(COStreamGeneric<ST> &os, const uchar_t* str)
{
	size_t len = ustrlen(str);
	for(size_t i=0;i<len;++i)
		os<<str[i];
	uchar_t eos=0;
	os<<eos;
	return os;
}
template<class ST>
inline CIStreamGeneric<ST> &operator >>(CIStreamGeneric<ST> &is, uchar_t* &str)
{
	str = reinterpret_cast<uchar_t*>(is.GetBuf()->GetBytes()+is.Pos());
	uchar_t c;
	is>>c;
	int i=0;
	while(c)//NOTE!!! we should check length to avoid bad string
	{
		str[i++] = c;//correct byte order
		is>>c;
	}
	return is;
}
template<class ST>
inline COStreamGeneric<ST> &operator <<(COStreamGeneric<ST> &os, const ustring &str)
{
	return os<<str.c_str();
}
template<class ST>
inline CIStreamGeneric<ST> &operator >>(CIStreamGeneric<ST> &is, ustring &str)
{
	str.reserve(128);
	str.clear();
	uchar_t c;
	is>>c;
	while(c)//NOTE!!! we should check length to avoid bad string
	{
		str+=c;
		is>>c;
	}
	return is;
}


#include <vector>

template<typename ST, typename T1, typename T2>
inline COStreamGeneric<ST> &operator <<(COStreamGeneric<ST> &os, const std::pair<T1, T2> & pair)
{
	os << pair.first << pair.second;
	return os;
}
template<typename ST, typename T1, typename T2>
inline CIStreamGeneric<ST> &operator >>(CIStreamGeneric<ST> &is, std::pair<T1, T2> & pair)
{
	is >> pair.first >> pair.second;
	return is;
}

template<typename ST, typename T>
inline COStreamGeneric<ST> &operator <<(COStreamGeneric<ST> &os, const std::vector<T> & vec)
{
	os << static_cast<int> ( vec.size() );
	for( size_t i=0; i<vec.size(); ++i)
	{
		os << vec[i];
	}
	return os;
}
template<typename ST, typename T>
inline CIStreamGeneric<ST> &operator >>(CIStreamGeneric<ST> &is, std::vector<T> & vec)
{
	int num;
	is >> num;
	for(int i = 0; i < num; ++ i )
	{
		T val;
		is >> val;
		vec.push_back( val );
	}
	return is;
}

#include <list>
template<typename ST, typename T>
inline COStreamGeneric<ST> &operator <<(COStreamGeneric<ST> &os, const std::list<T> & ilist)
{
	os << static_cast<int> ( ilist.size() );
	for(typename std::list<T>::const_iterator it = ilist.begin(); it != ilist.end(); ++ it)
	{
		os << * it;
	}
	return os;
}
template<typename ST, typename T>
inline CIStreamGeneric<ST> &operator >>(CIStreamGeneric<ST> &is, std::list<T> & ilist)
{
	int num;
	is >> num;
	for(int i = 0; i < num; ++ i )
	{
		T val;
		is >> val;
		ilist.push_back( val );
	}
	return is;
}

#include <deque>
template<typename ST, typename T>
inline COStreamGeneric<ST> &operator <<(COStreamGeneric<ST> &os, const std::deque<T> & ideque)
{
	os << static_cast<int> ( ideque.size() );
	for(typename std::deque<T>::const_iterator it = ideque.begin(); it != ideque.end(); ++ it)
	{
		os << * it;
	}
	return os;
}
template<typename ST, typename T>
inline CIStreamGeneric<ST> &operator >>(CIStreamGeneric<ST> &is, std::deque<T> & ideque)
{
	int num;
	is >> num;
	for(int i = 0; i < num; ++i)
	{
		T val;
		is >> val;
		ideque.push_back( val );
	}
	return is;
}

#include <map>
template<typename ST, typename K, typename V>
inline COStreamGeneric<ST> &operator <<(COStreamGeneric<ST> &os, const std::map<K, V> & amap)
{
	os << static_cast<int> ( amap.size() );
	for(typename std::map<K, V>::const_iterator it = amap.begin(); it != amap.end(); ++ it)
	{
		os << it->first << it->second;
	}
	return os;
}

template<typename ST, typename K, typename V>
inline CIStreamGeneric<ST> &operator >>(CIStreamGeneric<ST> &is, std::map<K, V> & amap)
{
	amap.clear();
	int num;
	is >> num;
	for(int i = 0; i < num; ++ i )
	{
		K key;
		V val;
		is >> key >> val;
		amap.insert(std::pair<K,V>(key, val));
	}
	return is;
}


#include <set>
template<typename ST, typename V, typename LESS>
inline COStreamGeneric<ST> &operator <<(COStreamGeneric<ST> &os, const std::set<V, LESS> & aset)
{
	os << static_cast<int> ( aset.size() );
	for(typename std::set<V, LESS>::const_iterator it = aset.begin(); it != aset.end(); ++ it)
	{
		os << *it;
	}
	return os;
}

template<typename ST, typename V, typename LESS>
inline CIStreamGeneric<ST> &operator >>(CIStreamGeneric<ST> &is, std::set<V, LESS> & aset)
{
	aset.clear();
	int num;
	is >> num;
	for(int i = 0; i < num; ++ i )
	{
		V val;
		is >> val;
		aset.insert(val);
	}
	return is;
}
// multiset
template<typename ST, typename V, typename LESS>
inline COStreamGeneric<ST> &operator <<(COStreamGeneric<ST> &os, const std::multiset<V, LESS> & aset)
{
    os << static_cast<int> ( aset.size() );
    for(typename std::multiset<V, LESS>::const_iterator it = aset.begin(); it != aset.end(); ++ it)
    {
        os << *it;
    }
    return os;
}

template<typename ST, typename V, typename LESS>
inline CIStreamGeneric<ST> &operator >>(CIStreamGeneric<ST> &is, std::multiset<V, LESS> & aset)
{
    aset.clear();
    int num;
    is >> num;
    for(int i = 0; i < num; ++ i )
    {
        V val;
        is >> val;
        aset.insert(val);
    }
    return is;
}

inline void CheckCharValid(const char* str)//checks only the validness of chars
{
	while(*str)
	{
		if((unsigned char)*str < 32)
		{
			throw stream_error(SEC_INVALID_CHAR);
		}
		++str;
	}
}
inline void CheckCharValid(const std::string& str)//checks only the validness of chars
{
	CheckCharValid(str.c_str());
}

typedef CIStreamGeneric<CStreamDefaultEndian> CIStream;
typedef COStreamGeneric<CStreamDefaultEndian> COStream;
typedef CIStreamGeneric<CStreamNetworkEndian> CIStreamNet;
typedef COStreamGeneric<CStreamNetworkEndian> COStreamNet;


//
//inline COStream& operator <<(COStream& os, const string_wrapper& str_wrapper)
//{
//	return os << str_wrapper.m_str_data;
//	//return os;
//}
//
//
//inline CIStream& operator >>(CIStream& is, string_wrapper& str_wrapper)
//{
//	return is >> str_wrapper.m_str_data;
//	//return is;
//}
////bibo_hashmap
//template<typename V>
//inline COStream &operator <<(COStream &os, const std::map<std::string,V>& amap)
//{
//	os << static_cast<int> ( amap.size() );
//	for(typename std::map<std::string,V>::const_iterator it = amap.begin(); it != amap.end(); ++ it)
//	{
//		os << it->first << it->second;
//	}
//	return os;
//}
//
//template<typename V>
//inline CIStream &operator >>(CIStream &is, std::map<std::string,V>& amap)
//{
//	amap.clear();
//	int num;
//	is >> num;
//	for(int i = 0; i < num; ++ i )
//	{
//		string_wrapper key;
//		V val;
//		is >> key >> val;
//		amap.insert(std::make_pair(key, val));
//	}
//	return is;
//}
//bibo_hashmap END
#endif // !defined(AFX_BYTESTREAM_H__B009F0DA_3478_4652_B154_260EA0EE14C1__INCLUDED_)
