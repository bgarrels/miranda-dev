#pragma once

template<class _Elem, unsigned long _Mymax = 0x10ffff>
class codecvt_my_utf8 : public std::codecvt<_Elem, char, mbstate_t>
{
public:
	typedef std::codecvt<_Elem, char, mbstate_t> _Mybase;
	typedef typename _Mybase::result result;
	typedef char _Byte;
	typedef _Elem intern_type;
	typedef _Byte extern_type;
	typedef mbstate_t state_type;

	explicit codecvt_my_utf8(size_t _Refs = 0)
	: _Mybase(_Refs)
	{	// construct with ref count
	}

	virtual ~codecvt_my_utf8()
	{	// destroy the object
	}

protected:
	virtual result do_in(mbstate_t& _State, const _Byte *_First1, const _Byte *_Last1, const _Byte *& _Mid1, _Elem *_First2, _Elem *_Last2, _Elem *& _Mid2) const
	{	// convert bytes [_First1, _Last1) to [_First2, _Last)
		_Mid1 = _First1;
		_Mid2 = _First2;

		for (; _Mid1 != _Last1 && _Mid2 != _Last2; )
		{	// convert a multibyte sequence
			unsigned char by = (unsigned char)*_Mid1;
			unsigned long ch;
			int nextra;

			if (by < 0x80)
				ch = by, nextra = 0;
			else if (by < 0xc0)
			{	// 0x80-0xdf not first byte
				++_Mid1;
				return (_Mybase::error);
			}
			else if (by < 0xe0)
				ch = by & 0x1f, nextra = 1;
			else if (by < 0xf0)
				ch = by & 0x0f, nextra = 2;
			else if (by < 0xf8)
				ch = by & 0x07, nextra = 3;
			else
				ch = by & 0x03, nextra = by < 0xfc ? 4 : 5;

			if (nextra == 0)
				++_Mid1;
			else if (_Last1 - _Mid1 < nextra + 1)
				break;	// not enough input
			else
				for (++_Mid1; 0 < nextra; --nextra, ++_Mid1)
					if ((by = (unsigned char)*_Mid1) < 0x80 || 0xc0 <= by)
						return (_Mybase::error);	// not continuation byte
					else
						ch = ch << 6 | by & 0x3f;

			if (_Mymax < ch)
				return (_Mybase::error);	// code too large
			*_Mid2++ = (_Elem)ch;
		}

		return (_First1 == _Mid1 ? _Mybase::partial : _Mybase::ok);
	}

	virtual result do_out(mbstate_t& _State, const _Elem *_First1, const _Elem *_Last1, const _Elem *& _Mid1, _Byte *_First2, _Byte *_Last2, _Byte *& _Mid2) const
	{	// convert [_First1, _Last1) to bytes [_First2, _Last)
		_Mid1 = _First1;
		_Mid2 = _First2;

		for (; _Mid1 != _Last1 && _Mid2 != _Last2; )
		{	// convert and put a wide char
			_Byte by;
			int nextra;
			unsigned long ch = (unsigned long)*_Mid1;

			if (_Mymax < ch)
				return (_Mybase::error);

			if (ch < 0x0080)
				by = (_Byte)ch, nextra = 0;
			else if (ch < 0x0800)
				by = (_Byte)(0xc0 | ch >> 6), nextra = 1;
			else if (ch < 0x00010000)
				by = (_Byte)(0xe0 | ch >> 12), nextra = 2;
			else if (ch < 0x00200000)
				by = (_Byte)(0xf0 | ch >> 18), nextra = 3;
			else if (ch < 0x04000000)
				by = (_Byte)(0xf8 | ch >> 24), nextra = 4;
			else
				by = (_Byte)(0xfc | ch >> 30 & 0x03), nextra = 5;
			
			if (_Last2 - _Mid2 < 1 + nextra)
				break;	// not enough room for output

			++_Mid1;
			for (*_Mid2++ = by; 0 < nextra; )
				*_Mid2++ = (_Byte)(ch >> 6 * --nextra & 0x3f | 0x80);
		}
		return (_First1 == _Mid1 ? _Mybase::partial : _Mybase::ok);
	}

	virtual result do_unshift(mbstate_t&,
	_Byte *_First2, _Byte *, _Byte *& _Mid2) const
	{	// generate bytes to return to default shift state
		_Mid2 = _First2;
		return (_Mybase::ok);
	}

	virtual int do_length(const mbstate_t& _State, const _Byte *_First1, const _Byte *_Last1, size_t _Count) const _THROW0()
	{	// return min(_Count, converted length of bytes [_First1, _Last1))
		size_t wchars = 0;
		mbstate_t mystate = _State;

		for (; wchars < _Count && _First1 != _Last1; )
		{
			const _Byte *_Mid1;
			_Elem *_Mid2;
			_Elem ch;

			switch (do_in(mystate, _First1, _Last1, _Mid1, &ch, &ch + 1, _Mid2))
			{
			case _Mybase::noconv:
				return ((int)(wchars + (_Last1 - _First1)));

			case  _Mybase::ok:
				if (_Mid2 == &ch + 1)
				++wchars;
				_First1 = _Mid1;
				break;

			default:
				return ((int)wchars);
			}
		}

		return ((int)wchars);
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
