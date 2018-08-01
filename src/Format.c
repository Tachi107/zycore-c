/***************************************************************************************************

  Zyan Core Library (Zycore-C)

  Original Author : Florian Bernd

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.

***************************************************************************************************/

#include <Zycore/Format.h>
#include <Zycore/LibC.h>

/* ============================================================================================== */
/* Constants                                                                                      */
/* ============================================================================================== */

/* ---------------------------------------------------------------------------------------------- */
/* Defines                                                                                        */
/* ---------------------------------------------------------------------------------------------- */

#define ZYCORE_MAXCHARS_DEC_32 10
#define ZYCORE_MAXCHARS_DEC_64 20
#define ZYCORE_MAXCHARS_HEX_32  8
#define ZYCORE_MAXCHARS_HEX_64 16

/* ---------------------------------------------------------------------------------------------- */
/* Lookup Tables                                                                                  */
/* ---------------------------------------------------------------------------------------------- */

static const char* decimal_lookup =
    "00010203040506070809"
    "10111213141516171819"
    "20212223242526272829"
    "30313233343536373839"
    "40414243444546474849"
    "50515253545556575859"
    "60616263646566676869"
    "70717273747576777879"
    "80818283848586878889"
    "90919293949596979899";

/* ---------------------------------------------------------------------------------------------- */
/* Static strings                                                                                 */
/* ---------------------------------------------------------------------------------------------- */

static const ZyanString sign_add = ZYAN_STRING_WRAP("+");
static const ZyanString sign_sub = ZYAN_STRING_WRAP("-");

/* ---------------------------------------------------------------------------------------------- */

/* ============================================================================================== */
/* Internal macros                                                                                */
/* ============================================================================================== */

/**
 * @brief   Writes a terminating '\0' character at the end of the string data.
 */
#define ZYCORE_STRING_NULLTERMINATE(string) \
      *(char*)((ZyanU8*)(string)->data.data + (string)->data.size - 1) = '\0';

/* ============================================================================================== */
/* Internal functions                                                                             */
/* ============================================================================================== */

/* ---------------------------------------------------------------------------------------------- */
/* Decimal                                                                                        */
/* ---------------------------------------------------------------------------------------------- */

#if defined(ZYAN_X86) || defined(ZYAN_ARM) || defined(ZYAN_EMSCRIPTEN)
ZyanStatus ZyanStringAppendDecU32(ZyanString* string, ZyanU32 value, ZyanU8 padding_length)
{
    if (!string)
    {
        return ZYAN_STATUS_INVALID_ARGUMENT;
    }

    if (string->flags & ZYAN_STRING_IS_IMMUTABLE)
    {
        return ZYAN_STATUS_INVALID_OPERATION;
    }

    char buffer[ZYCORE_MAXCHARS_DEC_32];
    char *buffer_end = &buffer[ZYCORE_MAXCHARS_DEC_32];
    char *buffer_write_pointer = buffer_end;
    while (value >= 100)
    {
        const ZyanU32 value_old = value;
        buffer_write_pointer -= 2;
        value /= 100;
        ZYAN_MEMCPY(buffer_write_pointer, &decimal_lookup[(value_old - (value * 100)) * 2], 2);
    }
    buffer_write_pointer -= 2;
    ZYAN_MEMCPY(buffer_write_pointer, &decimal_lookup[value * 2], 2);

    const ZyanUSize offset_odd    = (ZyanUSize)(value < 10);
    const ZyanUSize length_number = buffer_end - buffer_write_pointer - offset_odd;
    const ZyanUSize length_total  = ZYAN_MAX(length_number, padding_length);
    const ZyanUSize length_target = string->data.size;

    if (string->data.size + length_total > string->data.capacity)
    {
        ZYAN_CHECK(ZyanStringResize(string, string->data.size + length_total - 1));
    }

    ZyanUSize offset_write = 0;
    if (padding_length > length_number)
    {
        offset_write = padding_length - length_number;
        ZYAN_MEMSET((char*)string->data.data + length_target - 1, '0', offset_write);
    }

    ZYAN_MEMCPY((char*)string->data.data + length_target + offset_write - 1,
        buffer_write_pointer + offset_odd, length_number);
    string->data.size = length_target + length_total;
    ZYCORE_STRING_NULLTERMINATE(string);

    return ZYAN_STATUS_SUCCESS;
}
#endif

