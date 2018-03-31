// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_ARITH_UINT256_H
#define BITCOIN_ARITH_UINT256_H

#include <assert.h>
#include <cstring>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <vector>

class uint256;

class uint_error_v2 : public std::runtime_error {
public:
    explicit uint_error_v2(const std::string& str) : std::runtime_error(str) {}
};

/** Template base class for unsigned big integers. */
template<unsigned int BITS>
class base_uint_v2
{
protected:
    static constexpr int WIDTH = BITS / 32;
    uint32_t pn[WIDTH];
public:

    base_uint_v2()
    {
        static_assert(BITS/32 > 0 && BITS%32 == 0, "Template parameter BITS must be a positive multiple of 32.");

        for (int i = 0; i < WIDTH; i++)
            pn[i] = 0;
    }

    base_uint_v2(const base_uint_v2& b)
    {
        static_assert(BITS/32 > 0 && BITS%32 == 0, "Template parameter BITS must be a positive multiple of 32.");

        for (int i = 0; i < WIDTH; i++)
            pn[i] = b.pn[i];
    }

    base_uint_v2& operator=(const base_uint_v2& b)
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] = b.pn[i];
        return *this;
    }

    base_uint_v2(uint64_t b)
    {
        static_assert(BITS/32 > 0 && BITS%32 == 0, "Template parameter BITS must be a positive multiple of 32.");

        pn[0] = (unsigned int)b;
        pn[1] = (unsigned int)(b >> 32);
        for (int i = 2; i < WIDTH; i++)
            pn[i] = 0;
    }

    explicit base_uint_v2(const std::string& str);

    bool operator!() const
    {
        for (int i = 0; i < WIDTH; i++)
            if (pn[i] != 0)
                return false;
        return true;
    }

    const base_uint_v2 operator~() const
    {
        base_uint_v2 ret;
        for (int i = 0; i < WIDTH; i++)
            ret.pn[i] = ~pn[i];
        return ret;
    }

    const base_uint_v2 operator-() const
    {
        base_uint_v2 ret;
        for (int i = 0; i < WIDTH; i++)
            ret.pn[i] = ~pn[i];
        ++ret;
        return ret;
    }

    double getdouble() const;

    base_uint_v2& operator=(uint64_t b)
    {
        pn[0] = (unsigned int)b;
        pn[1] = (unsigned int)(b >> 32);
        for (int i = 2; i < WIDTH; i++)
            pn[i] = 0;
        return *this;
    }

    base_uint_v2& operator^=(const base_uint_v2& b)
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] ^= b.pn[i];
        return *this;
    }

    base_uint_v2& operator&=(const base_uint_v2& b)
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] &= b.pn[i];
        return *this;
    }

    base_uint_v2& operator|=(const base_uint_v2& b)
    {
        for (int i = 0; i < WIDTH; i++)
            pn[i] |= b.pn[i];
        return *this;
    }

    base_uint_v2& operator^=(uint64_t b)
    {
        pn[0] ^= (unsigned int)b;
        pn[1] ^= (unsigned int)(b >> 32);
        return *this;
    }

    base_uint_v2& operator|=(uint64_t b)
    {
        pn[0] |= (unsigned int)b;
        pn[1] |= (unsigned int)(b >> 32);
        return *this;
    }

    base_uint_v2& operator<<=(unsigned int shift);
    base_uint_v2& operator>>=(unsigned int shift);

    base_uint_v2& operator+=(const base_uint_v2& b)
    {
        uint64_t carry = 0;
        for (int i = 0; i < WIDTH; i++)
        {
            uint64_t n = carry + pn[i] + b.pn[i];
            pn[i] = n & 0xffffffff;
            carry = n >> 32;
        }
        return *this;
    }

    base_uint_v2& operator-=(const base_uint_v2& b)
    {
        *this += -b;
        return *this;
    }

    base_uint_v2& operator+=(uint64_t b64)
    {
        base_uint_v2 b;
        b = b64;
        *this += b;
        return *this;
    }

    base_uint_v2& operator-=(uint64_t b64)
    {
        base_uint_v2 b;
        b = b64;
        *this += -b;
        return *this;
    }

    base_uint_v2& operator*=(uint32_t b32);
    base_uint_v2& operator*=(const base_uint_v2& b);
    base_uint_v2& operator/=(const base_uint_v2& b);

    base_uint_v2& operator++()
    {
        // prefix operator
        int i = 0;
        while (i < WIDTH && ++pn[i] == 0)
            i++;
        return *this;
    }

    const base_uint_v2 operator++(int)
    {
        // postfix operator
        const base_uint_v2 ret = *this;
        ++(*this);
        return ret;
    }

    base_uint_v2& operator--()
    {
        // prefix operator
        int i = 0;
        while (i < WIDTH && --pn[i] == (uint32_t)-1)
            i++;
        return *this;
    }

    const base_uint_v2 operator--(int)
    {
        // postfix operator
        const base_uint_v2 ret = *this;
        --(*this);
        return ret;
    }

    int CompareTo(const base_uint_v2& b) const;
    bool EqualTo(uint64_t b) const;

    friend inline const base_uint_v2 operator+(const base_uint_v2& a, const base_uint_v2& b) { return base_uint_v2(a) += b; }
    friend inline const base_uint_v2 operator-(const base_uint_v2& a, const base_uint_v2& b) { return base_uint_v2(a) -= b; }
    friend inline const base_uint_v2 operator*(const base_uint_v2& a, const base_uint_v2& b) { return base_uint_v2(a) *= b; }
    friend inline const base_uint_v2 operator/(const base_uint_v2& a, const base_uint_v2& b) { return base_uint_v2(a) /= b; }
    friend inline const base_uint_v2 operator|(const base_uint_v2& a, const base_uint_v2& b) { return base_uint_v2(a) |= b; }
    friend inline const base_uint_v2 operator&(const base_uint_v2& a, const base_uint_v2& b) { return base_uint_v2(a) &= b; }
    friend inline const base_uint_v2 operator^(const base_uint_v2& a, const base_uint_v2& b) { return base_uint_v2(a) ^= b; }
    friend inline const base_uint_v2 operator>>(const base_uint_v2& a, int shift) { return base_uint_v2(a) >>= shift; }
    friend inline const base_uint_v2 operator<<(const base_uint_v2& a, int shift) { return base_uint_v2(a) <<= shift; }
    friend inline const base_uint_v2 operator*(const base_uint_v2& a, uint32_t b) { return base_uint_v2(a) *= b; }
    friend inline bool operator==(const base_uint_v2& a, const base_uint_v2& b) { return memcmp(a.pn, b.pn, sizeof(a.pn)) == 0; }
    friend inline bool operator!=(const base_uint_v2& a, const base_uint_v2& b) { return memcmp(a.pn, b.pn, sizeof(a.pn)) != 0; }
    friend inline bool operator>(const base_uint_v2& a, const base_uint_v2& b) { return a.CompareTo(b) > 0; }
    friend inline bool operator<(const base_uint_v2& a, const base_uint_v2& b) { return a.CompareTo(b) < 0; }
    friend inline bool operator>=(const base_uint_v2& a, const base_uint_v2& b) { return a.CompareTo(b) >= 0; }
    friend inline bool operator<=(const base_uint_v2& a, const base_uint_v2& b) { return a.CompareTo(b) <= 0; }
    friend inline bool operator==(const base_uint_v2& a, uint64_t b) { return a.EqualTo(b); }
    friend inline bool operator!=(const base_uint_v2& a, uint64_t b) { return !a.EqualTo(b); }

    std::string GetHex() const;
    void SetHex(const char* psz);
    void SetHex(const std::string& str);
    std::string ToString() const;

    unsigned int size() const
    {
        return sizeof(pn);
    }

    /**
     * Returns the position of the highest bit set plus one, or zero if the
     * value is zero.
     */
    unsigned int bits() const;

    uint64_t GetLow64() const
    {
        static_assert(WIDTH >= 2, "Assertion WIDTH >= 2 failed (WIDTH = BITS / 32). BITS is a template parameter.");
        return pn[0] | (uint64_t)pn[1] << 32;
    }
};

