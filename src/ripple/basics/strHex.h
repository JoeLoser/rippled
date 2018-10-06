//------------------------------------------------------------------------------
/*
    This file is part of rippled: https://github.com/ripple/rippled
    Copyright (c) 2012, 2013 Ripple Labs Inc.

    Permission to use, copy, modify, and/or distribute this software for any
    purpose  with  or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE  SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH  REGARD  TO  THIS  SOFTWARE  INCLUDING  ALL  IMPLIED  WARRANTIES  OF
    MERCHANTABILITY  AND  FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY  SPECIAL ,  DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER  RESULTING  FROM  LOSS  OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION  OF  CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
//==============================================================================

// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2011 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.

#ifndef RIPPLE_BASICS_STRHEX_H_INCLUDED
#define RIPPLE_BASICS_STRHEX_H_INCLUDED

#include <ripple/basics/Blob.h>
#include <cassert>
#include <string>

#include <boost/algorithm/hex.hpp>
#include <boost/endian/conversion.hpp>

namespace ripple {
class Slice;

/** Converts an integer to the corresponding hex digit
    @param iDigit 0-15 inclusive
    @return a character from '0'-'9' or 'A'-'F'.
*/
inline
char
charHex (unsigned int digit)
{
    static
    char const xtab[] = "0123456789ABCDEF";

    assert (digit < 16);

    return xtab[digit];
}

/** @{ */
/** Converts a hex digit to the corresponding integer
    @param cDigit one of '0'-'9', 'A'-'F' or 'a'-'f'
    @return an integer from 0 to 15 on success; -1 on failure.
*/
int
charUnHex (unsigned char c);

inline
int
charUnHex (char c)
{
    return charUnHex (static_cast<unsigned char>(c));
}
/** @} */

template<class InputIterator>
std::string strHex(InputIterator begin, InputIterator end)
{
    std::string result{};
    result.reserve (std::distance(begin, end));
    boost::algorithm::hex (begin, end, std::back_inserter(result));
    return result;
}

template<class T>
class check_has_bytes
{
    // has data() method
    template<class U, class R = decltype(
        std::declval<U>().data(),
            std::true_type{})>
    static R check1(int);
    template<class>
    static std::false_type check1(...);
    using type1 = decltype(check1<T>(0));

    // has size() method
    template<class U, class R = decltype(
        std::declval<U>().size(),
            std::true_type{})>
    static R check2(int);
    template<class>
    static std::false_type check2(...);
    using type2 = decltype(check2<T>(0));

public:
    using type = std::integral_constant<bool,
        type1::value && type2::value>;
};
template<class T>
using is_Bytes = typename check_has_bytes<T>::type;

template <class T, class = std::enable_if_t <is_Bytes<T>::value>>
std::string
strHex(T const& bobj)
{
    const auto begin = bobj.data();
    return strHex(begin, begin + bobj.size());
}

inline std::string strHex (std::string const& src)
{
    return boost::algorithm::hex (src);
}

inline std::string strHex (const std::uint64_t uiHost)
{
    uint64_t uBig    = boost::endian::native_to_big (uiHost);

    const auto begin = (unsigned char*) &uBig;
    const auto end   = begin + sizeof(uBig);
    return strHex(begin, end);
}
}

#endif