ZyanStatus ZyanStringAppendDecU64(ZyanString* string, ZyanU64 value, ZyanU8 padding_length)
{
    if (!string)
    {
        return ZYAN_STATUS_INVALID_ARGUMENT;
    }

    if (string->flags & ZYAN_STRING_IS_IMMUTABLE)
    {
        return ZYAN_STATUS_INVALID_OPERATION;
    }

    char buffer[ZYCORE_MAXCHARS_DEC_64];
    char *buffer_end = &buffer[ZYCORE_MAXCHARS_DEC_64];
    char *buffer_write_pointer = buffer_end;
    while (value >= 100)
    {
        const ZyanU64 value_old = value;
        buffer_write_pointer -= 2;
        value /= 100;
        ZYAN_MEMCPY(buffer_write_pointer, &decimal_lookup[(value_old - (value * 100)) * 2], 2);
    }
    buffer_write_pointer -= 2;
    ZYAN_MEMCPY(buffer_write_pointer, &decimal_lookup[value * 2], 2);

    const ZyanUSize offset_odd    = (ZyanUSize)(value < 10);
    const ZyanUSize length_number = buffer_end - buffer_write_pointer - offset_odd;
    const ZyanUSize length_total  = ZYAN_MAX(length_number, padding_length);
    const ZyanUSize length_target = string->data.size;

    if (string->data.size + length_total > string->data.capacity)
    {
        ZYAN_CHECK(ZyanStringResize(string, string->data.size + length_total - 1));
    }

    ZyanUSize offset_write = 0;
    if (padding_length > length_number)
    {
        offset_write = padding_length - length_number;
        ZYAN_MEMSET((char*)string->data.data + length_target - 1, '0', offset_write);
    }

    ZYAN_MEMCPY((char*)string->data.data + length_target + offset_write - 1,
        buffer_write_pointer + offset_odd, length_number);
    string->data.size = length_target + length_total;
    ZYCORE_STRING_NULLTERMINATE(string);

    return ZYAN_STATUS_SUCCESS;
}

/* ---------------------------------------------------------------------------------------------- */
/* Hexadecimal                                                                                    */
/* ---------------------------------------------------------------------------------------------- */

#if defined(ZYAN_X86) || defined(ZYAN_ARM) || defined(ZYAN_EMSCRIPTEN)
ZyanStatus ZyanStringAppendHexU32(ZyanString* string, ZyanU32 value, ZyanU8 padding_length,
    ZyanBool uppercase)
{
    if (!string)
    {
        return ZYAN_STATUS_INVALID_ARGUMENT;
    }

    if (string->flags & ZYAN_STRING_IS_IMMUTABLE)
    {
        return ZYAN_STATUS_INVALID_OPERATION;
    }

    const ZyanUSize len = string->data.size;
    ZyanUSize remaining = string->data.capacity - string->data.size;

    if (remaining < (ZyanUSize)padding_length)
    {
        ZYAN_CHECK(ZyanStringResize(string, len + padding_length - 1));
        remaining = padding_length;
    }

    if (!value)
    {
        const ZyanU8 n = (padding_length ? padding_length : 1);

        if (remaining < (ZyanUSize)n)
        {
            ZYAN_CHECK(ZyanStringResize(string, string->data.size + n - 1));
        }

        ZYAN_MEMSET((char*)string->data.data + len - 1, '0', n);
        string->data.size = len + n;
        ZYCORE_STRING_NULLTERMINATE(string);

        return ZYAN_STATUS_SUCCESS;
    }

    ZyanU8 n = 0;
    char* buffer = ZYAN_NULL;
    for (ZyanI8 i = ZYCORE_MAXCHARS_HEX_32 - 1; i >= 0; --i)
    {
        const ZyanU8 v = (value >> i * 4) & 0x0F;
        if (!n)
        {
            if (!v)
            {
                continue;
            }
            if (remaining <= (ZyanU8)i)
            {
                ZYAN_CHECK(ZyanStringResize(string, string->data.size + i));
            }
            buffer = (char*)string->data.data + len - 1;
            if (padding_length > i)
            {
                n = padding_length - i - 1;
                ZYAN_MEMSET(buffer, '0', n);
            }
        }
        ZYAN_ASSERT(buffer);
        if (uppercase)
        {
            buffer[n++] = "0123456789ABCDEF"[v];
        } else
        {
            buffer[n++] = "0123456789abcdef"[v];
        }
    }
    string->data.size = len + n;
    ZYCORE_STRING_NULLTERMINATE(string);

    return ZYAN_STATUS_SUCCESS;
}
#endif