/** 256-bit unsigned big integer. */
class arith_uint256 : public base_uint_v2<256> {
public:
    arith_uint256() {}
    arith_uint256(const base_uint_v2<256>& b) : base_uint_v2<256>(b) {}
    arith_uint256(uint64_t b) : base_uint_v2<256>(b) {}
    explicit arith_uint256(const std::string& str) : base_uint_v2<256>(str) {}

    /**
     * The "compact" format is a representation of a whole
     * number N using an unsigned 32bit number similar to a
     * floating point format.
     * The most significant 8 bits are the unsigned exponent of base 256.
     * This exponent can be thought of as "number of bytes of N".
     * The lower 23 bits are the mantissa.
     * Bit number 24 (0x800000) represents the sign of N.
     * N = (-1^sign) * mantissa * 256^(exponent-3)
     *
     * Satoshi's original implementation used BN_bn2mpi() and BN_mpi2bn().
     * MPI uses the most significant bit of the first byte as sign.
     * Thus 0x1234560000 is compact (0x05123456)
     * and  0xc0de000000 is compact (0x0600c0de)
     *
     * Bitcoin only uses this "compact" format for encoding difficulty
     * targets, which are unsigned 256bit quantities.  Thus, all the
     * complexities of the sign bit and using base 256 are probably an
     * implementation accident.
     */
    arith_uint256& SetCompact(uint32_t nCompact, bool *pfNegative = nullptr, bool *pfOverflow = nullptr);
    uint32_t GetCompact(bool fNegative = false) const;

    friend uint256 ArithToUint256(const arith_uint256 &);
    friend arith_uint256 UintToArith256(const uint256 &);
};

uint256 ArithToUint256(const arith_uint256 &);
arith_uint256 UintToArith256(const uint256 &);

#endif // BITCOIN_ARITH_UINT256_H