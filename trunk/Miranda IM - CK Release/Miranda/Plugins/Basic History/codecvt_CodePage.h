#pragma once

template<class _Elem>
class codecvt_CodePage : public std::codecvt<_Elem, char, mbstate_t>
{
private:
	UINT codePage;
public:
	typedef std::codecvt<_Elem, char, mbstate_t> _Mybase;
	typedef typename _Mybase::result result;
	typedef char _Byte;
	typedef _Elem intern_type;
	typedef _Byte extern_type;
	typedef mbstate_t state_type;

	explicit codecvt_CodePage(UINT _codePage)
	: _Mybase(0),
	codePage(_codePage)
	{	// construct with ref count
	}

	virtual ~codecvt_CodePage()
	{	// destroy the object
	}

protected:
	virtual result do_in(mbstate_t& _State, const _Byte *_First1, const _Byte *_Last1, const _Byte *& _Mid1, _Elem *_First2, _Elem *_Last2, _Elem *& _Mid2) const
	{	// convert bytes [_First1, _Last1) to [_First2, _Last)
		return (_Mybase::error);	// not implemented
	}

	virtual result do_out(mbstate_t& _State, const _Elem *_First1, const _Elem *_Last1, const _Elem *& _Mid1, _Byte *_First2, _Byte *_Last2, _Byte *& _Mid2) const
	{	// convert [_First1, _Last1) to bytes [_First2, _Last)
		_Mid1 = _First1;
		_Mid2 = _First2;

		int conv = WideCharToMultiByte(codePage, 0, _First1, _Last1 - _First1, _First2, _Last2 - _First2, NULL, NULL);
		if(conv == 0)
		{
			if(GetLastError() == ERROR_INSUFFICIENT_BUFFER)
				return (_Mybase::partial);
			else
				return (_Mybase::error);
		}
		else
		{
			_Mid1 = _Last1;
			_Mid2 = _First2 + conv;
		}

		return (_Mybase::ok);
	}

	virtual result do_unshift(mbstate_t&,
	_Byte *_First2, _Byte *, _Byte *& _Mid2) const
	{	// generate bytes to return to default shift state
		_Mid2 = _First2;
		return (_Mybase::ok);
	}

	virtual int do_length(const mbstate_t& _State, const _Byte *_First1, const _Byte *_Last1, size_t _Count) const _THROW0()
	{	// return min(_Count, converted length of bytes [_First1, _Last1))
		return _Count;	// not implemented
	}

	virtual bool do_always_noconv() const _THROW0()
	{	// return true if conversions never change input
		return (false);
	}

	virtual int do_max_length() const _THROW0()
	{	// return maximum length required for a conversion
		return 6;
	}

	virtual int do_encoding() const _THROW0()
	{	// return length of code sequence (from codecvt)
		return 0;	// -1 => state dependent, 0 => varying length
	}
};