ZyanStatus ZyanStringAppendHexU64(ZyanString* string, ZyanU64 value, ZyanU8 padding_length,
    ZyanBool uppercase)
{
    if (!string)
    {
        return ZYAN_STATUS_INVALID_ARGUMENT;
    }

    if (string->flags & ZYAN_STRING_IS_IMMUTABLE)
    {
        return ZYAN_STATUS_INVALID_OPERATION;
    }

    const ZyanUSize len = string->data.size;
    ZyanUSize remaining = string->data.capacity - string->data.size;

    if (remaining < (ZyanUSize)padding_length)
    {
        ZYAN_CHECK(ZyanStringResize(string, len + padding_length - 1));
        remaining = padding_length;
    }

    if (!value)
    {
        const ZyanU8 n = (padding_length ? padding_length : 1);

        if (remaining < (ZyanUSize)n)
        {
            ZYAN_CHECK(ZyanStringResize(string, string->data.size + n - 1));
        }

        ZYAN_MEMSET((char*)string->data.data + len - 1, '0', n);
        string->data.size = len + n;
        ZYCORE_STRING_NULLTERMINATE(string);

        return ZYAN_STATUS_SUCCESS;
    }

    ZyanU8 n = 0;
    char* buffer = ZYAN_NULL;
    for (ZyanI8 i = ((value & 0xFFFFFFFF00000000) ?
        ZYCORE_MAXCHARS_HEX_64 : ZYCORE_MAXCHARS_HEX_32) - 1; i >= 0; --i)
    {
        const ZyanU8 v = (value >> i * 4) & 0x0F;
        if (!n)
        {
            if (!v)
            {
                continue;
            }
            if (remaining <= (ZyanU8)i)
            {
                ZYAN_CHECK(ZyanStringResize(string, string->data.size + i));
            }
            buffer = (char*)string->data.data + len - 1;
            if (padding_length > i)
            {
                n = padding_length - i - 1;
                ZYAN_MEMSET(buffer, '0', n);
            }
        }
        ZYAN_ASSERT(buffer);
        if (uppercase)
        {
            buffer[n++] = "0123456789ABCDEF"[v];
        } else
        {
            buffer[n++] = "0123456789abcdef"[v];
        }
    }
    string->data.size = len + n;
    ZYCORE_STRING_NULLTERMINATE(string);

    return ZYAN_STATUS_SUCCESS;
}

/* ---------------------------------------------------------------------------------------------- */

/* ============================================================================================== */
/* Exported functions                                                                             */
/* ============================================================================================== */

/* ---------------------------------------------------------------------------------------------- */
/* Insertion                                                                                      */
/* ---------------------------------------------------------------------------------------------- */

//ZyanStatus ZyanStringInsertFormat(ZyanString* string, ZyanUSize index, const char* format, ...)
//{
//
//}
//
///* ---------------------------------------------------------------------------------------------- */
//
//ZyanStatus ZyanStringInsertDecU(ZyanString* string, ZyanUSize index, ZyanU64 value,
//    ZyanUSize padding_length)
//{
//
//}
//
//ZyanStatus ZyanStringInsertDecS(ZyanString* string, ZyanUSize index, ZyanI64 value,
//    ZyanUSize padding_length, ZyanBool force_sign, const ZyanString* prefix)
//{
//
//}
//
//ZyanStatus ZyanStringInsertHexU(ZyanString* string, ZyanUSize index, ZyanU64 value,
//    ZyanUSize padding_length, ZyanBool uppercase)
//{
//
//}
//
//ZyanStatus ZyanStringInsertHexS(ZyanString* string, ZyanUSize index, ZyanI64 value,
//    ZyanUSize padding_length, ZyanBool uppercase, ZyanBool force_sign, const ZyanString* prefix)
//{
//
//}

/* ---------------------------------------------------------------------------------------------- */
/* Appending                                                                                      */
/* ---------------------------------------------------------------------------------------------- */

