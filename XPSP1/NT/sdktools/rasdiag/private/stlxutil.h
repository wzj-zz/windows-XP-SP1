#pragma once
#ifndef _STLXUTIL_H_
#define _STLXUTIL_H_
//#include <utility>

#include <stlutil.h>

#ifdef  _MSC_VER
#pragma pack(push,8)
#endif  /* _MSC_VER */

_STD_BEGIN

// TEMPLATE FUNCTION copy
template<class _II, class _OI> inline
_OI copy(_II _F, _II _L, _OI _X)
{
    for (; _F != _L; ++_X, ++_F)
        *_X = *_F;
    return (_X);
}
// TEMPLATE FUNCTION copy_backward
template<class _BI1, class _BI2> inline
_BI2 copy_backward(_BI1 _F, _BI1 _L, _BI2 _X)
{
    while (_F != _L)
        *--_X = *--_L;
    return (_X);
}
// TEMPLATE FUNCTION equal
template<class _II1, class _II2> inline
bool equal(_II1 _F, _II1 _L, _II2 _X)
{
    return (mismatch(_F, _L, _X).first == _L);
}
// TEMPLATE FUNCTION equal WITH PRED
template<class _II1, class _II2, class _Pr> inline
bool equal(_II1 _F, _II1 _L, _II2 _X, _Pr _P)
{
    return (mismatch(_F, _L, _X, _P).first == _L);
}
// TEMPLATE FUNCTION fill
template<class _FI, class _Ty> inline
void fill(_FI _F, _FI _L, const _Ty& _X)
{
    for (; _F != _L; ++_F)
        *_F = _X;
}
// TEMPLATE FUNCTION fill_n
template<class _OI, class _Sz, class _Ty> inline
void fill_n(_OI _F, _Sz _N, const _Ty& _X)
{
    for (; 0 < _N; --_N, ++_F)
        *_F = _X;
}
// TEMPLATE FUNCTION lexicographical_compare
template<class _II1, class _II2> inline
bool lexicographical_compare(_II1 _F1, _II1 _L1,
                             _II2 _F2, _II2 _L2)
{
    for (; _F1 != _L1 && _F2 != _L2; ++_F1, ++_F2)
        if (*_F1 < *_F2)
            return (true);
        else if (*_F2 < *_F1)
            return (false);
    return (_F1 == _L1 && _F2 != _L2);
}
// TEMPLATE FUNCTION lexicographical_compare WITH PRED
template<class _II1, class _II2, class _Pr> inline
bool lexicographical_compare(_II1 _F1, _II1 _L1,
                             _II2 _F2, _II2 _L2, _Pr _P)
{
    for (; _F1 != _L1 && _F2 != _L2; ++_F1, ++_F2)
        if (_P(*_F1, *_F2))
            return (true);
        else if (_P(*_F2, *_F1))
            return (false);
    return (_F1 == _L1 && _F2 != _L2);
}
// TEMPLATE FUNCTION max
#ifndef _MAX
 #define _MAX   _cpp_max
 #define _MIN   _cpp_min
#endif
template<class _Ty> inline
const _Ty& _cpp_max(const _Ty& _X, const _Ty& _Y)
{
    return (_X < _Y ? _Y : _X);
}
// TEMPLATE FUNCTION max WITH PRED
template<class _Ty, class _Pr> inline
const _Ty& _cpp_max(const _Ty& _X, const _Ty& _Y, _Pr _P)
{
    return (_P(_X, _Y) ? _Y : _X);
}
// TEMPLATE FUNCTION min
template<class _Ty> inline
const _Ty& _cpp_min(const _Ty& _X, const _Ty& _Y)
{
    return (_Y < _X ? _Y : _X);
}
// TEMPLATE FUNCTION min WITH PRED
template<class _Ty, class _Pr> inline
const _Ty& _cpp_min(const _Ty& _X, const _Ty& _Y, _Pr _P)
{
    return (_P(_Y, _X) ? _Y : _X);
}
// TEMPLATE FUNCTION mismatch
template<class _II1, class _II2> inline
pair<_II1, _II2> mismatch(_II1 _F, _II1 _L, _II2 _X)
{
    for (; _F != _L && *_F == *_X; ++_F, ++_X)
        ;
    return (pair<_II1, _II2>(_F, _X));
}
// TEMPLATE FUNCTION mismatch WITH PRED
template<class _II1, class _II2, class _Pr> inline
pair<_II1, _II2> mismatch(_II1 _F, _II1 _L, _II2 _X, _Pr _P)
{
    for (; _F != _L && _P(*_F, *_X); ++_F, ++_X)
        ;
    return (pair<_II1, _II2>(_F, _X));
}
// TEMPLATE FUNCTION swap
template<class _Ty> inline
void swap(_Ty& _X, _Ty& _Y)
{
    _Ty _Tmp = _X;
    _X = _Y, _Y = _Tmp;
}

_STD_END

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif /* _STLXUTIL_H_ */

/*
 * Copyright (c) 1995 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
 */

/*
 * This file is derived from software bearing the following
 * restrictions:
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this
 * software and its documentation for any purpose is hereby
 * granted without fee, provided that the above copyright notice
 * appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation.
 * Hewlett-Packard Company makes no representations about the
 * suitability of this software for any purpose. It is provided
 * "as is" without express or implied warranty.
 */