ZyanStatus ZyanStringAppendFormat(ZyanString* string, const char* format, ...)
{
    if (!string || !format)
    {
        return ZYAN_STATUS_INVALID_ARGUMENT;
    }

    if (string->flags & ZYAN_STRING_IS_IMMUTABLE)
    {
        return ZYAN_STATUS_INVALID_OPERATION;
    }

    ZyanVAList arglist;
    ZYAN_VA_START(arglist, format);

    const ZyanUSize len = string->data.size;

    ZyanI32 w = ZYAN_VSNPRINTF((char*)string->data.data + len - 1, string->data.capacity - len + 1,
        format, arglist);
    if (w < 0)
    {
        ZYAN_VA_END(arglist);
        return ZYAN_STATUS_FAILED;
    }
    if (w <= (ZyanI32)(string->data.capacity - len))
    {
        string->data.size = len + w;

        ZYAN_VA_END(arglist);
        return ZYAN_STATUS_SUCCESS;
    }

    // The remaining capacity was not sufficent to fit the formatted string. Trying to resize ..
    const ZyanStatus status = ZyanStringResize(string, string->data.size + w - 1);
    if (!ZYAN_SUCCESS(status))
    {
        ZYAN_VA_END(arglist);
        return status;
    }

    w = ZYAN_VSNPRINTF((char*)string->data.data + len - 1,
        string->data.capacity - string->data.size + 1, format, arglist);
    if (w < 0)
    {
        ZYAN_VA_END(arglist);
        return ZYAN_STATUS_FAILED;
    }
    ZYAN_ASSERT(w <= (ZyanI32)(string->data.capacity - string->data.size));

    ZYAN_VA_END(arglist);
    return ZYAN_STATUS_SUCCESS;
}

/* ---------------------------------------------------------------------------------------------- */

ZyanStatus ZyanStringAppendDecU(ZyanString* string, ZyanU64 value, ZyanU8 padding_length)
{
#if defined(ZYAN_X64) || defined(ZYAN_AARCH64)
    return ZyanStringAppendDecU64(string, value, padding_length);
#else
    // Working with 64-bit values is slow on non 64-bit systems
    if (value & 0xFFFFFFFF00000000)
    {
        return ZyanStringAppendDecU64(string, value, padding_length);
    }
    return ZyanStringAppendDecU32(string, (ZyanU32)value, padding_length);
#endif
}

ZyanStatus ZyanStringAppendDecS(ZyanString* string, ZyanI64 value, ZyanU8 padding_length,
    ZyanBool force_sign, const ZyanString* prefix)
{
    if (value < 0)
    {
        ZYAN_CHECK(ZyanStringAppend(string, &sign_sub));
        if (prefix)
        {
            ZYAN_CHECK(ZyanStringAppend(string, prefix));
        }
        return ZyanStringAppendDecU(string, -value, padding_length);
    }

    if (force_sign)
    {
        ZYAN_ASSERT(value >= 0);
        ZYAN_CHECK(ZyanStringAppend(string, &sign_add));
    }

    if (prefix)
    {
        ZYAN_CHECK(ZyanStringAppend(string, prefix));
    }
    return ZyanStringAppendDecU(string, value, padding_length);
}

ZyanStatus ZyanStringAppendHexU(ZyanString* string, ZyanU64 value, ZyanU8 padding_length,
    ZyanBool uppercase)
{
#if defined(ZYAN_X64) || defined(ZYAN_AARCH64)
    return ZyanStringAppendHexU64(string, value, padding_length, uppercase);
#else
    // Working with 64-bit values is slow on non 64-bit systems
    if (value & 0xFFFFFFFF00000000)
    {
        return ZyanStringAppendHexU64(string, value, padding_length, uppercase);
    }
    return ZyanStringAppendHexU32(string, (ZyanU32)value, padding_length, uppercase);
#endif
}

ZyanStatus ZyanStringAppendHexS(ZyanString* string, ZyanI64 value, ZyanU8 padding_length,
    ZyanBool uppercase, ZyanBool force_sign, const ZyanString* prefix)
{
    if (value < 0)
    {
        ZYAN_CHECK(ZyanStringAppend(string, &sign_sub));
        if (prefix)
        {
            ZYAN_CHECK(ZyanStringAppend(string, prefix));
        }
        return ZyanStringAppendHexU(string, -value, padding_length, uppercase);
    }

    if (force_sign)
    {
        ZYAN_ASSERT(value >= 0);
        ZYAN_CHECK(ZyanStringAppend(string, &sign_add));
    }

    if (prefix)
    {
        ZYAN_CHECK(ZyanStringAppend(string, prefix));
    }
    return ZyanStringAppendHexU(string, value, padding_length, uppercase);
}

/* ---------------------------------------------------------------------------------------------- */

/* ============================================================================================== */