//-------------------------------------------------------------------------------------
// DirectXTexConvert.cpp
//
// DirectX Texture Library - Image pixel format conversion
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248926
//-------------------------------------------------------------------------------------

#include "DirectXTexP.h"

using namespace DirectX;
using namespace DirectX::Internal;
using namespace DirectX::PackedVector;
using Microsoft::WRL::ComPtr;

namespace
{
    inline uint32_t FloatTo7e3(float Value) noexcept
    {
        uint32_t IValue = reinterpret_cast<uint32_t *>(&Value)[0];

        if (IValue & 0x80000000U)
        {
            // Positive only
            return 0;
        }
        else if (IValue > 0x41FF73FFU)
        {
            // The number is too large to be represented as a 7e3. Saturate.
            return 0x3FFU;
        }
        else
        {
            if (IValue < 0x3E800000U)
            {
                // The number is too small to be represented as a normalized 7e3.
                // Convert it to a denormalized value.
                const uint32_t Shift = std::min<uint32_t>(125U - (IValue >> 23U), 24U);
                IValue = (0x800000U | (IValue & 0x7FFFFFU)) >> Shift;
            }
            else
            {
                // Rebias the exponent to represent the value as a normalized 7e3.
                IValue += 0xC2000000U;
            }

            return ((IValue + 0x7FFFU + ((IValue >> 16U) & 1U)) >> 16U) & 0x3FFU;
        }
    }

    inline float FloatFrom7e3(uint32_t Value) noexcept
    {
        auto Mantissa = static_cast<uint32_t>(Value & 0x7F);

        uint32_t Exponent = (Value & 0x380);
        if (Exponent != 0)  // The value is normalized
        {
            Exponent = static_cast<uint32_t>((Value >> 7) & 0x7);
        }
        else if (Mantissa != 0)     // The value is denormalized
        {
            // Normalize the value in the resulting float
            Exponent = 1;

            do
            {
                Exponent--;
                Mantissa <<= 1;
            }
            while ((Mantissa & 0x80) == 0);

            Mantissa &= 0x7F;
        }
        else                        // The value is zero
        {
            Exponent = uint32_t(-124);
        }

        const uint32_t Result = ((Exponent + 124) << 23) | // Exponent
            (Mantissa << 16);          // Mantissa

        return reinterpret_cast<const float*>(&Result)[0];
    }

    inline uint32_t FloatTo6e4(float Value) noexcept
    {
        uint32_t IValue = reinterpret_cast<uint32_t *>(&Value)[0];

        if (IValue & 0x80000000U)
        {
            // Positive only
            return 0;
        }
        else if (IValue > 0x43FEFFFFU)
        {
            // The number is too large to be represented as a 6e4. Saturate.
            return 0x3FFU;
        }
        else
        {
            if (IValue < 0x3C800000U)
            {
                // The number is too small to be represented as a normalized 6e4.
                // Convert it to a denormalized value.
                const uint32_t Shift = std::min<uint32_t>(121U - (IValue >> 23U), 24U);
                IValue = (0x800000U | (IValue & 0x7FFFFFU)) >> Shift;
            }
            else
            {
                // Rebias the exponent to represent the value as a normalized 6e4.
                IValue += 0xC4000000U;
            }

            return ((IValue + 0xFFFFU + ((IValue >> 17U) & 1U)) >> 17U) & 0x3FFU;
        }
    }

    inline float FloatFrom6e4(uint32_t Value) noexcept
    {
        uint32_t Mantissa = static_cast<uint32_t>(Value & 0x3F);

        uint32_t Exponent = (Value & 0x3C0);
        if (Exponent != 0)  // The value is normalized
        {
            Exponent = static_cast<uint32_t>((Value >> 6) & 0xF);
        }
        else if (Mantissa != 0)     // The value is denormalized
        {
            // Normalize the value in the resulting float
            Exponent = 1;

            do
            {
                Exponent--;
                Mantissa <<= 1;
            }
            while ((Mantissa & 0x40) == 0);

            Mantissa &= 0x3F;
        }
        else                        // The value is zero
        {
            Exponent = uint32_t(-120);
        }

        const uint32_t Result = ((Exponent + 120) << 23) | // Exponent
            (Mantissa << 17);          // Mantissa

        return reinterpret_cast<const float*>(&Result)[0];
    }

#if DIRECTX_MATH_VERSION >= 310
#define StoreFloat3SE XMStoreFloat3SE
#else
    inline void XM_CALLCONV StoreFloat3SE(_Out_ XMFLOAT3SE* pDestination, DirectX::FXMVECTOR V)
    {
        assert(pDestination);

        DirectX::XMFLOAT3A tmp;
        DirectX::XMStoreFloat3A(&tmp, V);

        constexpr float maxf9 = float(0x1FF << 7);
        constexpr float minf9 = float(1.f / (1 << 16));

        float x = (tmp.x >= 0.f) ? ((tmp.x > maxf9) ? maxf9 : tmp.x) : 0.f;
        float y = (tmp.y >= 0.f) ? ((tmp.y > maxf9) ? maxf9 : tmp.y) : 0.f;
        float z = (tmp.z >= 0.f) ? ((tmp.z > maxf9) ? maxf9 : tmp.z) : 0.f;

        const float max_xy = (x > y) ? x : y;
        const float max_xyz = (max_xy > z) ? max_xy : z;

        const float maxColor = (max_xyz > minf9) ? max_xyz : minf9;

        union { float f; int32_t i; } fi;
        fi.f = maxColor;
        fi.i += 0x00004000; // round up leaving 9 bits in fraction (including assumed 1)

        // Fix applied from DirectXMath 3.10
        uint32_t exp = fi.i >> 23;
        pDestination->e = exp - 0x6f;

        fi.i = 0x83000000 - (exp << 23);
        float ScaleR = fi.f;

        pDestination->xm = static_cast<uint32_t>(lroundf(x * ScaleR));
        pDestination->ym = static_cast<uint32_t>(lroundf(y * ScaleR));
        pDestination->zm = static_cast<uint32_t>(lroundf(z * ScaleR));
    }
#endif

    const XMVECTORF32 g_Grayscale = { { { 0.2125f, 0.7154f, 0.0721f, 0.0f } } };
    const XMVECTORF32 g_HalfMin = { { { -65504.f, -65504.f, -65504.f, -65504.f } } };
    const XMVECTORF32 g_HalfMax = { { { 65504.f, 65504.f, 65504.f, 65504.f } } };
    const XMVECTORF32 g_8BitBias = { { { 0.5f / 255.f, 0.5f / 255.f, 0.5f / 255.f, 0.5f / 255.f } } };
}

//-------------------------------------------------------------------------------------
// Copies an image row with optional clearing of alpha value to 1.0
// (can be used in place as well) otherwise copies the image row unmodified.
//-------------------------------------------------------------------------------------
_Use_decl_annotations_
void DirectX::Internal::CopyScanline(
    void* pDestination,
    size_t outSize,
    const void* pSource,
    size_t inSize,
    DXGI_FORMAT format,
    uint32_t tflags) noexcept
{
    assert(pDestination && outSize > 0);
    assert(pSource && inSize > 0);
    assert(IsValid(format) && !IsPalettized(format));

    if (tflags & TEXP_SCANLINE_SETALPHA)
    {
        switch (static_cast<int>(format))
        {
            //-----------------------------------------------------------------------------
        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_UINT:
        case DXGI_FORMAT_R32G32B32A32_SINT:
            if (inSize >= 16 && outSize >= 16)
            {
                uint32_t alpha;
                if (format == DXGI_FORMAT_R32G32B32A32_FLOAT)
                    alpha = 0x3f800000;
                else if (format == DXGI_FORMAT_R32G32B32A32_SINT)
                    alpha = 0x7fffffff;
                else
                    alpha = 0xffffffff;

                if (pDestination == pSource)
                {
                    auto dPtr = static_cast<uint32_t*>(pDestination);
                    for (size_t count = 0; count < (outSize - 15); count += 16)
                    {
                        dPtr += 3;
                        *(dPtr++) = alpha;
                    }
                }
                else
                {
                    const uint32_t * __restrict sPtr = static_cast<const uint32_t*>(pSource);
                    uint32_t * __restrict dPtr = static_cast<uint32_t*>(pDestination);
                    const size_t size = std::min<size_t>(outSize, inSize);
                    for (size_t count = 0; count < (size - 15); count += 16)
                    {
                        *(dPtr++) = *(sPtr++);
                        *(dPtr++) = *(sPtr++);
                        *(dPtr++) = *(sPtr++);
                        *(dPtr++) = alpha;
                        ++sPtr;
                    }
                }
            }
            return;

            //-----------------------------------------------------------------------------
        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R16G16B16A16_UNORM:
        case DXGI_FORMAT_R16G16B16A16_UINT:
        case DXGI_FORMAT_R16G16B16A16_SNORM:
        case DXGI_FORMAT_R16G16B16A16_SINT:
        case DXGI_FORMAT_Y416:
            if (inSize >= 8 && outSize >= 8)
            {
                uint16_t alpha;
                if (format == DXGI_FORMAT_R16G16B16A16_FLOAT)
                    alpha = 0x3c00;
                else if (format == DXGI_FORMAT_R16G16B16A16_SNORM || format == DXGI_FORMAT_R16G16B16A16_SINT)
                    alpha = 0x7fff;
                else
                    alpha = 0xffff;

                if (pDestination == pSource)
                {
                    auto dPtr = static_cast<uint16_t*>(pDestination);
                    for (size_t count = 0; count < (outSize - 7); count += 8)
                    {
                        dPtr += 3;
                        *(dPtr++) = alpha;
                    }
                }
                else
                {
                    const uint16_t * __restrict sPtr = static_cast<const uint16_t*>(pSource);
                    uint16_t * __restrict dPtr = static_cast<uint16_t*>(pDestination);
                    const size_t size = std::min<size_t>(outSize, inSize);
                    for (size_t count = 0; count < (size - 7); count += 8)
                    {
                        *(dPtr++) = *(sPtr++);
                        *(dPtr++) = *(sPtr++);
                        *(dPtr++) = *(sPtr++);
                        *(dPtr++) = alpha;
                        ++sPtr;
                    }
                }
            }
            return;

            //-----------------------------------------------------------------------------
        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        case DXGI_FORMAT_R10G10B10A2_UNORM:
        case DXGI_FORMAT_R10G10B10A2_UINT:
        case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        case DXGI_FORMAT_Y410:
        case XBOX_DXGI_FORMAT_R10G10B10_7E3_A2_FLOAT:
        case XBOX_DXGI_FORMAT_R10G10B10_6E4_A2_FLOAT:
        case XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM:
            if (inSize >= 4 && outSize >= 4)
            {
                if (pDestination == pSource)
                {
                    auto dPtr = static_cast<uint32_t*>(pDestination);
                    for (size_t count = 0; count < (outSize - 3); count += 4)
                    {
                        *dPtr |= 0xC0000000;
                        ++dPtr;
                    }
                }
                else
                {
                    const uint32_t * __restrict sPtr = static_cast<const uint32_t*>(pSource);
                    uint32_t * __restrict dPtr = static_cast<uint32_t*>(pDestination);
                    const size_t size = std::min<size_t>(outSize, inSize);
                    for (size_t count = 0; count < (size - 3); count += 4)
                    {
                        *(dPtr++) = *(sPtr++) | 0xC0000000;
                    }
                }
            }
            return;

            //-----------------------------------------------------------------------------
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R8G8B8A8_SINT:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        case DXGI_FORMAT_AYUV:
            if (inSize >= 4 && outSize >= 4)
            {
                const uint32_t alpha = (format == DXGI_FORMAT_R8G8B8A8_SNORM || format == DXGI_FORMAT_R8G8B8A8_SINT) ? 0x7f000000 : 0xff000000;

                if (pDestination == pSource)
                {
                    auto dPtr = static_cast<uint32_t*>(pDestination);
                    for (size_t count = 0; count < (outSize - 3); count += 4)
                    {
                        uint32_t t = *dPtr & 0xFFFFFF;
                        t |= alpha;
                        *(dPtr++) = t;
                    }
                }
                else
                {
                    const uint32_t * __restrict sPtr = static_cast<const uint32_t*>(pSource);
                    uint32_t * __restrict dPtr = static_cast<uint32_t*>(pDestination);
                    const size_t size = std::min<size_t>(outSize, inSize);
                    for (size_t count = 0; count < (size - 3); count += 4)
                    {
                        uint32_t t = *(sPtr++) & 0xFFFFFF;
                        t |= alpha;
                        *(dPtr++) = t;
                    }
                }
            }
            return;

            //-----------------------------------------------------------------------------
        case DXGI_FORMAT_B5G5R5A1_UNORM:
        case DXGI_FORMAT_B4G4R4A4_UNORM:
        case WIN11_DXGI_FORMAT_A4B4G4R4_UNORM:
            if (inSize >= 2 && outSize >= 2)
            {
                uint16_t alpha;
                if (format == DXGI_FORMAT_B4G4R4A4_UNORM)
                    alpha = 0xF000;
                else if (format == WIN11_DXGI_FORMAT_A4B4G4R4_UNORM)
                    alpha = 0x000F;
                else
                    alpha = 0x8000;

                if (pDestination == pSource)
                {
                    auto dPtr = static_cast<uint16_t*>(pDestination);
                    for (size_t count = 0; count < (outSize - 1); count += 2)
                    {
                        *(dPtr++) |= alpha;
                    }
                }
                else
                {
                    const uint16_t * __restrict sPtr = static_cast<const uint16_t*>(pSource);
                    uint16_t * __restrict dPtr = static_cast<uint16_t*>(pDestination);
                    const size_t size = std::min<size_t>(outSize, inSize);
                    for (size_t count = 0; count < (size - 1); count += 2)
                    {
                        *(dPtr++) = uint16_t(*(sPtr++) | alpha);
                    }
                }
            }
            return;

            //-----------------------------------------------------------------------------
        case DXGI_FORMAT_A8_UNORM:
            memset(pDestination, 0xff, outSize);
            return;

        default:
            break;
        }
    }

    // Fall-through case is to just use memcpy (assuming this is not an in-place operation)
    if (pDestination == pSource)
        return;

    const size_t size = std::min<size_t>(outSize, inSize);
    memcpy(pDestination, pSource, size);
}


//-------------------------------------------------------------------------------------
// Swizzles (RGB <-> BGR) an image row with optional clearing of alpha value to 1.0
// (can be used in place as well) otherwise copies the image row unmodified.
//-------------------------------------------------------------------------------------
_Use_decl_annotations_
void DirectX::Internal::SwizzleScanline(
    void* pDestination,
    size_t outSize,
    const void* pSource,
    size_t inSize,
    DXGI_FORMAT format,
    uint32_t tflags) noexcept
{
    assert(pDestination && outSize > 0);
    assert(pSource && inSize > 0);
    assert(IsValid(format) && !IsPlanar(format) && !IsPalettized(format));

    switch (static_cast<int>(format))
    {
        //---------------------------------------------------------------------------------
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
    case XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM:
        if (inSize >= 4 && outSize >= 4)
        {
            if (tflags & TEXP_SCANLINE_LEGACY)
            {
                // Swap Red (R) and Blue (B) channel (used for D3DFMT_A2R10G10B10 legacy sources)
                if (pDestination == pSource)
                {
                    auto dPtr = static_cast<uint32_t*>(pDestination);
                    for (size_t count = 0; count < (outSize - 3); count += 4)
                    {
                        const uint32_t t = *dPtr;

                        uint32_t t1 = (t & 0x3ff00000) >> 20;
                        uint32_t t2 = (t & 0x000003ff) << 20;
                        uint32_t t3 = (t & 0x000ffc00);
                        uint32_t ta = (tflags & TEXP_SCANLINE_SETALPHA) ? 0xC0000000 : (t & 0xC0000000);

                        *(dPtr++) = t1 | t2 | t3 | ta;
                    }
                }
                else
                {
                    const uint32_t * __restrict sPtr = static_cast<const uint32_t*>(pSource);
                    uint32_t * __restrict dPtr = static_cast<uint32_t*>(pDestination);
                    const size_t size = std::min<size_t>(outSize, inSize);
                    for (size_t count = 0; count < (size - 3); count += 4)
                    {
                        const uint32_t t = *(sPtr++);

                        uint32_t t1 = (t & 0x3ff00000) >> 20;
                        uint32_t t2 = (t & 0x000003ff) << 20;
                        uint32_t t3 = (t & 0x000ffc00);
                        uint32_t ta = (tflags & TEXP_SCANLINE_SETALPHA) ? 0xC0000000 : (t & 0xC0000000);

                        *(dPtr++) = t1 | t2 | t3 | ta;
                    }
                }
                return;
            }
        }
        break;

        //---------------------------------------------------------------------------------
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        if (inSize >= 4 && outSize >= 4)
        {
            // Swap Red (R) and Blue (B) channels (used to convert from DXGI 1.1 BGR formats to DXGI 1.0 RGB)
            if (pDestination == pSource)
            {
                auto dPtr = static_cast<uint32_t*>(pDestination);
                for (size_t count = 0; count < (outSize - 3); count += 4)
                {
                    const uint32_t t = *dPtr;

                    uint32_t t1 = (t & 0x00ff0000) >> 16;
                    uint32_t t2 = (t & 0x000000ff) << 16;
                    uint32_t t3 = (t & 0x0000ff00);
                    uint32_t ta = (tflags & TEXP_SCANLINE_SETALPHA) ? 0xff000000 : (t & 0xFF000000);

                    *(dPtr++) = t1 | t2 | t3 | ta;
                }
            }
            else
            {
                const uint32_t * __restrict sPtr = static_cast<const uint32_t*>(pSource);
                uint32_t * __restrict dPtr = static_cast<uint32_t*>(pDestination);
                const size_t size = std::min<size_t>(outSize, inSize);
                for (size_t count = 0; count < (size - 3); count += 4)
                {
                    const uint32_t t = *(sPtr++);

                    uint32_t t1 = (t & 0x00ff0000) >> 16;
                    uint32_t t2 = (t & 0x000000ff) << 16;
                    uint32_t t3 = (t & 0x0000ff00);
                    uint32_t ta = (tflags & TEXP_SCANLINE_SETALPHA) ? 0xff000000 : (t & 0xFF000000);

                    *(dPtr++) = t1 | t2 | t3 | ta;
                }
            }
            return;
        }
        break;

        //---------------------------------------------------------------------------------
    case DXGI_FORMAT_YUY2:
        if (inSize >= 4 && outSize >= 4)
        {
            if (tflags & TEXP_SCANLINE_LEGACY)
            {
                // Reorder YUV components (used to convert legacy UYVY -> YUY2)
                if (pDestination == pSource)
                {
                    auto dPtr = static_cast<uint32_t*>(pDestination);
                    for (size_t count = 0; count < (outSize - 3); count += 4)
                    {
                        const uint32_t t = *dPtr;

                        uint32_t t1 = (t & 0x000000ff) << 8;
                        uint32_t t2 = (t & 0x0000ff00) >> 8;
                        uint32_t t3 = (t & 0x00ff0000) << 8;
                        uint32_t t4 = (t & 0xff000000) >> 8;

                        *(dPtr++) = t1 | t2 | t3 | t4;
                    }
                }
                else
                {
                    const uint32_t * __restrict sPtr = static_cast<const uint32_t*>(pSource);
                    uint32_t * __restrict dPtr = static_cast<uint32_t*>(pDestination);
                    const size_t size = std::min<size_t>(outSize, inSize);
                    for (size_t count = 0; count < (size - 3); count += 4)
                    {
                        const uint32_t t = *(sPtr++);

                        uint32_t t1 = (t & 0x000000ff) << 8;
                        uint32_t t2 = (t & 0x0000ff00) >> 8;
                        uint32_t t3 = (t & 0x00ff0000) << 8;
                        uint32_t t4 = (t & 0xff000000) >> 8;

                        *(dPtr++) = t1 | t2 | t3 | t4;
                    }
                }
                return;
            }
        }
        break;

    default:
        break;
    }

    // Fall-through case is to just use memcpy (assuming this is not an in-place operation)
    if (pDestination == pSource)
        return;

    const size_t size = std::min<size_t>(outSize, inSize);
    memcpy(pDestination, pSource, size);
}


//-------------------------------------------------------------------------------------
// Converts an image row with optional clearing of alpha value to 1.0
// Returns true if supported, false if expansion case not supported
//-------------------------------------------------------------------------------------
_Use_decl_annotations_
bool DirectX::Internal::ExpandScanline(
    void* pDestination,
    size_t outSize,
    DXGI_FORMAT outFormat,
    const void* pSource,
    size_t inSize,
    DXGI_FORMAT inFormat,
    uint32_t tflags) noexcept
{
    assert(pDestination && outSize > 0);
    assert(pSource && inSize > 0);
    assert(IsValid(outFormat) && !IsPlanar(outFormat) && !IsPalettized(outFormat));
    assert(IsValid(inFormat) && !IsPlanar(inFormat) && !IsPalettized(inFormat));

    switch (static_cast<int>(inFormat))
    {
    case DXGI_FORMAT_B5G6R5_UNORM:
        if (outFormat != DXGI_FORMAT_R8G8B8A8_UNORM)
            return false;

        // DXGI_FORMAT_B5G6R5_UNORM -> DXGI_FORMAT_R8G8B8A8_UNORM
        if (inSize >= 2 && outSize >= 4)
        {
            const uint16_t * __restrict sPtr = static_cast<const uint16_t*>(pSource);
            uint32_t * __restrict dPtr = static_cast<uint32_t*>(pDestination);

            for (size_t ocount = 0, icount = 0; ((icount < (inSize - 1)) && (ocount < (outSize - 3))); icount += 2, ocount += 4)
            {
                const uint16_t t = *(sPtr++);

                uint32_t t1 = uint32_t(((t & 0xf800) >> 8) | ((t & 0xe000) >> 13));
                uint32_t t2 = uint32_t(((t & 0x07e0) << 5) | ((t & 0x0600) >> 5));
                uint32_t t3 = uint32_t(((t & 0x001f) << 19) | ((t & 0x001c) << 14));

                *(dPtr++) = t1 | t2 | t3 | 0xff000000;
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_B5G5R5A1_UNORM:
        if (outFormat != DXGI_FORMAT_R8G8B8A8_UNORM)
            return false;

        // DXGI_FORMAT_B5G5R5A1_UNORM -> DXGI_FORMAT_R8G8B8A8_UNORM
        if (inSize >= 2 && outSize >= 4)
        {
            const uint16_t * __restrict sPtr = static_cast<const uint16_t*>(pSource);
            uint32_t * __restrict dPtr = static_cast<uint32_t*>(pDestination);

            for (size_t ocount = 0, icount = 0; ((icount < (inSize - 1)) && (ocount < (outSize - 3))); icount += 2, ocount += 4)
            {
                const uint16_t t = *(sPtr++);

                uint32_t t1 = uint32_t(((t & 0x7c00) >> 7) | ((t & 0x7000) >> 12));
                uint32_t t2 = uint32_t(((t & 0x03e0) << 6) | ((t & 0x0380) << 1));
                uint32_t t3 = uint32_t(((t & 0x001f) << 19) | ((t & 0x001c) << 14));
                uint32_t ta = (tflags & TEXP_SCANLINE_SETALPHA) ? 0xff000000 : ((t & 0x8000) ? 0xff000000 : 0);

                *(dPtr++) = t1 | t2 | t3 | ta;
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_B4G4R4A4_UNORM:
        if (outFormat != DXGI_FORMAT_R8G8B8A8_UNORM)
            return false;

        // DXGI_FORMAT_B4G4R4A4_UNORM -> DXGI_FORMAT_R8G8B8A8_UNORM
        if (inSize >= 2 && outSize >= 4)
        {
            const uint16_t * __restrict sPtr = static_cast<const uint16_t*>(pSource);
            uint32_t * __restrict dPtr = static_cast<uint32_t*>(pDestination);

            for (size_t ocount = 0, icount = 0; ((icount < (inSize - 1)) && (ocount < (outSize - 3))); icount += 2, ocount += 4)
            {
                const uint16_t t = *(sPtr++);

                uint32_t t1 = uint32_t(((t & 0x0f00) >> 4) | ((t & 0x0f00) >> 8));
                uint32_t t2 = uint32_t(((t & 0x00f0) << 8) | ((t & 0x00f0) << 4));
                uint32_t t3 = uint32_t(((t & 0x000f) << 20) | ((t & 0x000f) << 16));
                uint32_t ta = (tflags & TEXP_SCANLINE_SETALPHA) ? 0xff000000 : uint32_t(((t & 0xf000) << 16) | ((t & 0xf000) << 12));

                *(dPtr++) = t1 | t2 | t3 | ta;
            }
            return true;
        }
        return false;

    case WIN11_DXGI_FORMAT_A4B4G4R4_UNORM:
        if (outFormat != DXGI_FORMAT_R8G8B8A8_UNORM)
            return false;

        // DXGI_FORMAT_A4B4G4R4_UNORM -> DXGI_FORMAT_R8G8B8A8_UNORM
        if (inSize >= 2 && outSize >= 4)
        {
            const uint16_t * __restrict sPtr = static_cast<const uint16_t*>(pSource);
            uint32_t * __restrict dPtr = static_cast<uint32_t*>(pDestination);

            for (size_t ocount = 0, icount = 0; ((icount < (inSize - 1)) && (ocount < (outSize - 3))); icount += 2, ocount += 4)
            {
                const uint16_t t = *(sPtr++);

                uint32_t t1 = uint32_t(((t & 0xf000) >> 8) | ((t & 0xf000) >> 12));
                uint32_t t2 = uint32_t((t & 0x0f00) | ((t & 0x0f00) << 4));
                uint32_t t3 = uint32_t(((t & 0x00f0) << 16) | ((t & 0x00f0) << 12));
                uint32_t ta = (tflags & TEXP_SCANLINE_SETALPHA) ? 0xff000000 : uint32_t(((t & 0x000f) << 28) | ((t & 0x000f) << 24));

                *(dPtr++) = t1 | t2 | t3 | ta;
            }
            return true;
        }
        return false;

    default:
        return false;
    }
}


//-------------------------------------------------------------------------------------
// Loads an image row into standard RGBA XMVECTOR (aligned) array
//-------------------------------------------------------------------------------------
#define LOAD_SCANLINE( type, func )\
        if (size >= sizeof(type))\
        {\
            const type * __restrict sPtr = reinterpret_cast<const type*>(pSource);\
            for(size_t icount = 0; icount < (size - sizeof(type) + 1); icount += sizeof(type))\
            {\
                if (dPtr >= ePtr) break;\
                *(dPtr++) = func(sPtr++);\
            }\
            return true;\
        }\
        return false;

#define LOAD_SCANLINE3( type, func, defvec )\
        if (size >= sizeof(type))\
        {\
            const type * __restrict sPtr = reinterpret_cast<const type*>(pSource);\
            for(size_t icount = 0; icount < (size - sizeof(type) + 1); icount += sizeof(type))\
            {\
                const XMVECTOR v = func(sPtr++);\
                if (dPtr >= ePtr) break;\
                *(dPtr++) = XMVectorSelect(defvec, v, g_XMSelect1110);\
            }\
            return true;\
        }\
        return false;

#define LOAD_SCANLINE2( type, func, defvec )\
        if (size >= sizeof(type))\
        {\
            const type * __restrict sPtr = reinterpret_cast<const type*>(pSource);\
            for(size_t icount = 0; icount < (size - sizeof(type) + 1); icount += sizeof(type))\
            {\
                const XMVECTOR v = func(sPtr++);\
                if (dPtr >= ePtr) break;\
                *(dPtr++) = XMVectorSelect(defvec, v, g_XMSelect1100);\
            }\
            return true;\
        }\
        return false;

#pragma warning(suppress: 6101)
_Use_decl_annotations_ bool DirectX::Internal::LoadScanline(
    XMVECTOR* pDestination,
    size_t count,
    const void* pSource,
    size_t size,
    DXGI_FORMAT format) noexcept
{
    assert(pDestination && count > 0 && ((reinterpret_cast<uintptr_t>(pDestination) & 0xF) == 0));
    assert(pSource && size > 0);
    assert(IsValid(format) && !IsTypeless(format, false) && !IsCompressed(format) && !IsPlanar(format) && !IsPalettized(format));

    XMVECTOR* __restrict dPtr = pDestination;
    if (!dPtr)
        return false;

    const XMVECTOR* ePtr = pDestination + count;

    switch (static_cast<int>(format))
    {
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
        {
            const size_t msize = (size > (sizeof(XMVECTOR)*count)) ? (sizeof(XMVECTOR)*count) : size;
            memcpy(dPtr, pSource, msize);
        }
        return true;

    case DXGI_FORMAT_R32G32B32A32_UINT:
        LOAD_SCANLINE(XMUINT4, XMLoadUInt4)

    case DXGI_FORMAT_R32G32B32A32_SINT:
        LOAD_SCANLINE(XMINT4, XMLoadSInt4)

    case DXGI_FORMAT_R32G32B32_FLOAT:
        LOAD_SCANLINE3(XMFLOAT3, XMLoadFloat3, g_XMIdentityR3)

    case DXGI_FORMAT_R32G32B32_UINT:
        LOAD_SCANLINE3(XMUINT3, XMLoadUInt3, g_XMIdentityR3)

    case DXGI_FORMAT_R32G32B32_SINT:
        LOAD_SCANLINE3(XMINT3, XMLoadSInt3, g_XMIdentityR3)

    case DXGI_FORMAT_R16G16B16A16_FLOAT:
        LOAD_SCANLINE(XMHALF4, XMLoadHalf4)

    case DXGI_FORMAT_R16G16B16A16_UNORM:
        LOAD_SCANLINE(XMUSHORTN4, XMLoadUShortN4)

    case DXGI_FORMAT_R16G16B16A16_UINT:
        LOAD_SCANLINE(XMUSHORT4, XMLoadUShort4)

    case DXGI_FORMAT_R16G16B16A16_SNORM:
        LOAD_SCANLINE(XMSHORTN4, XMLoadShortN4)

    case DXGI_FORMAT_R16G16B16A16_SINT:
        LOAD_SCANLINE(XMSHORT4, XMLoadShort4)

    case DXGI_FORMAT_R32G32_FLOAT:
        LOAD_SCANLINE2(XMFLOAT2, XMLoadFloat2, g_XMIdentityR3)

    case DXGI_FORMAT_R32G32_UINT:
        LOAD_SCANLINE2(XMUINT2, XMLoadUInt2, g_XMIdentityR3)

    case DXGI_FORMAT_R32G32_SINT:
        LOAD_SCANLINE2(XMINT2, XMLoadSInt2, g_XMIdentityR3)

    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            {
                constexpr size_t psize = sizeof(float) + sizeof(uint32_t);
                if (size >= psize)
                {
                    auto sPtr = static_cast<const float*>(pSource);
                    for (size_t icount = 0; icount < (size - psize + 1); icount += psize)
                    {
                        auto ps8 = reinterpret_cast<const uint8_t*>(&sPtr[1]);
                        if (dPtr >= ePtr) break;
                        *(dPtr++) = XMVectorSet(sPtr[0], static_cast<float>(*ps8), 0.f, 1.f);
                        sPtr += 2;
                    }
                    return true;
                }
            }
            return false;

    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        {
            constexpr size_t psize = sizeof(float) + sizeof(uint32_t);
            if (size >= psize)
            {
                auto sPtr = static_cast<const float*>(pSource);
                for (size_t icount = 0; icount < (size - psize + 1); icount += psize)
                {
                    if (dPtr >= ePtr) break;
                    *(dPtr++) = XMVectorSet(sPtr[0], 0.f /* typeless component assumed zero */, 0.f, 1.f);
                    sPtr += 2;
                }
                return true;
            }
        }
        return false;

    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        {
            constexpr size_t psize = sizeof(float) + sizeof(uint32_t);
            if (size >= psize)
            {
                auto sPtr = static_cast<const float*>(pSource);
                for (size_t icount = 0; icount < (size - psize + 1); icount += psize)
                {
                    auto pg8 = reinterpret_cast<const uint8_t*>(&sPtr[1]);
                    if (dPtr >= ePtr) break;
                    *(dPtr++) = XMVectorSet(0.f /* typeless component assumed zero */, static_cast<float>(*pg8), 0.f, 1.f);
                    sPtr += 2;
                }
                return true;
            }
        }
        return false;

    case DXGI_FORMAT_R10G10B10A2_UNORM:
        LOAD_SCANLINE(XMUDECN4, XMLoadUDecN4)

    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        LOAD_SCANLINE(XMUDECN4, XMLoadUDecN4_XR)

    case DXGI_FORMAT_R10G10B10A2_UINT:
        LOAD_SCANLINE(XMUDEC4, XMLoadUDec4)

    case DXGI_FORMAT_R11G11B10_FLOAT:
        LOAD_SCANLINE3(XMFLOAT3PK, XMLoadFloat3PK, g_XMIdentityR3)

    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        LOAD_SCANLINE(XMUBYTEN4, XMLoadUByteN4)

    case DXGI_FORMAT_R8G8B8A8_UINT:
        LOAD_SCANLINE(XMUBYTE4, XMLoadUByte4)

    case DXGI_FORMAT_R8G8B8A8_SNORM:
        LOAD_SCANLINE(XMBYTEN4, XMLoadByteN4)

    case DXGI_FORMAT_R8G8B8A8_SINT:
        LOAD_SCANLINE(XMBYTE4, XMLoadByte4)

    case DXGI_FORMAT_R16G16_FLOAT:
        LOAD_SCANLINE2(XMHALF2, XMLoadHalf2, g_XMIdentityR3)

    case DXGI_FORMAT_R16G16_UNORM:
        LOAD_SCANLINE2(XMUSHORTN2, XMLoadUShortN2, g_XMIdentityR3)

    case DXGI_FORMAT_R16G16_UINT:
        LOAD_SCANLINE2(XMUSHORT2, XMLoadUShort2, g_XMIdentityR3)

    case DXGI_FORMAT_R16G16_SNORM:
        LOAD_SCANLINE2(XMSHORTN2, XMLoadShortN2, g_XMIdentityR3)

    case DXGI_FORMAT_R16G16_SINT:
        LOAD_SCANLINE2(XMSHORT2, XMLoadShort2, g_XMIdentityR3)

    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
        if (size >= sizeof(float))
        {
            const float* __restrict sPtr = static_cast<const float*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(float) + 1); icount += sizeof(float))
            {
                const XMVECTOR v = XMLoadFloat(sPtr++);
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSelect(g_XMIdentityR3, v, g_XMSelect1000);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R32_UINT:
        if (size >= sizeof(uint32_t))
        {
            const uint32_t* __restrict sPtr = static_cast<const uint32_t*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(uint32_t) + 1); icount += sizeof(uint32_t))
            {
                XMVECTOR v = XMLoadInt(sPtr++);
                v = XMConvertVectorUIntToFloat(v, 0);
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSelect(g_XMIdentityR3, v, g_XMSelect1000);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R32_SINT:
        if (size >= sizeof(int32_t))
        {
            const int32_t * __restrict sPtr = static_cast<const int32_t*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(int32_t) + 1); icount += sizeof(int32_t))
            {
                XMVECTOR v = XMLoadInt(reinterpret_cast<const uint32_t*>(sPtr++));
                v = XMConvertVectorIntToFloat(v, 0);
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSelect(g_XMIdentityR3, v, g_XMSelect1000);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        if (size >= sizeof(uint32_t))
        {
            auto sPtr = static_cast<const uint32_t*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(uint32_t) + 1); icount += sizeof(uint32_t))
            {
                const auto d = static_cast<float>(*sPtr & 0xFFFFFF) / 16777215.f;
                const auto s = static_cast<float>((*sPtr & 0xFF000000) >> 24);
                ++sPtr;
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSet(d, s, 0.f, 1.f);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        if (size >= sizeof(uint32_t))
        {
            auto sPtr = static_cast<const uint32_t*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(uint32_t) + 1); icount += sizeof(uint32_t))
            {
                const auto r = static_cast<float>(*sPtr & 0xFFFFFF) / 16777215.f;
                ++sPtr;
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSet(r, 0.f /* typeless component assumed zero */, 0.f, 1.f);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        if (size >= sizeof(uint32_t))
        {
            auto sPtr = static_cast<const uint32_t*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(uint32_t) + 1); icount += sizeof(uint32_t))
            {
                const auto g = static_cast<float>((*sPtr & 0xFF000000) >> 24);
                ++sPtr;
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSet(0.f /* typeless component assumed zero */, g, 0.f, 1.f);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R8G8_UNORM:
        LOAD_SCANLINE2(XMUBYTEN2, XMLoadUByteN2, g_XMIdentityR3)

    case DXGI_FORMAT_R8G8_UINT:
        LOAD_SCANLINE2(XMUBYTE2, XMLoadUByte2, g_XMIdentityR3)

    case DXGI_FORMAT_R8G8_SNORM:
        LOAD_SCANLINE2(XMBYTEN2, XMLoadByteN2, g_XMIdentityR3)

    case DXGI_FORMAT_R8G8_SINT:
        LOAD_SCANLINE2(XMBYTE2, XMLoadByte2, g_XMIdentityR3)

    case DXGI_FORMAT_R16_FLOAT:
        if (size >= sizeof(HALF))
        {
            const HALF * __restrict sPtr = static_cast<const HALF*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(HALF) + 1); icount += sizeof(HALF))
            {
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSet(XMConvertHalfToFloat(*sPtr++), 0.f, 0.f, 1.f);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
        if (size >= sizeof(uint16_t))
        {
            const uint16_t* __restrict sPtr = static_cast<const uint16_t*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(uint16_t) + 1); icount += sizeof(uint16_t))
            {
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSet(static_cast<float>(*sPtr++) / 65535.f, 0.f, 0.f, 1.f);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R16_UINT:
        if (size >= sizeof(uint16_t))
        {
            const uint16_t * __restrict sPtr = static_cast<const uint16_t*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(uint16_t) + 1); icount += sizeof(uint16_t))
            {
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSet(static_cast<float>(*sPtr++), 0.f, 0.f, 1.f);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R16_SNORM:
        if (size >= sizeof(int16_t))
        {
            const int16_t * __restrict sPtr = static_cast<const int16_t*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(int16_t) + 1); icount += sizeof(int16_t))
            {
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSet(static_cast<float>(*sPtr++) / 32767.f, 0.f, 0.f, 1.f);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R16_SINT:
        if (size >= sizeof(int16_t))
        {
            const int16_t * __restrict sPtr = static_cast<const int16_t*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(int16_t) + 1); icount += sizeof(int16_t))
            {
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSet(static_cast<float>(*sPtr++), 0.f, 0.f, 1.f);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R8_UNORM:
        if (size >= sizeof(uint8_t))
        {
            const uint8_t * __restrict sPtr = static_cast<const uint8_t*>(pSource);
            for (size_t icount = 0; icount < size; icount += sizeof(uint8_t))
            {
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSet(static_cast<float>(*sPtr++) / 255.f, 0.f, 0.f, 1.f);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R8_UINT:
        if (size >= sizeof(uint8_t))
        {
            const uint8_t * __restrict sPtr = static_cast<const uint8_t*>(pSource);
            for (size_t icount = 0; icount < size; icount += sizeof(uint8_t))
            {
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSet(static_cast<float>(*sPtr++), 0.f, 0.f, 1.f);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R8_SNORM:
        if (size >= sizeof(int8_t))
        {
            const int8_t * __restrict sPtr = static_cast<const int8_t*>(pSource);
            for (size_t icount = 0; icount < size; icount += sizeof(int8_t))
            {
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSet(static_cast<float>(*sPtr++) / 127.f, 0.f, 0.f, 1.f);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R8_SINT:
        if (size >= sizeof(int8_t))
        {
            const int8_t * __restrict sPtr = static_cast<const int8_t*>(pSource);
            for (size_t icount = 0; icount < size; icount += sizeof(int8_t))
            {
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSet(static_cast<float>(*sPtr++), 0.f, 0.f, 1.f);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_A8_UNORM:
        if (size >= sizeof(uint8_t))
        {
            const uint8_t * __restrict sPtr = static_cast<const uint8_t*>(pSource);
            for (size_t icount = 0; icount < size; icount += sizeof(uint8_t))
            {
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSet(0.f, 0.f, 0.f, static_cast<float>(*sPtr++) / 255.f);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R1_UNORM:
        if (size >= sizeof(uint8_t))
        {
            const uint8_t * __restrict sPtr = static_cast<const uint8_t*>(pSource);
            for (size_t icount = 0; icount < size; icount += sizeof(uint8_t))
            {
                for (size_t bcount = 8; bcount > 0; --bcount)
                {
                    if (dPtr >= ePtr) break;
                    *(dPtr++) = XMVectorSet((((*sPtr >> (bcount - 1)) & 0x1) ? 1.f : 0.f), 0.f, 0.f, 1.f);
                }

                ++sPtr;
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
        LOAD_SCANLINE3(XMFLOAT3SE, XMLoadFloat3SE, g_XMIdentityR3)

    case DXGI_FORMAT_R8G8_B8G8_UNORM:
        if (size >= sizeof(XMUBYTEN4))
        {
            const XMUBYTEN4 * __restrict sPtr = static_cast<const XMUBYTEN4*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(XMUBYTEN4) + 1); icount += sizeof(XMUBYTEN4))
            {
                const XMVECTOR v = XMLoadUByteN4(sPtr++);
                const XMVECTOR v1 = XMVectorSwizzle<0, 3, 2, 1>(v);
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSelect(g_XMIdentityR3, v, g_XMSelect1110);
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSelect(g_XMIdentityR3, v1, g_XMSelect1110);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_G8R8_G8B8_UNORM:
        if (size >= sizeof(XMUBYTEN4))
        {
            const XMUBYTEN4 * __restrict sPtr = static_cast<const XMUBYTEN4*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(XMUBYTEN4) + 1); icount += sizeof(XMUBYTEN4))
            {
                const XMVECTOR v = XMLoadUByteN4(sPtr++);
                const XMVECTOR v0 = XMVectorSwizzle<1, 0, 3, 2>(v);
                const XMVECTOR v1 = XMVectorSwizzle<1, 2, 3, 0>(v);
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSelect(g_XMIdentityR3, v0, g_XMSelect1110);
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSelect(g_XMIdentityR3, v1, g_XMSelect1110);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_B5G6R5_UNORM:
        if (size >= sizeof(XMU565))
        {
            static const XMVECTORF32 s_Scale = { { { 1.f / 31.f, 1.f / 63.f, 1.f / 31.f, 1.f } } };
            const XMU565 * __restrict sPtr = static_cast<const XMU565*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(XMU565) + 1); icount += sizeof(XMU565))
            {
                XMVECTOR v = XMLoadU565(sPtr++);
                v = XMVectorMultiply(v, s_Scale);
                v = XMVectorSwizzle<2, 1, 0, 3>(v);
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSelect(g_XMIdentityR3, v, g_XMSelect1110);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_B5G5R5A1_UNORM:
        if (size >= sizeof(XMU555))
        {
            static const XMVECTORF32 s_Scale = { { { 1.f / 31.f, 1.f / 31.f, 1.f / 31.f, 1.f } } };
            const XMU555 * __restrict sPtr = static_cast<const XMU555*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(XMU555) + 1); icount += sizeof(XMU555))
            {
                XMVECTOR v = XMLoadU555(sPtr++);
                v = XMVectorMultiply(v, s_Scale);
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSwizzle<2, 1, 0, 3>(v);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        if (size >= sizeof(XMUBYTEN4))
        {
            const XMUBYTEN4 * __restrict sPtr = static_cast<const XMUBYTEN4*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(XMUBYTEN4) + 1); icount += sizeof(XMUBYTEN4))
            {
                const XMVECTOR v = XMLoadUByteN4(sPtr++);
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSwizzle<2, 1, 0, 3>(v);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        if (size >= sizeof(XMUBYTEN4))
        {
            const XMUBYTEN4 * __restrict sPtr = static_cast<const XMUBYTEN4*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(XMUBYTEN4) + 1); icount += sizeof(XMUBYTEN4))
            {
                XMVECTOR v = XMLoadUByteN4(sPtr++);
                v = XMVectorSwizzle<2, 1, 0, 3>(v);
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSelect(g_XMIdentityR3, v, g_XMSelect1110);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_AYUV:
        if (size >= sizeof(XMUBYTEN4))
        {
            const XMUBYTEN4 * __restrict sPtr = static_cast<const XMUBYTEN4*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(XMUBYTEN4) + 1); icount += sizeof(XMUBYTEN4))
            {
                const int v = int(sPtr->x) - 128;
                const int u = int(sPtr->y) - 128;
                const int y = int(sPtr->z) - 16;
                const unsigned int a = sPtr->w;
                ++sPtr;

                // http://msdn.microsoft.com/en-us/library/windows/desktop/dd206750.aspx

                // Y'  = Y - 16
                // Cb' = Cb - 128
                // Cr' = Cr - 128

                // R = 1.1644Y' + 1.5960Cr'
                // G = 1.1644Y' - 0.3917Cb' - 0.8128Cr'
                // B = 1.1644Y' + 2.0172Cb'

                const int r = (298 * y + 409 * v + 128) >> 8;
                const int g = (298 * y - 100 * u - 208 * v + 128) >> 8;
                const int b = (298 * y + 516 * u + 128) >> 8;

                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSet(float(std::min<int>(std::max<int>(r, 0), 255)) / 255.f,
                    float(std::min<int>(std::max<int>(g, 0), 255)) / 255.f,
                    float(std::min<int>(std::max<int>(b, 0), 255)) / 255.f,
                    float(a) / 255.f);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_Y410:
        if (size >= sizeof(XMUDECN4))
        {
            const XMUDECN4 * __restrict sPtr = static_cast<const XMUDECN4*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(XMUDECN4) + 1); icount += sizeof(XMUDECN4))
            {
                const int64_t u = int(sPtr->x) - 512;
                const int64_t y = int(sPtr->y) - 64;
                const int64_t v = int(sPtr->z) - 512;
                const unsigned int a = sPtr->w;
                ++sPtr;

                // http://msdn.microsoft.com/en-us/library/windows/desktop/bb970578.aspx

                // Y'  = Y - 64
                // Cb' = Cb - 512
                // Cr' = Cr - 512

                // R = 1.1678Y' + 1.6007Cr'
                // G = 1.1678Y' - 0.3929Cb' - 0.8152Cr'
                // B = 1.1678Y' + 2.0232Cb'

                const auto r = static_cast<int>((76533 * y + 104905 * v + 32768) >> 16);
                const auto g = static_cast<int>((76533 * y - 25747 * u - 53425 * v + 32768) >> 16);
                const auto b = static_cast<int>((76533 * y + 132590 * u + 32768) >> 16);

                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSet(float(std::min<int>(std::max<int>(r, 0), 1023)) / 1023.f,
                    float(std::min<int>(std::max<int>(g, 0), 1023)) / 1023.f,
                    float(std::min<int>(std::max<int>(b, 0), 1023)) / 1023.f,
                    float(a) / 3.f);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_Y416:
        if (size >= sizeof(XMUSHORTN4))
        {
            const XMUSHORTN4 * __restrict sPtr = static_cast<const XMUSHORTN4*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(XMUSHORTN4) + 1); icount += sizeof(XMUSHORTN4))
            {
                const int64_t u = int64_t(sPtr->x) - 32768;
                const int64_t y = int64_t(sPtr->y) - 4096;
                const int64_t v = int64_t(sPtr->z) - 32768;
                const auto a = static_cast<int>(sPtr->w);
                ++sPtr;

                // http://msdn.microsoft.com/en-us/library/windows/desktop/bb970578.aspx

                // Y'  = Y - 4096
                // Cb' = Cb - 32768
                // Cr' = Cr - 32768

                // R = 1.1689Y' + 1.6023Cr'
                // G = 1.1689Y' - 0.3933Cb' - 0.8160Cr'
                // B = 1.1689Y'+ 2.0251Cb'

                const auto r = static_cast<int>((76607 * y + 105006 * v + 32768) >> 16);
                const auto g = static_cast<int>((76607 * y - 25772 * u - 53477 * v + 32768) >> 16);
                const auto b = static_cast<int>((76607 * y + 132718 * u + 32768) >> 16);

                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSet(float(std::min<int>(std::max<int>(r, 0), 65535)) / 65535.f,
                    float(std::min<int>(std::max<int>(g, 0), 65535)) / 65535.f,
                    float(std::min<int>(std::max<int>(b, 0), 65535)) / 65535.f,
                    float(std::min<int>(std::max<int>(a, 0), 65535)) / 65535.f);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_YUY2:
        if (size >= sizeof(XMUBYTEN4))
        {
            const XMUBYTEN4 * __restrict sPtr = static_cast<const XMUBYTEN4*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(XMUBYTEN4) + 1); icount += sizeof(XMUBYTEN4))
            {
                const int y0 = int(sPtr->x) - 16;
                const int u = int(sPtr->y) - 128;
                const int y1 = int(sPtr->z) - 16;
                const int v = int(sPtr->w) - 128;
                ++sPtr;

                // See AYUV
                int r = (298 * y0 + 409 * v + 128) >> 8;
                int g = (298 * y0 - 100 * u - 208 * v + 128) >> 8;
                int b = (298 * y0 + 516 * u + 128) >> 8;

                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSet(float(std::min<int>(std::max<int>(r, 0), 255)) / 255.f,
                    float(std::min<int>(std::max<int>(g, 0), 255)) / 255.f,
                    float(std::min<int>(std::max<int>(b, 0), 255)) / 255.f,
                    1.f);

                r = (298 * y1 + 409 * v + 128) >> 8;
                g = (298 * y1 - 100 * u - 208 * v + 128) >> 8;
                b = (298 * y1 + 516 * u + 128) >> 8;

                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSet(float(std::min<int>(std::max<int>(r, 0), 255)) / 255.f,
                    float(std::min<int>(std::max<int>(g, 0), 255)) / 255.f,
                    float(std::min<int>(std::max<int>(b, 0), 255)) / 255.f,
                    1.f);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_Y210:
        // Same as Y216 with least significant 6 bits set to zero
        if (size >= sizeof(XMUSHORTN4))
        {
            const XMUSHORTN4 * __restrict sPtr = static_cast<const XMUSHORTN4*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(XMUSHORTN4) + 1); icount += sizeof(XMUSHORTN4))
            {
                const int64_t y0 = int64_t(sPtr->x >> 6) - 64;
                const int64_t u = int64_t(sPtr->y >> 6) - 512;
                const int64_t y1 = int64_t(sPtr->z >> 6) - 64;
                const int64_t v = int64_t(sPtr->w >> 6) - 512;
                ++sPtr;

                // See Y410
                auto r = static_cast<int>((76533 * y0 + 104905 * v + 32768) >> 16);
                auto g = static_cast<int>((76533 * y0 - 25747 * u - 53425 * v + 32768) >> 16);
                auto b = static_cast<int>((76533 * y0 + 132590 * u + 32768) >> 16);

                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSet(float(std::min<int>(std::max<int>(r, 0), 1023)) / 1023.f,
                    float(std::min<int>(std::max<int>(g, 0), 1023)) / 1023.f,
                    float(std::min<int>(std::max<int>(b, 0), 1023)) / 1023.f,
                    1.f);

                r = static_cast<int>((76533 * y1 + 104905 * v + 32768) >> 16);
                g = static_cast<int>((76533 * y1 - 25747 * u - 53425 * v + 32768) >> 16);
                b = static_cast<int>((76533 * y1 + 132590 * u + 32768) >> 16);

                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSet(float(std::min<int>(std::max<int>(r, 0), 1023)) / 1023.f,
                    float(std::min<int>(std::max<int>(g, 0), 1023)) / 1023.f,
                    float(std::min<int>(std::max<int>(b, 0), 1023)) / 1023.f,
                    1.f);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_Y216:
        if (size >= sizeof(XMUSHORTN4))
        {
            const XMUSHORTN4 * __restrict sPtr = static_cast<const XMUSHORTN4*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(XMUSHORTN4) + 1); icount += sizeof(XMUSHORTN4))
            {
                const int64_t y0 = int64_t(sPtr->x) - 4096;
                const int64_t u = int64_t(sPtr->y) - 32768;
                const int64_t y1 = int64_t(sPtr->z) - 4096;
                const int64_t v = int64_t(sPtr->w) - 32768;
                ++sPtr;

                // See Y416
                auto r = static_cast<int>((76607 * y0 + 105006 * v + 32768) >> 16);
                auto g = static_cast<int>((76607 * y0 - 25772 * u - 53477 * v + 32768) >> 16);
                auto b = static_cast<int>((76607 * y0 + 132718 * u + 32768) >> 16);

                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSet(float(std::min<int>(std::max<int>(r, 0), 65535)) / 65535.f,
                    float(std::min<int>(std::max<int>(g, 0), 65535)) / 65535.f,
                    float(std::min<int>(std::max<int>(b, 0), 65535)) / 65535.f,
                    1.f);

                r = static_cast<int>((76607 * y1 + 105006 * v + 32768) >> 16);
                g = static_cast<int>((76607 * y1 - 25772 * u - 53477 * v + 32768) >> 16);
                b = static_cast<int>((76607 * y1 + 132718 * u + 32768) >> 16);

                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSet(float(std::min<int>(std::max<int>(r, 0), 65535)) / 65535.f,
                    float(std::min<int>(std::max<int>(g, 0), 65535)) / 65535.f,
                    float(std::min<int>(std::max<int>(b, 0), 65535)) / 65535.f,
                    1.f);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_B4G4R4A4_UNORM:
        if (size >= sizeof(XMUNIBBLE4))
        {
            static const XMVECTORF32 s_Scale = { { { 1.f / 15.f, 1.f / 15.f, 1.f / 15.f, 1.f / 15.f } } };
            const XMUNIBBLE4 * __restrict sPtr = static_cast<const XMUNIBBLE4*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(XMUNIBBLE4) + 1); icount += sizeof(XMUNIBBLE4))
            {
                XMVECTOR v = XMLoadUNibble4(sPtr++);
                v = XMVectorMultiply(v, s_Scale);
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSwizzle<2, 1, 0, 3>(v);
            }
            return true;
        }
        return false;

    case WIN11_DXGI_FORMAT_A4B4G4R4_UNORM:
        if (size >= sizeof(XMUNIBBLE4))
        {
            static const XMVECTORF32 s_Scale = { { { 1.f / 15.f, 1.f / 15.f, 1.f / 15.f, 1.f / 15.f } } };
            const XMUNIBBLE4 * __restrict sPtr = static_cast<const XMUNIBBLE4*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(XMUNIBBLE4) + 1); icount += sizeof(XMUNIBBLE4))
            {
                XMVECTOR v = XMLoadUNibble4(sPtr++);
                v = XMVectorMultiply(v, s_Scale);
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSwizzle<3, 2, 1, 0>(v);
            }
            return true;
        }
        return false;

    case XBOX_DXGI_FORMAT_R10G10B10_7E3_A2_FLOAT:
        // Xbox One specific 7e3 format
        if (size >= sizeof(XMUDECN4))
        {
            const XMUDECN4 * __restrict sPtr = static_cast<const XMUDECN4*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(XMUDECN4) + 1); icount += sizeof(XMUDECN4))
            {
                if (dPtr >= ePtr) break;

                XMVECTORF32 vResult = { { {
                    FloatFrom7e3(sPtr->x),
                    FloatFrom7e3(sPtr->y),
                    FloatFrom7e3(sPtr->z),
                    static_cast<float>(sPtr->v >> 30) / 3.0f
                } } };

                ++sPtr;

                *(dPtr++) = vResult.v;
            }
            return true;
        }
        return false;

    case XBOX_DXGI_FORMAT_R10G10B10_6E4_A2_FLOAT:
        // Xbox One specific 6e4 format
        if (size >= sizeof(XMUDECN4))
        {
            const XMUDECN4 * __restrict sPtr = static_cast<const XMUDECN4*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(XMUDECN4) + 1); icount += sizeof(XMUDECN4))
            {
                if (dPtr >= ePtr) break;

                XMVECTORF32 vResult = { { {
                    FloatFrom6e4(sPtr->x),
                    FloatFrom6e4(sPtr->y),
                    FloatFrom6e4(sPtr->z),
                    static_cast<float>(sPtr->v >> 30) / 3.0f
                } } };

                ++sPtr;

                *(dPtr++) = vResult.v;
            }
            return true;
        }
        return false;

    case XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM:
        // Xbox One specific format
        LOAD_SCANLINE(XMXDECN4, XMLoadXDecN4)

    case XBOX_DXGI_FORMAT_R4G4_UNORM:
        // Xbox One specific format
        if (size >= sizeof(uint8_t))
        {
            static const XMVECTORF32 s_Scale = { { { 1.f / 15.f, 1.f / 15.f, 0.f, 0.f } } };
            const uint8_t * __restrict sPtr = static_cast<const uint8_t*>(pSource);
            for (size_t icount = 0; icount < (size - sizeof(uint8_t) + 1); icount += sizeof(uint8_t))
            {
                XMUNIBBLE4 nibble;
                nibble.v = static_cast<uint16_t>(*sPtr++);
                XMVECTOR v = XMLoadUNibble4(&nibble);
                v = XMVectorMultiply(v, s_Scale);
                if (dPtr >= ePtr) break;
                *(dPtr++) = XMVectorSelect(g_XMIdentityR3, v, g_XMSelect1100);
            }
            return true;
        }
        return false;

        // We don't support the planar or palettized formats

    default:
        return false;
    }
}

#undef LOAD_SCANLINE
#undef LOAD_SCANLINE3
#undef LOAD_SCANLINE2


//-------------------------------------------------------------------------------------
// Stores an image row from standard RGBA XMVECTOR (aligned) array
//-------------------------------------------------------------------------------------
#define STORE_SCANLINE( type, func )\
        if (size >= sizeof(type))\
        {\
            type * __restrict dPtr = reinterpret_cast<type*>(pDestination);\
            for(size_t icount = 0; icount < (size - sizeof(type) + 1); icount += sizeof(type))\
            {\
                if (sPtr >= ePtr) break;\
                func(dPtr++, *sPtr++);\
            }\
            return true; \
        }\
        return false;

_Use_decl_annotations_
bool DirectX::Internal::StoreScanline(
    void* pDestination,
    size_t size,
    DXGI_FORMAT format,
    const XMVECTOR* pSource,
    size_t count,
    float threshold) noexcept
{
    assert(pDestination != nullptr);
    assert(IsValid(format) && !IsTypeless(format) && !IsCompressed(format) && !IsPlanar(format) && !IsPalettized(format));

    if (!size || !count)
        return false;

    const XMVECTOR* __restrict sPtr = pSource;
    if (!sPtr)
        return false;

    assert((reinterpret_cast<uintptr_t>(pSource) & 0xF) == 0);

    const XMVECTOR* ePtr = sPtr + count;

#ifdef _PREFAST_
    *reinterpret_cast<uint8_t*>(pDestination) = 0;
#endif

    switch (static_cast<int>(format))
    {
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
        STORE_SCANLINE(XMFLOAT4, XMStoreFloat4)

    case DXGI_FORMAT_R32G32B32A32_UINT:
        STORE_SCANLINE(XMUINT4, XMStoreUInt4)

    case DXGI_FORMAT_R32G32B32A32_SINT:
        STORE_SCANLINE(XMINT4, XMStoreSInt4)

    case DXGI_FORMAT_R32G32B32_FLOAT:
        STORE_SCANLINE(XMFLOAT3, XMStoreFloat3)

    case DXGI_FORMAT_R32G32B32_UINT:
        STORE_SCANLINE(XMUINT3, XMStoreUInt3)

    case DXGI_FORMAT_R32G32B32_SINT:
        STORE_SCANLINE(XMINT3, XMStoreSInt3)

    case DXGI_FORMAT_R16G16B16A16_FLOAT:
        if (size >= sizeof(XMHALF4))
        {
            XMHALF4* __restrict dPtr = static_cast<XMHALF4*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(XMHALF4) + 1); icount += sizeof(XMHALF4))
            {
                if (sPtr >= ePtr) break;
                XMVECTOR v = *sPtr++;
                v = XMVectorClamp(v, g_HalfMin, g_HalfMax);
                XMStoreHalf4(dPtr++, v);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R16G16B16A16_UNORM:
        STORE_SCANLINE(XMUSHORTN4, XMStoreUShortN4)

    case DXGI_FORMAT_R16G16B16A16_UINT:
        STORE_SCANLINE(XMUSHORT4, XMStoreUShort4)

    case DXGI_FORMAT_R16G16B16A16_SNORM:
        STORE_SCANLINE(XMSHORTN4, XMStoreShortN4)

    case DXGI_FORMAT_R16G16B16A16_SINT:
        STORE_SCANLINE(XMSHORT4, XMStoreShort4)

    case DXGI_FORMAT_R32G32_FLOAT:
        STORE_SCANLINE(XMFLOAT2, XMStoreFloat2)

    case DXGI_FORMAT_R32G32_UINT:
        STORE_SCANLINE(XMUINT2, XMStoreUInt2)

    case DXGI_FORMAT_R32G32_SINT:
        STORE_SCANLINE(XMINT2, XMStoreSInt2)

    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            {
                constexpr size_t psize = sizeof(float) + sizeof(uint32_t);
                if (size >= psize)
                {
                    auto dPtr = static_cast<float*>(pDestination);
                    for (size_t icount = 0; icount < (size - psize + 1); icount += psize)
                    {
                        if (sPtr >= ePtr) break;
                        XMFLOAT4 f;
                        XMStoreFloat4(&f, *sPtr++);
                        dPtr[0] = f.x;
                        auto ps8 = reinterpret_cast<uint8_t*>(&dPtr[1]);
                        ps8[0] = static_cast<uint8_t>(std::min<float>(255.f, std::max<float>(0.f, f.y)));
                        ps8[1] = ps8[2] = ps8[3] = 0;
                        dPtr += 2;
                    }
                    return true;
                }
            }
            return false;

    case DXGI_FORMAT_R10G10B10A2_UNORM:
        STORE_SCANLINE(XMUDECN4, XMStoreUDecN4)

    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        STORE_SCANLINE(XMUDECN4, XMStoreUDecN4_XR)

    case DXGI_FORMAT_R10G10B10A2_UINT:
        STORE_SCANLINE(XMUDEC4, XMStoreUDec4)

    case DXGI_FORMAT_R11G11B10_FLOAT:
        STORE_SCANLINE(XMFLOAT3PK, XMStoreFloat3PK)

    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        if (size >= sizeof(XMUBYTEN4))
        {
            XMUBYTEN4 * __restrict dPtr = static_cast<XMUBYTEN4*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(XMUBYTEN4) + 1); icount += sizeof(XMUBYTEN4))
            {
                if (sPtr >= ePtr) break;
                const XMVECTOR v = XMVectorAdd(*sPtr++, g_8BitBias);
                XMStoreUByteN4(dPtr++, v);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R8G8B8A8_UINT:
        STORE_SCANLINE(XMUBYTE4, XMStoreUByte4)

    case DXGI_FORMAT_R8G8B8A8_SNORM:
        STORE_SCANLINE(XMBYTEN4, XMStoreByteN4)

    case DXGI_FORMAT_R8G8B8A8_SINT:
        STORE_SCANLINE(XMBYTE4, XMStoreByte4)

    case DXGI_FORMAT_R16G16_FLOAT:
        if (size >= sizeof(XMHALF2))
        {
            XMHALF2* __restrict dPtr = static_cast<XMHALF2*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(XMHALF2) + 1); icount += sizeof(XMHALF2))
            {
                if (sPtr >= ePtr) break;
                XMVECTOR v = *sPtr++;
                v = XMVectorClamp(v, g_HalfMin, g_HalfMax);
                XMStoreHalf2(dPtr++, v);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R16G16_UNORM:
        STORE_SCANLINE(XMUSHORTN2, XMStoreUShortN2)

    case DXGI_FORMAT_R16G16_UINT:
        STORE_SCANLINE(XMUSHORT2, XMStoreUShort2)

    case DXGI_FORMAT_R16G16_SNORM:
        STORE_SCANLINE(XMSHORTN2, XMStoreShortN2)

    case DXGI_FORMAT_R16G16_SINT:
        STORE_SCANLINE(XMSHORT2, XMStoreShort2)

    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
        if (size >= sizeof(float))
        {
            float * __restrict dPtr = static_cast<float*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(float) + 1); icount += sizeof(float))
            {
                if (sPtr >= ePtr) break;
                XMStoreFloat(dPtr++, *(sPtr++));
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R32_UINT:
        if (size >= sizeof(uint32_t))
        {
            uint32_t * __restrict dPtr = static_cast<uint32_t*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(uint32_t) + 1); icount += sizeof(uint32_t))
            {
                if (sPtr >= ePtr) break;
                const XMVECTOR v = XMConvertVectorFloatToUInt(*(sPtr++), 0);
                XMStoreInt(dPtr++, v);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R32_SINT:
        if (size >= sizeof(int32_t))
        {
            uint32_t * __restrict dPtr = static_cast<uint32_t*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(int32_t) + 1); icount += sizeof(int32_t))
            {
                if (sPtr >= ePtr) break;
                const XMVECTOR v = XMConvertVectorFloatToInt(*(sPtr++), 0);
                XMStoreInt(dPtr++, v);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        if (size >= sizeof(uint32_t))
        {
            static const XMVECTORF32 clamp = { { { 1.f, 255.f, 0.f, 0.f } } };
            const XMVECTOR zero = XMVectorZero();
            auto dPtr = static_cast<uint32_t*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(uint32_t) + 1); icount += sizeof(uint32_t))
            {
                if (sPtr >= ePtr) break;
                XMFLOAT4 f;
                XMStoreFloat4(&f, XMVectorClamp(*sPtr++, zero, clamp));
                *dPtr++ = (static_cast<uint32_t>(f.x * 16777215.f) & 0xFFFFFF)
                    | ((static_cast<uint32_t>(f.y) & 0xFF) << 24);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R8G8_UNORM:
        STORE_SCANLINE(XMUBYTEN2, XMStoreUByteN2)

    case DXGI_FORMAT_R8G8_UINT:
        STORE_SCANLINE(XMUBYTE2, XMStoreUByte2)

    case DXGI_FORMAT_R8G8_SNORM:
        STORE_SCANLINE(XMBYTEN2, XMStoreByteN2)

    case DXGI_FORMAT_R8G8_SINT:
        STORE_SCANLINE(XMBYTE2, XMStoreByte2)

    case DXGI_FORMAT_R16_FLOAT:
        if (size >= sizeof(HALF))
        {
            HALF * __restrict dPtr = static_cast<HALF*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(HALF) + 1); icount += sizeof(HALF))
            {
                if (sPtr >= ePtr) break;
                float v = XMVectorGetX(*sPtr++);
                v = std::max<float>(std::min<float>(v, 65504.f), -65504.f);
                *(dPtr++) = XMConvertFloatToHalf(v);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
        if (size >= sizeof(uint16_t))
        {
            uint16_t * __restrict dPtr = static_cast<uint16_t*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(uint16_t) + 1); icount += sizeof(uint16_t))
            {
                if (sPtr >= ePtr) break;
                float v = XMVectorGetX(*sPtr++);
                v = std::max<float>(std::min<float>(v, 1.f), 0.f);
                *(dPtr++) = static_cast<uint16_t>(v*65535.f + 0.5f);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R16_UINT:
        if (size >= sizeof(uint16_t))
        {
            uint16_t * __restrict dPtr = static_cast<uint16_t*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(uint16_t) + 1); icount += sizeof(uint16_t))
            {
                if (sPtr >= ePtr) break;
                float v = XMVectorGetX(*sPtr++);
                v = std::max<float>(std::min<float>(v, 65535.f), 0.f);
                *(dPtr++) = static_cast<uint16_t>(v);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R16_SNORM:
        if (size >= sizeof(int16_t))
        {
            int16_t * __restrict dPtr = static_cast<int16_t*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(int16_t) + 1); icount += sizeof(int16_t))
            {
                if (sPtr >= ePtr) break;
                float v = XMVectorGetX(*sPtr++);
                v = std::max<float>(std::min<float>(v, 1.f), -1.f);
                *(dPtr++) = static_cast<int16_t>(lroundf(v * 32767.f));
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R16_SINT:
        if (size >= sizeof(int16_t))
        {
            int16_t * __restrict dPtr = static_cast<int16_t*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(int16_t) + 1); icount += sizeof(int16_t))
            {
                if (sPtr >= ePtr) break;
                float v = XMVectorGetX(*sPtr++);
                v = std::max<float>(std::min<float>(v, 32767.f), -32767.f);
                *(dPtr++) = static_cast<int16_t>(v);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R8_UNORM:
        if (size >= sizeof(uint8_t))
        {
            uint8_t * __restrict dPtr = static_cast<uint8_t*>(pDestination);
            for (size_t icount = 0; icount < size; icount += sizeof(uint8_t))
            {
                if (sPtr >= ePtr) break;
                float v = XMVectorGetX(*sPtr++);
                v = std::max<float>(std::min<float>(v, 1.f), 0.f);
                *(dPtr++) = static_cast<uint8_t>(v * 255.f);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R8_UINT:
        if (size >= sizeof(uint8_t))
        {
            uint8_t * __restrict dPtr = static_cast<uint8_t*>(pDestination);
            for (size_t icount = 0; icount < size; icount += sizeof(uint8_t))
            {
                if (sPtr >= ePtr) break;
                float v = XMVectorGetX(*sPtr++);
                v = std::max<float>(std::min<float>(v, 255.f), 0.f);
                *(dPtr++) = static_cast<uint8_t>(v);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R8_SNORM:
        if (size >= sizeof(int8_t))
        {
            int8_t * __restrict dPtr = static_cast<int8_t*>(pDestination);
            for (size_t icount = 0; icount < size; icount += sizeof(int8_t))
            {
                if (sPtr >= ePtr) break;
                float v = XMVectorGetX(*sPtr++);
                v = std::max<float>(std::min<float>(v, 1.f), -1.f);
                *(dPtr++) = static_cast<int8_t>(lroundf(v * 127.f));
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R8_SINT:
        if (size >= sizeof(int8_t))
        {
            int8_t * __restrict dPtr = static_cast<int8_t*>(pDestination);
            for (size_t icount = 0; icount < size; icount += sizeof(int8_t))
            {
                if (sPtr >= ePtr) break;
                float v = XMVectorGetX(*sPtr++);
                v = std::max<float>(std::min<float>(v, 127.f), -127.f);
                *(dPtr++) = static_cast<int8_t>(v);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_A8_UNORM:
        if (size >= sizeof(uint8_t))
        {
            uint8_t * __restrict dPtr = static_cast<uint8_t*>(pDestination);
            for (size_t icount = 0; icount < size; icount += sizeof(uint8_t))
            {
                if (sPtr >= ePtr) break;
                float v = XMVectorGetW(*sPtr++);
                v = std::max<float>(std::min<float>(v, 1.f), 0.f);
                *(dPtr++) = static_cast<uint8_t>(v * 255.f);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R1_UNORM:
        if (size >= sizeof(uint8_t))
        {
            uint8_t * __restrict dPtr = static_cast<uint8_t*>(pDestination);
            for (size_t icount = 0; icount < size; icount += sizeof(uint8_t))
            {
                uint8_t pixels = 0;
                for (size_t bcount = 8; bcount > 0; --bcount)
                {
                    if (sPtr >= ePtr) break;
                    const float v = XMVectorGetX(*sPtr++);

                    // Absolute thresholding generally doesn't give good results for all images
                    // Picking the 'right' threshold automatically requires whole-image analysis

                    if (v > 0.25f)
                        pixels |= 1 << (bcount - 1);
                }
                *(dPtr++) = pixels;
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
        STORE_SCANLINE(XMFLOAT3SE, StoreFloat3SE)

    case DXGI_FORMAT_R8G8_B8G8_UNORM:
        if (size >= sizeof(XMUBYTEN4))
        {
            XMUBYTEN4 * __restrict dPtr = static_cast<XMUBYTEN4*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(XMUBYTEN4) + 1); icount += sizeof(XMUBYTEN4))
            {
                if (sPtr >= ePtr) break;
                const XMVECTOR v0 = *sPtr++;
                const XMVECTOR v1 = (sPtr < ePtr) ? XMVectorSplatY(*sPtr++) : XMVectorZero();
                XMVECTOR v = XMVectorSelect(v1, v0, g_XMSelect1110);
                v = XMVectorAdd(v, g_8BitBias);
                XMStoreUByteN4(dPtr++, v);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_G8R8_G8B8_UNORM:
        if (size >= sizeof(XMUBYTEN4))
        {
            static XMVECTORU32 select1101 = { { { XM_SELECT_1, XM_SELECT_1, XM_SELECT_0, XM_SELECT_1 } } };

            XMUBYTEN4 * __restrict dPtr = static_cast<XMUBYTEN4*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(XMUBYTEN4) + 1); icount += sizeof(XMUBYTEN4))
            {
                if (sPtr >= ePtr) break;
                const XMVECTOR v0 = XMVectorSwizzle<1, 0, 3, 2>(*sPtr++);
                const XMVECTOR v1 = (sPtr < ePtr) ? XMVectorSplatY(*sPtr++) : XMVectorZero();
                XMVECTOR v = XMVectorSelect(v1, v0, select1101);
                v = XMVectorAdd(v, g_8BitBias);
                XMStoreUByteN4(dPtr++, v);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_B5G6R5_UNORM:
        if (size >= sizeof(XMU565))
        {
            static const XMVECTORF32 s_Scale = { { { 31.f, 63.f, 31.f, 1.f } } };
            XMU565 * __restrict dPtr = static_cast<XMU565*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(XMU565) + 1); icount += sizeof(XMU565))
            {
                if (sPtr >= ePtr) break;
                XMVECTOR v = XMVectorSwizzle<2, 1, 0, 3>(*sPtr++);
            #if defined(_M_ARM) || defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64) || defined(_M_ARM64EC) || __arm__ || __aarch64__
                v = XMVectorMultiplyAdd(v, s_Scale, g_XMOneHalf);
            #else
                v = XMVectorMultiply(v, s_Scale);
            #endif
                XMStoreU565(dPtr++, v);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_B5G5R5A1_UNORM:
        if (size >= sizeof(XMU555))
        {
            static const XMVECTORF32 s_Scale = { { { 31.f, 31.f, 31.f, 1.f } } };
        #if defined(_M_ARM) || defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64) || defined(_M_ARM64EC) || __arm__ || __aarch64__
            static const XMVECTORF32 s_OneHalfXYZ = { { { 0.5f, 0.5f, 0.5f, 0.f } } };
        #endif
            XMU555 * __restrict dPtr = static_cast<XMU555*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(XMU555) + 1); icount += sizeof(XMU555))
            {
                if (sPtr >= ePtr) break;
                XMVECTOR v = XMVectorSwizzle<2, 1, 0, 3>(*sPtr++);
            #if defined(_M_ARM) || defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64) || defined(_M_ARM64EC) || __arm__ || __aarch64__
                v = XMVectorMultiplyAdd(v, s_Scale, s_OneHalfXYZ);
            #else
                v = XMVectorMultiply(v, s_Scale);
            #endif
                XMStoreU555(dPtr, v);
                dPtr->w = (XMVectorGetW(v) > threshold) ? 1u : 0u;
                ++dPtr;
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        if (size >= sizeof(XMUBYTEN4))
        {
            XMUBYTEN4 * __restrict dPtr = static_cast<XMUBYTEN4*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(XMUBYTEN4) + 1); icount += sizeof(XMUBYTEN4))
            {
                if (sPtr >= ePtr) break;
                XMVECTOR v = XMVectorSwizzle<2, 1, 0, 3>(*sPtr++);
                v = XMVectorAdd(v, g_8BitBias);
                XMStoreUByteN4(dPtr++, v);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        if (size >= sizeof(XMUBYTEN4))
        {
            XMUBYTEN4 * __restrict dPtr = static_cast<XMUBYTEN4*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(XMUBYTEN4) + 1); icount += sizeof(XMUBYTEN4))
            {
                if (sPtr >= ePtr) break;
                XMVECTOR v = XMVectorPermute<2, 1, 0, 7>(*sPtr++, g_XMIdentityR3);
                v = XMVectorAdd(v, g_8BitBias);
                XMStoreUByteN4(dPtr++, v);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_AYUV:
        if (size >= sizeof(XMUBYTEN4))
        {
            XMUBYTEN4 * __restrict dPtr = static_cast<XMUBYTEN4*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(XMUBYTEN4) + 1); icount += sizeof(XMUBYTEN4))
            {
                if (sPtr >= ePtr) break;

                XMUBYTEN4 rgba;
                XMStoreUByteN4(&rgba, *sPtr++);

                // http://msdn.microsoft.com/en-us/library/windows/desktop/dd206750.aspx

                // Y  =  0.2568R + 0.5041G + 0.1001B + 16
                // Cb = -0.1482R - 0.2910G + 0.4392B + 128
                // Cr =  0.4392R - 0.3678G - 0.0714B + 128

                const int y = ((66 * rgba.x + 129 * rgba.y + 25 * rgba.z + 128) >> 8) + 16;
                const int u = ((-38 * rgba.x - 74 * rgba.y + 112 * rgba.z + 128) >> 8) + 128;
                const int v = ((112 * rgba.x - 94 * rgba.y - 18 * rgba.z + 128) >> 8) + 128;

                dPtr->x = static_cast<uint8_t>(std::min<int>(std::max<int>(v, 0), 255));
                dPtr->y = static_cast<uint8_t>(std::min<int>(std::max<int>(u, 0), 255));
                dPtr->z = static_cast<uint8_t>(std::min<int>(std::max<int>(y, 0), 255));
                dPtr->w = rgba.w;
                ++dPtr;
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_Y410:
        if (size >= sizeof(XMUDECN4))
        {
            XMUDECN4 * __restrict dPtr = static_cast<XMUDECN4*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(XMUDECN4) + 1); icount += sizeof(XMUDECN4))
            {
                if (sPtr >= ePtr) break;

                XMUDECN4 rgba;
                XMStoreUDecN4(&rgba, *sPtr++);

                // http://msdn.microsoft.com/en-us/library/windows/desktop/bb970578.aspx

                // Y  =  0.2560R + 0.5027G + 0.0998B + 64
                // Cb = -0.1478R - 0.2902G + 0.4379B + 512
                // Cr =  0.4379R - 0.3667G - 0.0712B + 512

                const int64_t r = rgba.x;
                const int64_t g = rgba.y;
                const int64_t b = rgba.z;

                const int y = static_cast<int>((16780 * r + 32942 * g + 6544 * b + 32768) >> 16) + 64;
                const int u = static_cast<int>((-9683 * r - 19017 * g + 28700 * b + 32768) >> 16) + 512;
                const int v = static_cast<int>((28700 * r - 24033 * g - 4667 * b + 32768) >> 16) + 512;

                dPtr->x = static_cast<uint32_t>(std::min<int>(std::max<int>(u, 0), 1023));
                dPtr->y = static_cast<uint32_t>(std::min<int>(std::max<int>(y, 0), 1023));
                dPtr->z = static_cast<uint32_t>(std::min<int>(std::max<int>(v, 0), 1023));
                dPtr->w = rgba.w;
                ++dPtr;
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_Y416:
        if (size >= sizeof(XMUSHORTN4))
        {
            XMUSHORTN4 * __restrict dPtr = static_cast<XMUSHORTN4*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(XMUSHORTN4) + 1); icount += sizeof(XMUSHORTN4))
            {
                if (sPtr >= ePtr) break;

                XMUSHORTN4 rgba;
                XMStoreUShortN4(&rgba, *sPtr++);

                // http://msdn.microsoft.com/en-us/library/windows/desktop/bb970578.aspx

                // Y  =  0.2558R + 0.5022G + 0.0998B + 4096
                // Cb = -0.1476R - 0.2899G + 0.4375B + 32768
                // Cr =  0.4375R - 0.3664G - 0.0711B + 32768

                const int64_t r = int64_t(rgba.x);
                const int64_t g = int64_t(rgba.y);
                const int64_t b = int64_t(rgba.z);

                const int y = static_cast<int>((16763 * r + 32910 * g + 6537 * b + 32768) >> 16) + 4096;
                const int u = static_cast<int>((-9674 * r - 18998 * g + 28672 * b + 32768) >> 16) + 32768;
                const int v = static_cast<int>((28672 * r - 24010 * g - 4662 * b + 32768) >> 16) + 32768;

                dPtr->x = static_cast<uint16_t>(std::min<int>(std::max<int>(u, 0), 65535));
                dPtr->y = static_cast<uint16_t>(std::min<int>(std::max<int>(y, 0), 65535));
                dPtr->z = static_cast<uint16_t>(std::min<int>(std::max<int>(v, 0), 65535));
                dPtr->w = rgba.w;
                ++dPtr;
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_YUY2:
        if (size >= sizeof(XMUBYTEN4))
        {
            XMUBYTEN4 * __restrict dPtr = static_cast<XMUBYTEN4*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(XMUBYTEN4) + 1); icount += sizeof(XMUBYTEN4))
            {
                if (sPtr >= ePtr) break;

                XMUBYTEN4 rgb1;
                XMStoreUByteN4(&rgb1, *sPtr++);

                // See AYUV
                const int y0 = ((66 * rgb1.x + 129 * rgb1.y + 25 * rgb1.z + 128) >> 8) + 16;
                const int u0 = ((-38 * rgb1.x - 74 * rgb1.y + 112 * rgb1.z + 128) >> 8) + 128;
                const int v0 = ((112 * rgb1.x - 94 * rgb1.y - 18 * rgb1.z + 128) >> 8) + 128;

                XMUBYTEN4 rgb2 = {};
                if (sPtr < ePtr)
                {
                    XMStoreUByteN4(&rgb2, *sPtr++);
                }

                const int y1 = ((66 * rgb2.x + 129 * rgb2.y + 25 * rgb2.z + 128) >> 8) + 16;
                const int u1 = ((-38 * rgb2.x - 74 * rgb2.y + 112 * rgb2.z + 128) >> 8) + 128;
                const int v1 = ((112 * rgb2.x - 94 * rgb2.y - 18 * rgb2.z + 128) >> 8) + 128;

                dPtr->x = static_cast<uint8_t>(std::min<int>(std::max<int>(y0, 0), 255));
                dPtr->y = static_cast<uint8_t>(std::min<int>(std::max<int>((u0 + u1) >> 1, 0), 255));
                dPtr->z = static_cast<uint8_t>(std::min<int>(std::max<int>(y1, 0), 255));
                dPtr->w = static_cast<uint8_t>(std::min<int>(std::max<int>((v0 + v1) >> 1, 0), 255));
                ++dPtr;
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_Y210:
        // Same as Y216 with least significant 6 bits set to zero
        if (size >= sizeof(XMUSHORTN4))
        {
            XMUSHORTN4 * __restrict dPtr = static_cast<XMUSHORTN4*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(XMUSHORTN4) + 1); icount += sizeof(XMUSHORTN4))
            {
                if (sPtr >= ePtr) break;

                XMUDECN4 rgb1;
                XMStoreUDecN4(&rgb1, *sPtr++);

                // See Y410
                int64_t r = rgb1.x;
                int64_t g = rgb1.y;
                int64_t b = rgb1.z;

                const int y0 = static_cast<int>((16780 * r + 32942 * g + 6544 * b + 32768) >> 16) + 64;
                const int u0 = static_cast<int>((-9683 * r - 19017 * g + 28700 * b + 32768) >> 16) + 512;
                const int v0 = static_cast<int>((28700 * r - 24033 * g - 4667 * b + 32768) >> 16) + 512;

                XMUDECN4 rgb2 = {};
                if (sPtr < ePtr)
                {
                    XMStoreUDecN4(&rgb2, *sPtr++);
                }

                r = rgb2.x;
                g = rgb2.y;
                b = rgb2.z;

                const int y1 = static_cast<int>((16780 * r + 32942 * g + 6544 * b + 32768) >> 16) + 64;
                const int u1 = static_cast<int>((-9683 * r - 19017 * g + 28700 * b + 32768) >> 16) + 512;
                const int v1 = static_cast<int>((28700 * r - 24033 * g - 4667 * b + 32768) >> 16) + 512;

                dPtr->x = static_cast<uint16_t>(std::min<int>(std::max<int>(y0, 0), 1023) << 6);
                dPtr->y = static_cast<uint16_t>(std::min<int>(std::max<int>((u0 + u1) >> 1, 0), 1023) << 6);
                dPtr->z = static_cast<uint16_t>(std::min<int>(std::max<int>(y1, 0), 1023) << 6);
                dPtr->w = static_cast<uint16_t>(std::min<int>(std::max<int>((v0 + v1) >> 1, 0), 1023) << 6);
                ++dPtr;
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_Y216:
        if (size >= sizeof(XMUSHORTN4))
        {
            XMUSHORTN4 * __restrict dPtr = static_cast<XMUSHORTN4*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(XMUSHORTN4) + 1); icount += sizeof(XMUSHORTN4))
            {
                if (sPtr >= ePtr) break;

                XMUSHORTN4 rgb1;
                XMStoreUShortN4(&rgb1, *sPtr++);

                // See Y416
                int64_t r = int64_t(rgb1.x);
                int64_t g = int64_t(rgb1.y);
                int64_t b = int64_t(rgb1.z);

                const int y0 = static_cast<int>((16763 * r + 32910 * g + 6537 * b + 32768) >> 16) + 4096;
                const int u0 = static_cast<int>((-9674 * r - 18998 * g + 28672 * b + 32768) >> 16) + 32768;
                const int v0 = static_cast<int>((28672 * r - 24010 * g - 4662 * b + 32768) >> 16) + 32768;

                XMUSHORTN4 rgb2 = {};
                if (sPtr < ePtr)
                {
                    XMStoreUShortN4(&rgb2, *sPtr++);
                }

                r = int64_t(rgb2.x);
                g = int64_t(rgb2.y);
                b = int64_t(rgb2.z);

                const int y1 = static_cast<int>((16763 * r + 32910 * g + 6537 * b + 32768) >> 16) + 4096;
                const int u1 = static_cast<int>((-9674 * r - 18998 * g + 28672 * b + 32768) >> 16) + 32768;
                const int v1 = static_cast<int>((28672 * r - 24010 * g - 4662 * b + 32768) >> 16) + 32768;

                dPtr->x = static_cast<uint16_t>(std::min<int>(std::max<int>(y0, 0), 65535));
                dPtr->y = static_cast<uint16_t>(std::min<int>(std::max<int>((u0 + u1) >> 1, 0), 65535));
                dPtr->z = static_cast<uint16_t>(std::min<int>(std::max<int>(y1, 0), 65535));
                dPtr->w = static_cast<uint16_t>(std::min<int>(std::max<int>((v0 + v1) >> 1, 0), 65535));
                ++dPtr;
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_B4G4R4A4_UNORM:
        if (size >= sizeof(XMUNIBBLE4))
        {
            static const XMVECTORF32 s_Scale = { { { 15.f, 15.f, 15.f, 15.f } } };
            XMUNIBBLE4 * __restrict dPtr = static_cast<XMUNIBBLE4*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(XMUNIBBLE4) + 1); icount += sizeof(XMUNIBBLE4))
            {
                if (sPtr >= ePtr) break;
                XMVECTOR v = XMVectorSwizzle<2, 1, 0, 3>(*sPtr++);
            #if defined(_M_ARM) || defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64) || defined(_M_ARM64EC) || __arm__ || __aarch64__
                v = XMVectorMultiplyAdd(v, s_Scale, g_XMOneHalf);
            #else
                v = XMVectorMultiply(v, s_Scale);
            #endif
                XMStoreUNibble4(dPtr++, v);
            }
            return true;
        }
        return false;

    case WIN11_DXGI_FORMAT_A4B4G4R4_UNORM:
        if (size >= sizeof(XMUNIBBLE4))
        {
            static const XMVECTORF32 s_Scale = { { { 15.f, 15.f, 15.f, 15.f } } };
            XMUNIBBLE4 * __restrict dPtr = static_cast<XMUNIBBLE4*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(XMUNIBBLE4) + 1); icount += sizeof(XMUNIBBLE4))
            {
                if (sPtr >= ePtr) break;
                XMVECTOR v = XMVectorSwizzle<3, 2, 1, 0>(*sPtr++);
            #if defined(_M_ARM) || defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64) || defined(_M_ARM64EC) || __arm__ || __aarch64__
                v = XMVectorMultiplyAdd(v, s_Scale, g_XMOneHalf);
            #else
                v = XMVectorMultiply(v, s_Scale);
            #endif
                XMStoreUNibble4(dPtr++, v);
            }
            return true;
        }
        return false;

    case XBOX_DXGI_FORMAT_R10G10B10_7E3_A2_FLOAT:
        // Xbox One specific 7e3 format with alpha
        if (size >= sizeof(XMUDECN4))
        {
            static const XMVECTORF32  Scale = { { { 1.0f, 1.0f, 1.0f, 3.0f } } };
            static const XMVECTORF32  C = { { { 31.875f, 31.875f, 31.875f, 3.f } } };

            XMUDECN4 * __restrict dPtr = static_cast<XMUDECN4*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(XMUDECN4) + 1); icount += sizeof(XMUDECN4))
            {
                if (sPtr >= ePtr) break;

                XMVECTOR V = XMVectorMultiply(*sPtr++, Scale);
                V = XMVectorClamp(V, g_XMZero, C);

                XMFLOAT4A tmp;
                XMStoreFloat4A(&tmp, V);

                dPtr->x = FloatTo7e3(tmp.x);
                dPtr->y = FloatTo7e3(tmp.y);
                dPtr->z = FloatTo7e3(tmp.z);
                dPtr->w = static_cast<uint32_t>(tmp.w);
                ++dPtr;
            }
            return true;
        }
        return false;

    case XBOX_DXGI_FORMAT_R10G10B10_6E4_A2_FLOAT:
        // Xbox One specific 6e4 format with alpha
        if (size >= sizeof(XMUDECN4))
        {
            static const XMVECTORF32  Scale = { { { 1.0f, 1.0f, 1.0f, 3.0f } } };
            static const XMVECTORF32  C = { { { 508.f, 508.f, 508.f, 3.f } } };

            XMUDECN4 * __restrict dPtr = static_cast<XMUDECN4*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(XMUDECN4) + 1); icount += sizeof(XMUDECN4))
            {
                if (sPtr >= ePtr) break;

                XMVECTOR V = XMVectorMultiply(*sPtr++, Scale);
                V = XMVectorClamp(V, g_XMZero, C);

                XMFLOAT4A tmp;
                XMStoreFloat4A(&tmp, V);

                dPtr->x = FloatTo6e4(tmp.x);
                dPtr->y = FloatTo6e4(tmp.y);
                dPtr->z = FloatTo6e4(tmp.z);
                dPtr->w = static_cast<uint32_t>(tmp.w);
                ++dPtr;
            }
            return true;
        }
        return false;

    case XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM:
        // Xbox One specific format
        STORE_SCANLINE(XMXDECN4, XMStoreXDecN4)

    case XBOX_DXGI_FORMAT_R4G4_UNORM:
        // Xbox One specific format
        if (size >= sizeof(uint8_t))
        {
            static const XMVECTORF32 s_Scale = { { { 15.f, 15.f, 0.f, 0.f } } };
            uint8_t * __restrict dPtr = static_cast<uint8_t*>(pDestination);
            for (size_t icount = 0; icount < (size - sizeof(uint8_t) + 1); icount += sizeof(uint8_t))
            {
                if (sPtr >= ePtr) break;
            #if defined(_M_ARM) || defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64) || defined(_M_ARM64EC) || __arm__ || __aarch64__
                const XMVECTOR v = XMVectorMultiplyAdd(*sPtr++, s_Scale, g_XMOneHalf);
            #else
                const XMVECTOR v = XMVectorMultiply(*sPtr++, s_Scale);
            #endif
                XMUNIBBLE4 nibble;
                XMStoreUNibble4(&nibble, v);
                *dPtr = static_cast<uint8_t>(nibble.v);
                ++dPtr;
            }
            return true;
        }
        return false;

        // We don't support the planar or palettized formats

    default:
        return false;
    }
}

#undef STORE_SCANLINE


//-------------------------------------------------------------------------------------
// Convert DXGI image to/from GUID_WICPixelFormat128bppRGBAFloat (no range conversions)
//-------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT DirectX::Internal::ConvertToR32G32B32A32(const Image& srcImage, ScratchImage& image) noexcept
{
    if (!srcImage.pixels)
        return E_POINTER;

    HRESULT hr = image.Initialize2D(DXGI_FORMAT_R32G32B32A32_FLOAT, srcImage.width, srcImage.height, 1, 1);
    if (FAILED(hr))
        return hr;

    const Image *img = image.GetImage(0, 0, 0);
    if (!img)
    {
        image.Release();
        return E_POINTER;
    }

    uint8_t* pDest = img->pixels;
    if (!pDest)
    {
        image.Release();
        return E_POINTER;
    }

    const uint8_t *pSrc = srcImage.pixels;
    for (size_t h = 0; h < srcImage.height; ++h)
    {
        if (!LoadScanline(reinterpret_cast<XMVECTOR*>(pDest), srcImage.width, pSrc, srcImage.rowPitch, srcImage.format))
        {
            image.Release();
            return E_FAIL;
        }

        pSrc += srcImage.rowPitch;
        pDest += img->rowPitch;
    }

    return S_OK;
}

_Use_decl_annotations_
HRESULT DirectX::Internal::ConvertFromR32G32B32A32(const Image& srcImage, const Image& destImage) noexcept
{
    assert(srcImage.format == DXGI_FORMAT_R32G32B32A32_FLOAT);

    if (!srcImage.pixels || !destImage.pixels)
        return E_POINTER;

    if (srcImage.width != destImage.width || srcImage.height != destImage.height)
        return E_FAIL;

    const uint8_t *pSrc = srcImage.pixels;
    uint8_t* pDest = destImage.pixels;

    for (size_t h = 0; h < srcImage.height; ++h)
    {
        if (!StoreScanline(pDest, destImage.rowPitch, destImage.format, reinterpret_cast<const XMVECTOR*>(pSrc), srcImage.width))
            return E_FAIL;

        pSrc += srcImage.rowPitch;
        pDest += destImage.rowPitch;
    }

    return S_OK;
}

_Use_decl_annotations_
HRESULT DirectX::Internal::ConvertFromR32G32B32A32(
    const Image& srcImage,
    DXGI_FORMAT format,
    ScratchImage& image) noexcept
{
    if (!srcImage.pixels)
        return E_POINTER;

    HRESULT hr = image.Initialize2D(format, srcImage.width, srcImage.height, 1, 1);
    if (FAILED(hr))
        return hr;

    const Image *img = image.GetImage(0, 0, 0);
    if (!img)
    {
        image.Release();
        return E_POINTER;
    }

    hr = ConvertFromR32G32B32A32(srcImage, *img);
    if (FAILED(hr))
    {
        image.Release();
        return hr;
    }

    return S_OK;
}

_Use_decl_annotations_
HRESULT DirectX::Internal::ConvertFromR32G32B32A32(
    const Image* srcImages,
    size_t nimages,
    const TexMetadata& metadata,
    DXGI_FORMAT format,
    ScratchImage& result) noexcept
{
    if (!srcImages)
        return E_POINTER;

    result.Release();

    assert(metadata.format == DXGI_FORMAT_R32G32B32A32_FLOAT);

    TexMetadata mdata2 = metadata;
    mdata2.format = format;
    HRESULT hr = result.Initialize(mdata2);
    if (FAILED(hr))
        return hr;

    if (nimages != result.GetImageCount())
    {
        result.Release();
        return E_FAIL;
    }

    const Image* dest = result.GetImages();
    if (!dest)
    {
        result.Release();
        return E_POINTER;
    }

    for (size_t index = 0; index < nimages; ++index)
    {
        const Image& src = srcImages[index];
        const Image& dst = dest[index];

        assert(src.format == DXGI_FORMAT_R32G32B32A32_FLOAT);
        assert(dst.format == format);

        if (src.width != dst.width || src.height != dst.height)
        {
            result.Release();
            return E_FAIL;
        }

        const uint8_t* pSrc = src.pixels;
        uint8_t* pDest = dst.pixels;
        if (!pSrc || !pDest)
        {
            result.Release();
            return E_POINTER;
        }

        for (size_t h = 0; h < src.height; ++h)
        {
            if (!StoreScanline(pDest, dst.rowPitch, format, reinterpret_cast<const XMVECTOR*>(pSrc), src.width))
            {
                result.Release();
                return E_FAIL;
            }

            pSrc += src.rowPitch;
            pDest += dst.rowPitch;
        }
    }

    return S_OK;
}


//-------------------------------------------------------------------------------------
// Convert DXGI image to/from GUID_WICPixelFormat64bppRGBAHalf (no range conversions)
//-------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT DirectX::Internal::ConvertToR16G16B16A16(const Image& srcImage, ScratchImage& image) noexcept
{
    if (!srcImage.pixels)
        return E_POINTER;

    HRESULT hr = image.Initialize2D(DXGI_FORMAT_R16G16B16A16_FLOAT, srcImage.width, srcImage.height, 1, 1);
    if (FAILED(hr))
        return hr;

    auto scanline = make_AlignedArrayXMVECTOR(srcImage.width);
    if (!scanline)
    {
        image.Release();
        return E_OUTOFMEMORY;
    }

    const Image *img = image.GetImage(0, 0, 0);
    if (!img)
    {
        image.Release();
        return E_POINTER;
    }

    uint8_t* pDest = img->pixels;
    if (!pDest)
    {
        image.Release();
        return E_POINTER;
    }

    const uint8_t *pSrc = srcImage.pixels;
    for (size_t h = 0; h < srcImage.height; ++h)
    {
        if (!LoadScanline(scanline.get(), srcImage.width, pSrc, srcImage.rowPitch, srcImage.format))
        {
            image.Release();
            return E_FAIL;
        }

        XMConvertFloatToHalfStream(
            reinterpret_cast<HALF*>(pDest), sizeof(HALF),
            reinterpret_cast<float*>(scanline.get()), sizeof(float),
            srcImage.width * 4);

        pSrc += srcImage.rowPitch;
        pDest += img->rowPitch;
    }

    return S_OK;
}

_Use_decl_annotations_
HRESULT DirectX::Internal::ConvertFromR16G16B16A16(const Image& srcImage, const Image& destImage) noexcept
{
    assert(srcImage.format == DXGI_FORMAT_R16G16B16A16_FLOAT);

    if (!srcImage.pixels || !destImage.pixels)
        return E_POINTER;

    if (srcImage.width != destImage.width || srcImage.height != destImage.height)
        return E_FAIL;

    auto scanline = make_AlignedArrayXMVECTOR(srcImage.width);
    if (!scanline)
        return E_OUTOFMEMORY;

    const uint8_t *pSrc = srcImage.pixels;
    uint8_t* pDest = destImage.pixels;

    for (size_t h = 0; h < srcImage.height; ++h)
    {
        XMConvertHalfToFloatStream(
            reinterpret_cast<float*>(scanline.get()), sizeof(float),
            reinterpret_cast<const HALF*>(pSrc), sizeof(HALF),
            srcImage.width * 4);

        if (!StoreScanline(pDest, destImage.rowPitch, destImage.format, scanline.get(), srcImage.width))
            return E_FAIL;

        pSrc += srcImage.rowPitch;
        pDest += destImage.rowPitch;
    }

    return S_OK;
}


//-------------------------------------------------------------------------------------
// Convert from Linear RGB to sRGB
//
// if C_linear <= 0.0031308 -> C_srgb = 12.92 * C_linear
// if C_linear >  0.0031308 -> C_srgb = ( 1 + a ) * pow( C_Linear, 1 / 2.4 ) - a
//                             where a = 0.055
//-------------------------------------------------------------------------------------
_Use_decl_annotations_
bool DirectX::Internal::StoreScanlineLinear(
    void* pDestination,
    size_t size,
    DXGI_FORMAT format,
    XMVECTOR* pSource,
    size_t count,
    TEX_FILTER_FLAGS flags,
    float threshold) noexcept
{
    if (!pSource || !count)
        return false;

    assert((reinterpret_cast<uintptr_t>(pSource) & 0xF) == 0);

    switch (static_cast<int>(format))
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        flags |= TEX_FILTER_SRGB;
        break;

    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R11G11B10_FLOAT:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DXGI_FORMAT_B5G6R5_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_B4G4R4A4_UNORM:
    case WIN11_DXGI_FORMAT_A4B4G4R4_UNORM:
        break;

    default:
        // can't treat A8, XR, Depth, SNORM, UINT, or SINT as sRGB
        flags &= ~TEX_FILTER_SRGB;
        break;
    }

    // sRGB output processing (Linear RGB -> sRGB)
    if (flags & TEX_FILTER_SRGB_OUT)
    {
        // To avoid the need for another temporary scanline buffer, we allow this function to overwrite the source buffer in-place
        // Given the intended usage in the filtering routines, this is not a problem.
        XMVECTOR* ptr = pSource;
        for (size_t i = 0; i < count; ++i, ++ptr)
        {
            *ptr = XMColorRGBToSRGB(*ptr);
        }
    }

    return StoreScanline(pDestination, size, format, pSource, count, threshold);
}


//-------------------------------------------------------------------------------------
// Convert from sRGB to Linear RGB
//
// if C_srgb <= 0.04045 -> C_linear = C_srgb / 12.92
// if C_srgb >  0.04045 -> C_linear = pow( ( C_srgb + a ) / ( 1 + a ), 2.4 )
//                         where a = 0.055
//-------------------------------------------------------------------------------------
_Use_decl_annotations_
bool DirectX::Internal::LoadScanlineLinear(
    XMVECTOR* pDestination,
    size_t count,
    const void* pSource,
    size_t size,
    DXGI_FORMAT format,
    TEX_FILTER_FLAGS flags) noexcept
{
    switch (static_cast<int>(format))
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        flags |= TEX_FILTER_SRGB;
        break;

    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R11G11B10_FLOAT:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DXGI_FORMAT_B5G6R5_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_B4G4R4A4_UNORM:
    case WIN11_DXGI_FORMAT_A4B4G4R4_UNORM:
        break;

    default:
        // can't treat A8, XR, Depth, SNORM, UINT, or SINT as sRGB
        flags &= ~TEX_FILTER_SRGB;
        break;
    }

    if (LoadScanline(pDestination, count, pSource, size, format))
    {
        // sRGB input processing (sRGB -> Linear RGB)
        if (flags & TEX_FILTER_SRGB_IN)
        {
            XMVECTOR* ptr = pDestination;
            for (size_t i = 0; i < count; ++i, ++ptr)
            {
                *ptr = XMColorSRGBToRGB(*ptr);
            }
        }

        return true;
    }

    return false;
}


//-------------------------------------------------------------------------------------
// Convert scanline based on source/target formats
//-------------------------------------------------------------------------------------
namespace
{
    struct ConvertData
    {
        DXGI_FORMAT format;
        size_t      datasize;
        uint32_t    flags;
    };

    const ConvertData g_ConvertTable[] =
    {
        { DXGI_FORMAT_R32G32B32A32_FLOAT,           32, CONVF_FLOAT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_R32G32B32A32_UINT,            32, CONVF_UINT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_R32G32B32A32_SINT,            32, CONVF_SINT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_R32G32B32_FLOAT,              32, CONVF_FLOAT | CONVF_R | CONVF_G | CONVF_B },
        { DXGI_FORMAT_R32G32B32_UINT,               32, CONVF_UINT | CONVF_R | CONVF_G | CONVF_B },
        { DXGI_FORMAT_R32G32B32_SINT,               32, CONVF_SINT | CONVF_R | CONVF_G | CONVF_B },
        { DXGI_FORMAT_R16G16B16A16_FLOAT,           16, CONVF_FLOAT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_R16G16B16A16_UNORM,           16, CONVF_UNORM | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_R16G16B16A16_UINT,            16, CONVF_UINT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_R16G16B16A16_SNORM,           16, CONVF_SNORM | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_R16G16B16A16_SINT,            16, CONVF_SINT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_R32G32_FLOAT,                 32, CONVF_FLOAT | CONVF_R | CONVF_G },
        { DXGI_FORMAT_R32G32_UINT,                  32, CONVF_UINT | CONVF_R | CONVF_G  },
        { DXGI_FORMAT_R32G32_SINT,                  32, CONVF_SINT | CONVF_R | CONVF_G  },
        { DXGI_FORMAT_D32_FLOAT_S8X24_UINT,         32, CONVF_FLOAT | CONVF_DEPTH | CONVF_STENCIL },
        { DXGI_FORMAT_R10G10B10A2_UNORM,            10, CONVF_UNORM | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_R10G10B10A2_UINT,             10, CONVF_UINT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_R11G11B10_FLOAT,              10, CONVF_FLOAT | CONVF_POS_ONLY | CONVF_R | CONVF_G | CONVF_B },
        { DXGI_FORMAT_R8G8B8A8_UNORM,                8, CONVF_UNORM | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,           8, CONVF_UNORM | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_R8G8B8A8_UINT,                 8, CONVF_UINT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_R8G8B8A8_SNORM,                8, CONVF_SNORM | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_R8G8B8A8_SINT,                 8, CONVF_SINT | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_R16G16_FLOAT,                 16, CONVF_FLOAT | CONVF_R | CONVF_G },
        { DXGI_FORMAT_R16G16_UNORM,                 16, CONVF_UNORM | CONVF_R | CONVF_G },
        { DXGI_FORMAT_R16G16_UINT,                  16, CONVF_UINT | CONVF_R | CONVF_G },
        { DXGI_FORMAT_R16G16_SNORM,                 16, CONVF_SNORM | CONVF_R | CONVF_G },
        { DXGI_FORMAT_R16G16_SINT,                  16, CONVF_SINT | CONVF_R | CONVF_G },
        { DXGI_FORMAT_D32_FLOAT,                    32, CONVF_FLOAT | CONVF_DEPTH },
        { DXGI_FORMAT_R32_FLOAT,                    32, CONVF_FLOAT | CONVF_R },
        { DXGI_FORMAT_R32_UINT,                     32, CONVF_UINT | CONVF_R },
        { DXGI_FORMAT_R32_SINT,                     32, CONVF_SINT | CONVF_R },
        { DXGI_FORMAT_D24_UNORM_S8_UINT,            32, CONVF_UNORM | CONVF_DEPTH | CONVF_STENCIL },
        { DXGI_FORMAT_R8G8_UNORM,                    8, CONVF_UNORM | CONVF_R | CONVF_G },
        { DXGI_FORMAT_R8G8_UINT,                     8, CONVF_UINT | CONVF_R | CONVF_G },
        { DXGI_FORMAT_R8G8_SNORM,                    8, CONVF_SNORM | CONVF_R | CONVF_G },
        { DXGI_FORMAT_R8G8_SINT,                     8, CONVF_SINT | CONVF_R | CONVF_G },
        { DXGI_FORMAT_R16_FLOAT,                    16, CONVF_FLOAT | CONVF_R },
        { DXGI_FORMAT_D16_UNORM,                    16, CONVF_UNORM | CONVF_DEPTH },
        { DXGI_FORMAT_R16_UNORM,                    16, CONVF_UNORM | CONVF_R },
        { DXGI_FORMAT_R16_UINT,                     16, CONVF_UINT | CONVF_R },
        { DXGI_FORMAT_R16_SNORM,                    16, CONVF_SNORM | CONVF_R },
        { DXGI_FORMAT_R16_SINT,                     16, CONVF_SINT | CONVF_R },
        { DXGI_FORMAT_R8_UNORM,                      8, CONVF_UNORM | CONVF_R },
        { DXGI_FORMAT_R8_UINT,                       8, CONVF_UINT | CONVF_R },
        { DXGI_FORMAT_R8_SNORM,                      8, CONVF_SNORM | CONVF_R },
        { DXGI_FORMAT_R8_SINT,                       8, CONVF_SINT | CONVF_R },
        { DXGI_FORMAT_A8_UNORM,                      8, CONVF_UNORM | CONVF_A },
        { DXGI_FORMAT_R1_UNORM,                      1, CONVF_UNORM | CONVF_R },
        { DXGI_FORMAT_R9G9B9E5_SHAREDEXP,            9, CONVF_FLOAT | CONVF_SHAREDEXP | CONVF_POS_ONLY | CONVF_R | CONVF_G | CONVF_B },
        { DXGI_FORMAT_R8G8_B8G8_UNORM,               8, CONVF_UNORM | CONVF_PACKED | CONVF_R | CONVF_G | CONVF_B },
        { DXGI_FORMAT_G8R8_G8B8_UNORM,               8, CONVF_UNORM | CONVF_PACKED | CONVF_R | CONVF_G | CONVF_B },
        { DXGI_FORMAT_BC1_UNORM,                     8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_BC1_UNORM_SRGB,                8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_BC2_UNORM,                     8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_BC2_UNORM_SRGB,                8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_BC3_UNORM,                     8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_BC3_UNORM_SRGB,                8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_BC4_UNORM,                     8, CONVF_UNORM | CONVF_BC | CONVF_R },
        { DXGI_FORMAT_BC4_SNORM,                     8, CONVF_SNORM | CONVF_BC | CONVF_R },
        { DXGI_FORMAT_BC5_UNORM,                     8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G },
        { DXGI_FORMAT_BC5_SNORM,                     8, CONVF_SNORM | CONVF_BC | CONVF_R | CONVF_G },
        { DXGI_FORMAT_B5G6R5_UNORM,                  5, CONVF_UNORM | CONVF_R | CONVF_G | CONVF_B },
        { DXGI_FORMAT_B5G5R5A1_UNORM,                5, CONVF_UNORM | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_B8G8R8A8_UNORM,                8, CONVF_UNORM | CONVF_BGR | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_B8G8R8X8_UNORM,                8, CONVF_UNORM | CONVF_BGR | CONVF_R | CONVF_G | CONVF_B },
        { DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,   10, CONVF_UNORM | CONVF_XR | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,           8, CONVF_UNORM | CONVF_BGR | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,           8, CONVF_UNORM | CONVF_BGR | CONVF_R | CONVF_G | CONVF_B },
        { DXGI_FORMAT_BC6H_UF16,                    16, CONVF_FLOAT | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_BC6H_SF16,                    16, CONVF_FLOAT | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_BC7_UNORM,                     8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_BC7_UNORM_SRGB,                8, CONVF_UNORM | CONVF_BC | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_AYUV,                          8, CONVF_UNORM | CONVF_YUV | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_Y410,                         10, CONVF_UNORM | CONVF_YUV | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_Y416,                         16, CONVF_UNORM | CONVF_YUV | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { DXGI_FORMAT_YUY2,                          8, CONVF_UNORM | CONVF_YUV | CONVF_PACKED | CONVF_R | CONVF_G | CONVF_B },
        { DXGI_FORMAT_Y210,                         10, CONVF_UNORM | CONVF_YUV | CONVF_PACKED | CONVF_R | CONVF_G | CONVF_B },
        { DXGI_FORMAT_Y216,                         16, CONVF_UNORM | CONVF_YUV | CONVF_PACKED | CONVF_R | CONVF_G | CONVF_B },
        { DXGI_FORMAT_B4G4R4A4_UNORM,                4, CONVF_UNORM | CONVF_BGR | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { XBOX_DXGI_FORMAT_R10G10B10_7E3_A2_FLOAT,  10, CONVF_FLOAT | CONVF_POS_ONLY | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { XBOX_DXGI_FORMAT_R10G10B10_6E4_A2_FLOAT,  10, CONVF_FLOAT | CONVF_POS_ONLY | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM,10, CONVF_SNORM | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
        { XBOX_DXGI_FORMAT_R4G4_UNORM,               4, CONVF_UNORM | CONVF_R | CONVF_G },
        { WIN11_DXGI_FORMAT_A4B4G4R4_UNORM,          4, CONVF_UNORM | CONVF_BGR | CONVF_R | CONVF_G | CONVF_B | CONVF_A },
    };

#pragma prefast( suppress : 25004, "Signature must match bsearch" );
    int __cdecl ConvertCompare(const void* ptr1, const void *ptr2) noexcept
    {
        auto p1 = static_cast<const ConvertData*>(ptr1);
        auto p2 = static_cast<const ConvertData*>(ptr2);
        if (p1->format == p2->format) return 0;
        else return (p1->format < p2->format) ? -1 : 1;
    }
}

_Use_decl_annotations_
uint32_t DirectX::Internal::GetConvertFlags(DXGI_FORMAT format) noexcept
{
#ifdef _DEBUG
    // Ensure conversion table is in ascending order
    assert(std::size(g_ConvertTable) > 0);
    DXGI_FORMAT lastvalue = g_ConvertTable[0].format;
    for (size_t index = 1; index < std::size(g_ConvertTable); ++index)
    {
        assert(g_ConvertTable[index].format > lastvalue);
        lastvalue = g_ConvertTable[index].format;
    }
#endif

    ConvertData key = { format, 0, 0 };
    auto in = reinterpret_cast<const ConvertData*>(bsearch(&key, g_ConvertTable, std::size(g_ConvertTable), sizeof(ConvertData),
        ConvertCompare));
    return (in) ? in->flags : 0;
}

_Use_decl_annotations_
void DirectX::Internal::ConvertScanline(
    XMVECTOR* pBuffer,
    size_t count,
    DXGI_FORMAT outFormat,
    DXGI_FORMAT inFormat,
    TEX_FILTER_FLAGS flags) noexcept
{
    assert(pBuffer && count > 0 && ((reinterpret_cast<uintptr_t>(pBuffer) & 0xF) == 0));
    assert(IsValid(outFormat) && !IsTypeless(outFormat) && !IsPlanar(outFormat) && !IsPalettized(outFormat));
    assert(IsValid(inFormat) && !IsTypeless(inFormat) && !IsPlanar(inFormat) && !IsPalettized(inFormat));

    if (!pBuffer)
        return;

#ifdef _DEBUG
    // Ensure conversion table is in ascending order
    assert(std::size(g_ConvertTable) > 0);
    DXGI_FORMAT lastvalue = g_ConvertTable[0].format;
    for (size_t index = 1; index < std::size(g_ConvertTable); ++index)
    {
        assert(g_ConvertTable[index].format > lastvalue);
        lastvalue = g_ConvertTable[index].format;
    }
#endif

    // Determine conversion details about source and dest formats
    ConvertData key = { inFormat, 0, 0 };
    auto in = reinterpret_cast<const ConvertData*>(
        bsearch(&key, g_ConvertTable, std::size(g_ConvertTable), sizeof(ConvertData), ConvertCompare));
    key.format = outFormat;
    auto out = reinterpret_cast<const ConvertData*>(
        bsearch(&key, g_ConvertTable, std::size(g_ConvertTable), sizeof(ConvertData), ConvertCompare));
    if (!in || !out)
    {
        assert(false);
        return;
    }

    assert(GetConvertFlags(inFormat) == in->flags);
    assert(GetConvertFlags(outFormat) == out->flags);

    // Handle SRGB filtering modes
    switch (inFormat)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        flags |= TEX_FILTER_SRGB_IN;
        break;

    case DXGI_FORMAT_A8_UNORM:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        flags &= ~TEX_FILTER_SRGB_IN;
        break;

    default:
        break;
    }

    switch (outFormat)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        flags |= TEX_FILTER_SRGB_OUT;
        break;

    case DXGI_FORMAT_A8_UNORM:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        flags &= ~TEX_FILTER_SRGB_OUT;
        break;

    default:
        break;
    }

    if ((flags & (TEX_FILTER_SRGB_IN | TEX_FILTER_SRGB_OUT)) == (TEX_FILTER_SRGB_IN | TEX_FILTER_SRGB_OUT))
    {
        flags &= ~(TEX_FILTER_SRGB_IN | TEX_FILTER_SRGB_OUT);
    }

    // sRGB input processing (sRGB -> Linear RGB)
    if (flags & TEX_FILTER_SRGB_IN)
    {
        if (!(in->flags & CONVF_DEPTH) && ((in->flags & CONVF_FLOAT) || (in->flags & CONVF_UNORM)))
        {
            XMVECTOR* ptr = pBuffer;
            for (size_t i = 0; i < count; ++i, ++ptr)
            {
                *ptr = XMColorSRGBToRGB(*ptr);
            }
        }
    }

    // Handle conversion special cases
    const uint32_t diffFlags = in->flags ^ out->flags;
    if (diffFlags != 0)
    {
        if (diffFlags & CONVF_DEPTH)
        {
            //--- Depth conversions ---
            if (in->flags & CONVF_DEPTH)
            {
                // CONVF_DEPTH -> !CONVF_DEPTH
                if (in->flags & CONVF_STENCIL)
                {
                    // Stencil -> Alpha
                    static const XMVECTORF32 S = { { { 1.f, 1.f, 1.f, 255.f } } };

                    if (out->flags & CONVF_UNORM)
                    {
                        // UINT -> UNORM
                        XMVECTOR* ptr = pBuffer;
                        for (size_t i = 0; i < count; ++i)
                        {
                            const XMVECTOR v = *ptr;
                            XMVECTOR v1 = XMVectorSplatY(v);
                            v1 = XMVectorClamp(v1, g_XMZero, S);
                            v1 = XMVectorDivide(v1, S);
                            *ptr++ = XMVectorSelect(v1, v, g_XMSelect1110);
                        }
                    }
                    else if (out->flags & CONVF_SNORM)
                    {
                        // UINT -> SNORM
                        XMVECTOR* ptr = pBuffer;
                        for (size_t i = 0; i < count; ++i)
                        {
                            const XMVECTOR v = *ptr;
                            XMVECTOR v1 = XMVectorSplatY(v);
                            v1 = XMVectorClamp(v1, g_XMZero, S);
                            v1 = XMVectorDivide(v1, S);
                            v1 = XMVectorMultiplyAdd(v1, g_XMTwo, g_XMNegativeOne);
                            *ptr++ = XMVectorSelect(v1, v, g_XMSelect1110);
                        }
                    }
                    else
                    {
                        XMVECTOR* ptr = pBuffer;
                        for (size_t i = 0; i < count; ++i)
                        {
                            const XMVECTOR v = *ptr;
                            const XMVECTOR v1 = XMVectorSplatY(v);
                            *ptr++ = XMVectorSelect(v1, v, g_XMSelect1110);
                        }
                    }
                }

                // Depth -> RGB
                if ((out->flags & CONVF_UNORM) && (in->flags & CONVF_FLOAT))
                {
                    // Depth FLOAT -> UNORM
                    XMVECTOR* ptr = pBuffer;
                    for (size_t i = 0; i < count; ++i)
                    {
                        const XMVECTOR v = *ptr;
                        XMVECTOR v1 = XMVectorSaturate(v);
                        v1 = XMVectorSplatX(v1);
                        *ptr++ = XMVectorSelect(v, v1, g_XMSelect1110);
                    }
                }
                else if (out->flags & CONVF_SNORM)
                {
                    if (in->flags & CONVF_UNORM)
                    {
                        // Depth UNORM -> SNORM
                        XMVECTOR* ptr = pBuffer;
                        for (size_t i = 0; i < count; ++i)
                        {
                            const XMVECTOR v = *ptr;
                            XMVECTOR v1 = XMVectorMultiplyAdd(v, g_XMTwo, g_XMNegativeOne);
                            v1 = XMVectorSplatX(v1);
                            *ptr++ = XMVectorSelect(v, v1, g_XMSelect1110);
                        }
                    }
                    else
                    {
                        // Depth FLOAT -> SNORM
                        XMVECTOR* ptr = pBuffer;
                        for (size_t i = 0; i < count; ++i)
                        {
                            const XMVECTOR v = *ptr;
                            XMVECTOR v1 = XMVectorClamp(v, g_XMNegativeOne, g_XMOne);
                            v1 = XMVectorSplatX(v1);
                            *ptr++ = XMVectorSelect(v, v1, g_XMSelect1110);
                        }
                    }
                }
                else
                {
                    XMVECTOR* ptr = pBuffer;
                    for (size_t i = 0; i < count; ++i)
                    {
                        const XMVECTOR v = *ptr;
                        const XMVECTOR v1 = XMVectorSplatX(v);
                        *ptr++ = XMVectorSelect(v, v1, g_XMSelect1110);
                    }
                }
            }
            else
            {
                // !CONVF_DEPTH -> CONVF_DEPTH

                // RGB -> Depth (red channel or other specified channel)
                switch (flags & (TEX_FILTER_RGB_COPY_RED | TEX_FILTER_RGB_COPY_GREEN | TEX_FILTER_RGB_COPY_BLUE | TEX_FILTER_RGB_COPY_ALPHA))
                {
                case TEX_FILTER_RGB_COPY_GREEN:
                    {
                        XMVECTOR* ptr = pBuffer;
                        for (size_t i = 0; i < count; ++i)
                        {
                            const XMVECTOR v = *ptr;
                            const XMVECTOR v1 = XMVectorSplatY(v);
                            *ptr++ = XMVectorSelect(v, v1, g_XMSelect1000);
                        }
                    }
                    break;

                case TEX_FILTER_RGB_COPY_BLUE:
                    {
                        XMVECTOR* ptr = pBuffer;
                        for (size_t i = 0; i < count; ++i)
                        {
                            const XMVECTOR v = *ptr;
                            const XMVECTOR v1 = XMVectorSplatZ(v);
                            *ptr++ = XMVectorSelect(v, v1, g_XMSelect1000);
                        }
                    }
                    break;

                case TEX_FILTER_RGB_COPY_ALPHA:
                    {
                        XMVECTOR* ptr = pBuffer;
                        for (size_t i = 0; i < count; ++i)
                        {
                            const XMVECTOR v = *ptr;
                            const XMVECTOR v1 = XMVectorSplatW(v);
                            *ptr++ = XMVectorSelect(v, v1, g_XMSelect1000);
                        }
                    }
                    break;

                default:
                    if ((in->flags & CONVF_UNORM) && ((in->flags & CONVF_RGB_MASK) == (CONVF_R | CONVF_G | CONVF_B)))
                    {
                        XMVECTOR* ptr = pBuffer;
                        for (size_t i = 0; i < count; ++i)
                        {
                            const XMVECTOR v = *ptr;
                            const XMVECTOR v1 = XMVector3Dot(v, g_Grayscale);
                            *ptr++ = XMVectorSelect(v, v1, g_XMSelect1000);
                        }
                        break;
                    }

                #if (__cplusplus >= 201703L)
                    [[fallthrough]];
                #elif defined(__clang__)
                    [[clang::fallthrough]];
                #elif defined(_MSC_VER)
                    __fallthrough;
                #endif

                case TEX_FILTER_RGB_COPY_RED:
                    {
                        XMVECTOR* ptr = pBuffer;
                        for (size_t i = 0; i < count; ++i)
                        {
                            const XMVECTOR v = *ptr;
                            const XMVECTOR v1 = XMVectorSplatX(v);
                            *ptr++ = XMVectorSelect(v, v1, g_XMSelect1000);
                        }
                    }
                    break;
                }

                // Finialize type conversion for depth (red channel)
                if (out->flags & CONVF_UNORM)
                {
                    if (in->flags & CONVF_SNORM)
                    {
                        // SNORM -> UNORM
                        XMVECTOR* ptr = pBuffer;
                        for (size_t i = 0; i < count; ++i)
                        {
                            const XMVECTOR v = *ptr;
                            const XMVECTOR v1 = XMVectorMultiplyAdd(v, g_XMOneHalf, g_XMOneHalf);
                            *ptr++ = XMVectorSelect(v, v1, g_XMSelect1000);
                        }
                    }
                    else if (in->flags & CONVF_FLOAT)
                    {
                        // FLOAT -> UNORM
                        XMVECTOR* ptr = pBuffer;
                        for (size_t i = 0; i < count; ++i)
                        {
                            const XMVECTOR v = *ptr;
                            const XMVECTOR v1 = XMVectorSaturate(v);
                            *ptr++ = XMVectorSelect(v, v1, g_XMSelect1000);
                        }
                    }
                }

                if (out->flags & CONVF_STENCIL)
                {
                    // Alpha -> Stencil (green channel)
                    static const XMVECTORU32 select0100 = { { { XM_SELECT_0, XM_SELECT_1, XM_SELECT_0, XM_SELECT_0 } } };
                    static const XMVECTORF32 S = { { { 255.f, 255.f, 255.f, 255.f } } };

                    if (in->flags & CONVF_UNORM)
                    {
                        // UNORM -> UINT
                        XMVECTOR* ptr = pBuffer;
                        for (size_t i = 0; i < count; ++i)
                        {
                            const XMVECTOR v = *ptr;
                            XMVECTOR v1 = XMVectorMultiply(v, S);
                            v1 = XMVectorSplatW(v1);
                            *ptr++ = XMVectorSelect(v, v1, select0100);
                        }
                    }
                    else if (in->flags & CONVF_SNORM)
                    {
                        // SNORM -> UINT
                        XMVECTOR* ptr = pBuffer;
                        for (size_t i = 0; i < count; ++i)
                        {
                            const XMVECTOR v = *ptr;
                            XMVECTOR v1 = XMVectorMultiplyAdd(v, g_XMOneHalf, g_XMOneHalf);
                            v1 = XMVectorMultiply(v1, S);
                            v1 = XMVectorSplatW(v1);
                            *ptr++ = XMVectorSelect(v, v1, select0100);
                        }
                    }
                    else
                    {
                        XMVECTOR* ptr = pBuffer;
                        for (size_t i = 0; i < count; ++i)
                        {
                            const XMVECTOR v = *ptr;
                            const XMVECTOR v1 = XMVectorSplatW(v);
                            *ptr++ = XMVectorSelect(v, v1, select0100);
                        }
                    }
                }
            }
        }
        else if (out->flags & CONVF_DEPTH)
        {
            // CONVF_DEPTH -> CONVF_DEPTH
            if (diffFlags & CONVF_FLOAT)
            {
                if (in->flags & CONVF_FLOAT)
                {
                    // FLOAT -> UNORM depth, preserve stencil
                    XMVECTOR* ptr = pBuffer;
                    for (size_t i = 0; i < count; ++i)
                    {
                        const XMVECTOR v = *ptr;
                        const XMVECTOR v1 = XMVectorSaturate(v);
                        *ptr++ = XMVectorSelect(v, v1, g_XMSelect1000);
                    }
                }
            }
        }
        else if (out->flags & CONVF_UNORM)
        {
            //--- Converting to a UNORM ---
            if (in->flags & CONVF_SNORM)
            {
                // SNORM -> UNORM
                XMVECTOR* ptr = pBuffer;
                for (size_t i = 0; i < count; ++i)
                {
                    const XMVECTOR v = *ptr;
                    *ptr++ = XMVectorMultiplyAdd(v, g_XMOneHalf, g_XMOneHalf);
                }
            }
            else if (in->flags & CONVF_FLOAT)
            {
                XMVECTOR* ptr = pBuffer;
                if (!(in->flags & CONVF_POS_ONLY) && (flags & TEX_FILTER_FLOAT_X2BIAS))
                {
                    // FLOAT -> UNORM (x2 bias)
                    for (size_t i = 0; i < count; ++i)
                    {
                        XMVECTOR v = *ptr;
                        v = XMVectorClamp(v, g_XMNegativeOne, g_XMOne);
                        *ptr++ = XMVectorMultiplyAdd(v, g_XMOneHalf, g_XMOneHalf);
                    }
                }
                else
                {
                    // FLOAT -> UNORM
                    for (size_t i = 0; i < count; ++i)
                    {
                        const XMVECTOR v = *ptr;
                        *ptr++ = XMVectorSaturate(v);
                    }
                }
            }
        }
        else if (out->flags & CONVF_SNORM)
        {
            //--- Converting to a SNORM ---
            if (in->flags & CONVF_UNORM)
            {
                // UNORM -> SNORM
                XMVECTOR* ptr = pBuffer;
                for (size_t i = 0; i < count; ++i)
                {
                    const XMVECTOR v = *ptr;
                    *ptr++ = XMVectorMultiplyAdd(v, g_XMTwo, g_XMNegativeOne);
                }
            }
            else if (in->flags & CONVF_FLOAT)
            {
                XMVECTOR* ptr = pBuffer;
                if ((in->flags & CONVF_POS_ONLY) && (flags & TEX_FILTER_FLOAT_X2BIAS))
                {
                    // FLOAT (positive only, x2 bias) -> SNORM
                    for (size_t i = 0; i < count; ++i)
                    {
                        XMVECTOR v = *ptr;
                        v = XMVectorSaturate(v);
                        *ptr++ = XMVectorMultiplyAdd(v, g_XMTwo, g_XMNegativeOne);
                    }
                }
                else
                {
                    // FLOAT -> SNORM
                    for (size_t i = 0; i < count; ++i)
                    {
                        const XMVECTOR v = *ptr;
                        *ptr++ = XMVectorClamp(v, g_XMNegativeOne, g_XMOne);
                    }
                }
            }
        }
        else if (diffFlags & CONVF_UNORM)
        {
            //--- Converting from a UNORM ---
            assert(in->flags & CONVF_UNORM);
            if (out->flags & CONVF_FLOAT)
            {
                if (!(out->flags & CONVF_POS_ONLY) && (flags & TEX_FILTER_FLOAT_X2BIAS))
                {
                    // UNORM (x2 bias) -> FLOAT
                    XMVECTOR* ptr = pBuffer;
                    for (size_t i = 0; i < count; ++i)
                    {
                        const XMVECTOR v = *ptr;
                        *ptr++ = XMVectorMultiplyAdd(v, g_XMTwo, g_XMNegativeOne);
                    }
                }
            }
        }
        else if (diffFlags & CONVF_POS_ONLY)
        {
            if (flags & TEX_FILTER_FLOAT_X2BIAS)
            {
                if (in->flags & CONVF_POS_ONLY)
                {
                    if (out->flags & CONVF_FLOAT)
                    {
                        // FLOAT (positive only, x2 bias) -> FLOAT
                        XMVECTOR* ptr = pBuffer;
                        for (size_t i = 0; i < count; ++i)
                        {
                            XMVECTOR v = *ptr;
                            v = XMVectorSaturate(v);
                            *ptr++ = XMVectorMultiplyAdd(v, g_XMTwo, g_XMNegativeOne);
                        }
                    }
                }
                else if (out->flags & CONVF_POS_ONLY)
                {
                    if (in->flags & CONVF_FLOAT)
                    {
                        // FLOAT -> FLOAT (positive only, x2 bias)
                        XMVECTOR* ptr = pBuffer;
                        for (size_t i = 0; i < count; ++i)
                        {
                            XMVECTOR v = *ptr;
                            v = XMVectorClamp(v, g_XMNegativeOne, g_XMOne);
                            *ptr++ = XMVectorMultiplyAdd(v, g_XMOneHalf, g_XMOneHalf);
                        }
                    }
                    else if (in->flags & CONVF_SNORM)
                    {
                        // SNORM -> FLOAT (positive only, x2 bias)
                        XMVECTOR* ptr = pBuffer;
                        for (size_t i = 0; i < count; ++i)
                        {
                            const XMVECTOR v = *ptr;
                            *ptr++ = XMVectorMultiplyAdd(v, g_XMOneHalf, g_XMOneHalf);
                        }
                    }
                }
            }
        }

        // !CONVF_A -> CONVF_A is handled because LoadScanline ensures alpha defaults to 1.0 for no-alpha formats

        // CONVF_PACKED cases are handled because LoadScanline/StoreScanline handles packing/unpacking

        if (((out->flags & CONVF_RGBA_MASK) == CONVF_A) && !(in->flags & CONVF_A))
        {
            // !CONVF_A -> A format
            // We ignore TEX_FILTER_RGB_COPY_ALPHA since there's no input alpha channel.
            switch (flags & (TEX_FILTER_RGB_COPY_RED | TEX_FILTER_RGB_COPY_GREEN | TEX_FILTER_RGB_COPY_BLUE))
            {
            case TEX_FILTER_RGB_COPY_GREEN:
                {
                    XMVECTOR* ptr = pBuffer;
                    for (size_t i = 0; i < count; ++i)
                    {
                        const XMVECTOR v = *ptr;
                        *ptr++ = XMVectorSplatY(v);
                    }
                }
                break;

            case TEX_FILTER_RGB_COPY_BLUE:
                {
                    XMVECTOR* ptr = pBuffer;
                    for (size_t i = 0; i < count; ++i)
                    {
                        const XMVECTOR v = *ptr;
                        *ptr++ = XMVectorSplatZ(v);
                    }
                }
                break;

            default:
                if ((in->flags & CONVF_UNORM) && ((in->flags & CONVF_RGB_MASK) == (CONVF_R | CONVF_G | CONVF_B)))
                {
                    XMVECTOR* ptr = pBuffer;
                    for (size_t i = 0; i < count; ++i)
                    {
                        const XMVECTOR v = *ptr;
                        *ptr++ = XMVector3Dot(v, g_Grayscale);
                    }
                    break;
                }

            #if (__cplusplus >= 201703L)
                [[fallthrough]];
            #elif defined(__clang__)
                [[clang::fallthrough]];
            #elif defined(_MSC_VER)
                __fallthrough;
            #endif

            case TEX_FILTER_RGB_COPY_RED:
                {
                    XMVECTOR* ptr = pBuffer;
                    for (size_t i = 0; i < count; ++i)
                    {
                        const XMVECTOR v = *ptr;
                        *ptr++ = XMVectorSplatX(v);
                    }
                }
                break;
            }
        }
        else if (((in->flags & CONVF_RGBA_MASK) == CONVF_A) && !(out->flags & CONVF_A))
        {
            // A format -> !CONVF_A
            XMVECTOR* ptr = pBuffer;
            for (size_t i = 0; i < count; ++i)
            {
                const XMVECTOR v = *ptr;
                *ptr++ = XMVectorSplatW(v);
            }
        }
        else if ((in->flags & CONVF_RGB_MASK) == CONVF_R)
        {
            if ((out->flags & CONVF_RGB_MASK) == (CONVF_R | CONVF_G | CONVF_B))
            {
                // R format -> RGB format
                XMVECTOR* ptr = pBuffer;
                for (size_t i = 0; i < count; ++i)
                {
                    const XMVECTOR v = *ptr;
                    const XMVECTOR v1 = XMVectorSplatX(v);
                    *ptr++ = XMVectorSelect(v, v1, g_XMSelect1110);
                }
            }
            else if ((out->flags & CONVF_RGB_MASK) == (CONVF_R | CONVF_G))
            {
                // R format -> RG format
                XMVECTOR* ptr = pBuffer;
                for (size_t i = 0; i < count; ++i)
                {
                    const XMVECTOR v = *ptr;
                    const XMVECTOR v1 = XMVectorSplatX(v);
                    *ptr++ = XMVectorSelect(v, v1, g_XMSelect1100);
                }
            }
        }
        else if ((in->flags & CONVF_RGB_MASK) == (CONVF_R | CONVF_G | CONVF_B))
        {
            if ((out->flags & CONVF_RGB_MASK) == CONVF_R)
            {
                // RGB(A) format -> R format
                switch (flags & (TEX_FILTER_RGB_COPY_RED | TEX_FILTER_RGB_COPY_GREEN | TEX_FILTER_RGB_COPY_BLUE | TEX_FILTER_RGB_COPY_ALPHA))
                {
                case TEX_FILTER_RGB_COPY_GREEN:
                    {
                        XMVECTOR* ptr = pBuffer;
                        for (size_t i = 0; i < count; ++i)
                        {
                            const XMVECTOR v = *ptr;
                            const XMVECTOR v1 = XMVectorSplatY(v);
                            *ptr++ = XMVectorSelect(v, v1, g_XMSelect1110);
                        }
                    }
                    break;

                case TEX_FILTER_RGB_COPY_BLUE:
                    {
                        XMVECTOR* ptr = pBuffer;
                        for (size_t i = 0; i < count; ++i)
                        {
                            const XMVECTOR v = *ptr;
                            const XMVECTOR v1 = XMVectorSplatZ(v);
                            *ptr++ = XMVectorSelect(v, v1, g_XMSelect1110);
                        }
                    }
                    break;

                case TEX_FILTER_RGB_COPY_ALPHA:
                    {
                        XMVECTOR* ptr = pBuffer;
                        for (size_t i = 0; i < count; ++i)
                        {
                            const XMVECTOR v = *ptr;
                            const XMVECTOR v1 = XMVectorSplatW(v);
                            *ptr++ = XMVectorSelect(v, v1, g_XMSelect1110);
                        }
                    }
                    break;

                default:
                    if (in->flags & CONVF_UNORM)
                    {
                        XMVECTOR* ptr = pBuffer;
                        for (size_t i = 0; i < count; ++i)
                        {
                            const XMVECTOR v = *ptr;
                            const XMVECTOR v1 = XMVector3Dot(v, g_Grayscale);
                            *ptr++ = XMVectorSelect(v, v1, g_XMSelect1110);
                        }
                        break;
                    }

                #if (__cplusplus >= 201703L)
                    [[fallthrough]];
                #elif defined(__clang__)
                    [[clang::fallthrough]];
                #elif defined(_MSC_VER)
                    __fallthrough;
                #endif

                case TEX_FILTER_RGB_COPY_RED:
                    // Leave data unchanged and the store will handle this...
                    break;
                }
            }
            else if ((out->flags & CONVF_RGB_MASK) == (CONVF_R | CONVF_G))
            {
                if ((flags & TEX_FILTER_RGB_COPY_ALPHA) && (in->flags & CONVF_A))
                {
                    // RGBA -> RG format
                    switch (static_cast<int>(flags & (TEX_FILTER_RGB_COPY_RED | TEX_FILTER_RGB_COPY_GREEN | TEX_FILTER_RGB_COPY_BLUE | TEX_FILTER_RGB_COPY_ALPHA)))
                    {
                    case (static_cast<int>(TEX_FILTER_RGB_COPY_RED) | static_cast<int>(TEX_FILTER_RGB_COPY_ALPHA)):
                    default:
                        {
                            XMVECTOR* ptr = pBuffer;
                            for (size_t i = 0; i < count; ++i)
                            {
                                const XMVECTOR v = *ptr;
                                const XMVECTOR v1 = XMVectorSwizzle<0, 3, 0, 3>(v);
                                *ptr++ = XMVectorSelect(v, v1, g_XMSelect1100);
                            }
                        }
                        break;

                    case (static_cast<int>(TEX_FILTER_RGB_COPY_GREEN) | static_cast<int>(TEX_FILTER_RGB_COPY_ALPHA)):
                        {
                            XMVECTOR* ptr = pBuffer;
                            for (size_t i = 0; i < count; ++i)
                            {
                                const XMVECTOR v = *ptr;
                                const XMVECTOR v1 = XMVectorSwizzle<1, 3, 1, 3>(v);
                                *ptr++ = XMVectorSelect(v, v1, g_XMSelect1100);
                            }
                        }
                        break;

                    case (static_cast<int>(TEX_FILTER_RGB_COPY_BLUE) | static_cast<int>(TEX_FILTER_RGB_COPY_ALPHA)):
                        {
                            XMVECTOR* ptr = pBuffer;
                            for (size_t i = 0; i < count; ++i)
                            {
                                const XMVECTOR v = *ptr;
                                const XMVECTOR v1 = XMVectorSwizzle<2, 3, 2, 3>(v);
                                *ptr++ = XMVectorSelect(v, v1, g_XMSelect1100);
                            }
                        }
                        break;
                    }
                }
                else
                {
                    // RGB format -> RG format
                    switch (static_cast<int>(flags & (TEX_FILTER_RGB_COPY_RED | TEX_FILTER_RGB_COPY_GREEN | TEX_FILTER_RGB_COPY_BLUE)))
                    {
                    case (static_cast<int>(TEX_FILTER_RGB_COPY_RED) | static_cast<int>(TEX_FILTER_RGB_COPY_BLUE)):
                        {
                            XMVECTOR* ptr = pBuffer;
                            for (size_t i = 0; i < count; ++i)
                            {
                                const XMVECTOR v = *ptr;
                                const XMVECTOR v1 = XMVectorSwizzle<0, 2, 0, 2>(v);
                                *ptr++ = XMVectorSelect(v, v1, g_XMSelect1100);
                            }
                        }
                        break;

                    case (static_cast<int>(TEX_FILTER_RGB_COPY_GREEN) | static_cast<int>(TEX_FILTER_RGB_COPY_BLUE)):
                        {
                            XMVECTOR* ptr = pBuffer;
                            for (size_t i = 0; i < count; ++i)
                            {
                                const XMVECTOR v = *ptr;
                                const XMVECTOR v1 = XMVectorSwizzle<1, 2, 3, 0>(v);
                                *ptr++ = XMVectorSelect(v, v1, g_XMSelect1100);
                            }
                        }
                        break;

                    case (static_cast<int>(TEX_FILTER_RGB_COPY_RED) | static_cast<int>(TEX_FILTER_RGB_COPY_GREEN)):
                    default:
                        // Leave data unchanged and the store will handle this...
                        break;
                    }
                }
            }
        }
    }

    // sRGB output processing (Linear RGB -> sRGB)
    if (flags & TEX_FILTER_SRGB_OUT)
    {
        if (!(out->flags & CONVF_DEPTH) && ((out->flags & CONVF_FLOAT) || (out->flags & CONVF_UNORM)))
        {
            XMVECTOR* ptr = pBuffer;
            for (size_t i = 0; i < count; ++i, ++ptr)
            {
                *ptr = XMColorRGBToSRGB(*ptr);
            }
        }
    }
}


//-------------------------------------------------------------------------------------
// Dithering
//-------------------------------------------------------------------------------------
namespace
{
    // 4X4X4 ordered dithering matrix
    const float g_Dither[] =
    {
        // (z & 3) + ( (y & 3) * 8) + (x & 3)
        0.468750f,  -0.031250f, 0.343750f, -0.156250f, 0.468750f, -0.031250f, 0.343750f, -0.156250f,
        -0.281250f,  0.218750f, -0.406250f, 0.093750f, -0.281250f, 0.218750f, -0.406250f, 0.093750f,
        0.281250f,  -0.218750f, 0.406250f, -0.093750f, 0.281250f, -0.218750f, 0.406250f, -0.093750f,
        -0.468750f,  0.031250f, -0.343750f, 0.156250f, -0.468750f, 0.031250f, -0.343750f, 0.156250f,
    };

    const XMVECTORF32 g_Scale16pc = { { { 65535.f, 65535.f, 65535.f, 65535.f } } };
    const XMVECTORF32 g_Scale15pc = { { { 32767.f, 32767.f, 32767.f, 32767.f } } };
    const XMVECTORF32 g_Scale10pc = { { {  1023.f,  1023.f,  1023.f,     3.f } } };
    const XMVECTORF32 g_Scale9pc = { { {   511.f,   511.f,   511.f,     3.f } } };
    const XMVECTORF32 g_Scale8pc = { { {   255.f,   255.f,   255.f,   255.f } } };
    const XMVECTORF32 g_Scale7pc = { { {   127.f,   127.f,   127.f,   127.f } } };
    const XMVECTORF32 g_Scale565pc = { { {    31.f,    63.f,    31.f,     1.f } } };
    const XMVECTORF32 g_Scale5551pc = { { {    31.f,    31.f,    31.f,     1.f } } };
    const XMVECTORF32 g_Scale4pc = { { {    15.f,    15.f,    15.f,    15.f } } };

    const XMVECTORF32 g_ErrorWeight3 = { { { 3.f / 16.f, 3.f / 16.f, 3.f / 16.f, 3.f / 16.f } } };
    const XMVECTORF32 g_ErrorWeight5 = { { { 5.f / 16.f, 5.f / 16.f, 5.f / 16.f, 5.f / 16.f } } };
    const XMVECTORF32 g_ErrorWeight1 = { { { 1.f / 16.f, 1.f / 16.f, 1.f / 16.f, 1.f / 16.f } } };
    const XMVECTORF32 g_ErrorWeight7 = { { { 7.f / 16.f, 7.f / 16.f, 7.f / 16.f, 7.f / 16.f } } };

#define STORE_SCANLINE( type, scalev, clampzero, norm, itype, mask, row, bgr ) \
        if (size >= sizeof(type)) \
        { \
            type * __restrict dest = reinterpret_cast<type*>(pDestination); \
            for(size_t i = 0; i < count; ++i) \
            { \
                auto index = static_cast<ptrdiff_t>((row & 1) ? (count - i - 1) : i ); \
                ptrdiff_t delta = (row & 1) ? -2 : 0; \
                \
                XMVECTOR v = sPtr[ index ]; \
                if (bgr) { v = XMVectorSwizzle<2, 1, 0, 3>(v); } \
                if (norm && clampzero) v = XMVectorSaturate(v) ; \
                else if (clampzero) v = XMVectorClamp(v, g_XMZero, scalev); \
                else if (norm) v = XMVectorClamp(v, g_XMNegativeOne, g_XMOne); \
                else v = XMVectorClamp(v, XMVectorAdd(XMVectorNegate(scalev), g_XMOne), scalev); \
                v = XMVectorAdd(v, vError); \
                if (norm) v = XMVectorMultiply(v, scalev); \
                \
                XMVECTOR target; \
                if (pDiffusionErrors) \
                { \
                    target = XMVectorRound(v); \
                    vError = XMVectorSubtract(v, target); \
                    if (norm) vError = XMVectorDivide(vError, scalev); \
                    \
                    /* Distribute error to next scanline and next pixel */ \
                    pDiffusionErrors[ index-delta ]   = XMVectorMultiplyAdd(g_ErrorWeight3, vError, pDiffusionErrors[ index-delta ]); \
                    pDiffusionErrors[ index+1 ]       = XMVectorMultiplyAdd(g_ErrorWeight5, vError, pDiffusionErrors[ index+1 ]); \
                    pDiffusionErrors[ index+2+delta ] = XMVectorMultiplyAdd(g_ErrorWeight1, vError, pDiffusionErrors[ index+2+delta ]); \
                    vError = XMVectorMultiply(vError, g_ErrorWeight7); \
                } \
                else \
                { \
                    /* Applied ordered dither */ \
                    target = XMVectorAdd(v, ordered[ index & 3 ]); \
                    target = XMVectorRound(target); \
                } \
                \
                target = XMVectorMin(scalev, target); \
                target = XMVectorMax((clampzero) ? g_XMZero : (XMVectorAdd(XMVectorNegate(scalev), g_XMOne)), target); \
                \
                XMFLOAT4A tmp; \
                XMStoreFloat4A(&tmp, target); \
                \
                auto dPtr = &dest[ index ]; \
                if (dPtr >= ePtr) break; \
                dPtr->x = itype(static_cast<itype>(tmp.x) & mask); \
                dPtr->y = itype(static_cast<itype>(tmp.y) & mask); \
                dPtr->z = itype(static_cast<itype>(tmp.z) & mask); \
                dPtr->w = itype(static_cast<itype>(tmp.w) & mask); \
            } \
            return true; \
        } \
        return false;

#define STORE_SCANLINE2( type, scalev, clampzero, norm, itype, mask, row ) \
        /* The 2 component cases are always bgr=false */ \
        if (size >= sizeof(type)) \
        { \
            type * __restrict dest = reinterpret_cast<type*>(pDestination); \
            for(size_t i = 0; i < count; ++i) \
            { \
                auto index = static_cast<ptrdiff_t>((row & 1) ? (count - i - 1) : i ); \
                ptrdiff_t delta = (row & 1) ? -2 : 0; \
                \
                XMVECTOR v = sPtr[ index ]; \
                if (norm && clampzero) v = XMVectorSaturate(v) ; \
                else if (clampzero) v = XMVectorClamp(v, g_XMZero, scalev); \
                else if (norm) v = XMVectorClamp(v, g_XMNegativeOne, g_XMOne); \
                else v = XMVectorClamp(v, XMVectorAdd(XMVectorNegate(scalev), g_XMOne), scalev); \
                v = XMVectorAdd(v, vError); \
                if (norm) v = XMVectorMultiply(v, scalev); \
                \
                XMVECTOR target; \
                if (pDiffusionErrors) \
                { \
                    target = XMVectorRound(v); \
                    vError = XMVectorSubtract(v, target); \
                    if (norm) vError = XMVectorDivide(vError, scalev); \
                    \
                    /* Distribute error to next scanline and next pixel */ \
                    pDiffusionErrors[ index-delta ]   = XMVectorMultiplyAdd(g_ErrorWeight3, vError, pDiffusionErrors[ index-delta ]); \
                    pDiffusionErrors[ index+1 ]       = XMVectorMultiplyAdd(g_ErrorWeight5, vError, pDiffusionErrors[ index+1 ]); \
                    pDiffusionErrors[ index+2+delta ] = XMVectorMultiplyAdd(g_ErrorWeight1, vError, pDiffusionErrors[ index+2+delta ]); \
                    vError = XMVectorMultiply(vError, g_ErrorWeight7); \
                } \
                else \
                { \
                    /* Applied ordered dither */ \
                    target = XMVectorAdd(v, ordered[ index & 3 ]); \
                    target = XMVectorRound(target); \
                } \
                \
                target = XMVectorMin(scalev, target); \
                target = XMVectorMax((clampzero) ? g_XMZero : (XMVectorAdd(XMVectorNegate(scalev), g_XMOne)), target); \
                \
                XMFLOAT4A tmp; \
                XMStoreFloat4A(&tmp, target); \
                \
                auto dPtr = &dest[ index ]; \
                if (dPtr >= ePtr) break; \
                dPtr->x = itype(static_cast<itype>(tmp.x) & mask); \
                dPtr->y = itype(static_cast<itype>(tmp.y) & mask); \
            } \
            return true; \
        } \
        return false;

#define STORE_SCANLINE1( type, scalev, clampzero, norm, mask, row, selectw ) \
        /* The 1 component cases are always bgr=false */ \
        if (size >= sizeof(type)) \
        { \
            type * __restrict dest = reinterpret_cast<type*>(pDestination); \
            for(size_t i = 0; i < count; ++i) \
            { \
                auto index = static_cast<ptrdiff_t>((row & 1) ? (count - i - 1) : i ); \
                ptrdiff_t delta = (row & 1) ? -2 : 0; \
                \
                XMVECTOR v = sPtr[ index ]; \
                if (norm && clampzero) v = XMVectorSaturate(v) ; \
                else if (clampzero) v = XMVectorClamp(v, g_XMZero, scalev); \
                else if (norm) v = XMVectorClamp(v, g_XMNegativeOne, g_XMOne); \
                else v = XMVectorClamp(v, XMVectorAdd(XMVectorNegate(scalev), g_XMOne), scalev); \
                v = XMVectorAdd(v, vError); \
                if (norm) v = XMVectorMultiply(v, scalev); \
                \
                XMVECTOR target; \
                if (pDiffusionErrors) \
                { \
                    target = XMVectorRound(v); \
                    vError = XMVectorSubtract(v, target); \
                    if (norm) vError = XMVectorDivide(vError, scalev); \
                    \
                    /* Distribute error to next scanline and next pixel */ \
                    pDiffusionErrors[ index-delta ]   = XMVectorMultiplyAdd(g_ErrorWeight3, vError, pDiffusionErrors[ index-delta ]); \
                    pDiffusionErrors[ index+1 ]       = XMVectorMultiplyAdd(g_ErrorWeight5, vError, pDiffusionErrors[ index+1 ]); \
                    pDiffusionErrors[ index+2+delta ] = XMVectorMultiplyAdd(g_ErrorWeight1, vError, pDiffusionErrors[ index+2+delta ]); \
                    vError = XMVectorMultiply(vError, g_ErrorWeight7); \
                } \
                else \
                { \
                    /* Applied ordered dither */ \
                    target = XMVectorAdd(v, ordered[ index & 3 ]); \
                    target = XMVectorRound(target); \
                } \
                \
                target = XMVectorMin(scalev, target); \
                target = XMVectorMax((clampzero) ? g_XMZero : (XMVectorAdd(XMVectorNegate(scalev), g_XMOne)), target); \
                \
                auto dPtr = &dest[ index ]; \
                if (dPtr >= ePtr) break; \
                *dPtr = type(static_cast<type>((selectw) ? XMVectorGetW(target) : XMVectorGetX(target)) & mask); \
            } \
            return true; \
        } \
        return false;
}

#pragma warning(push)
#pragma warning( disable : 4127 )

_Use_decl_annotations_
bool DirectX::Internal::StoreScanlineDither(
    void* pDestination,
    size_t size,
    DXGI_FORMAT format,
    XMVECTOR* pSource,
    size_t count,
    float threshold,
    size_t y,
    size_t z,
    XMVECTOR* pDiffusionErrors) noexcept
{
    assert(pDestination != nullptr);
    assert(IsValid(format) && !IsTypeless(format) && !IsCompressed(format) && !IsPlanar(format) && !IsPalettized(format));

    if (!size || !count)
        return false;

    const XMVECTOR* __restrict sPtr = pSource;
    if (!sPtr)
        return false;

    assert((reinterpret_cast<uintptr_t>(pSource) & 0xF) == 0);

    XMVECTOR ordered[4];
    if (pDiffusionErrors)
    {
        // If pDiffusionErrors != 0, then this function performs error diffusion dithering (aka Floyd-Steinberg dithering)

        // To avoid the need for another temporary scanline buffer, we allow this function to overwrite the source buffer in-place
        // Given the intended usage in the conversion routines, this is not a problem.

        XMVECTOR* ptr = pSource;
        const XMVECTOR* err = pDiffusionErrors + 1;
        for (size_t i = 0; i < count; ++i)
        {
            // Add contribution from previous scanline
            XMVECTOR v = XMVectorAdd(*ptr, *err++);
            *ptr++ = v;
        }

        // Reset errors for next scanline
        memset(pDiffusionErrors, 0, sizeof(XMVECTOR)*(count + 2));
    }
    else
    {
        // If pDiffusionErrors == 0, then this function performs ordered dithering

        const XMVECTOR dither = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(g_Dither + (z & 3) + ((y & 3) * 8)));

        ordered[0] = XMVectorSplatX(dither);
        ordered[1] = XMVectorSplatY(dither);
        ordered[2] = XMVectorSplatZ(dither);
        ordered[3] = XMVectorSplatW(dither);
    }

    const void* ePtr = static_cast<const uint8_t*>(pDestination) + size;

#ifdef _PREFAST_
    *reinterpret_cast<uint8_t*>(pDestination) = 0;
#endif

    XMVECTOR vError = XMVectorZero();

    switch (static_cast<int>(format))
    {
    case DXGI_FORMAT_R16G16B16A16_UNORM:
        STORE_SCANLINE(XMUSHORTN4, g_Scale16pc, true, true, uint16_t, 0xFFFF, y, false)

    case DXGI_FORMAT_R16G16B16A16_UINT:
        STORE_SCANLINE(XMUSHORT4, g_Scale16pc, true, false, uint16_t, 0xFFFF, y, false)

    case DXGI_FORMAT_R16G16B16A16_SNORM:
        STORE_SCANLINE(XMSHORTN4, g_Scale15pc, false, true, int16_t, 0xFFFF, y, false)

    case DXGI_FORMAT_R16G16B16A16_SINT:
        STORE_SCANLINE(XMSHORT4, g_Scale15pc, false, false, int16_t, 0xFFFF, y, false)

    case DXGI_FORMAT_R10G10B10A2_UNORM:
        STORE_SCANLINE(XMUDECN4, g_Scale10pc, true, true, uint16_t, 0x3FF, y, false)

    case DXGI_FORMAT_R10G10B10A2_UINT:
        STORE_SCANLINE(XMUDEC4, g_Scale10pc, true, false, uint16_t, 0x3FF, y, false)

    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        if (size >= sizeof(XMUDEC4))
        {
            static const XMVECTORF32  Scale = { { { 510.0f, 510.0f, 510.0f, 3.0f } } };
            static const XMVECTORF32  Bias = { { { 384.0f, 384.0f, 384.0f, 0.0f } } };
            static const XMVECTORF32  MinXR = { { { -0.7529f, -0.7529f, -0.7529f, 0.f } } };
            static const XMVECTORF32  MaxXR = { { { 1.2529f, 1.2529f, 1.2529f, 1.0f } } };

            XMUDEC4 * __restrict dest = static_cast<XMUDEC4*>(pDestination);
            for (size_t i = 0; i < count; ++i)
            {
                auto index = static_cast<ptrdiff_t>((y & 1) ? (count - i - 1) : i);
                ptrdiff_t delta = (y & 1) ? -2 : 0;

                XMVECTOR v = XMVectorClamp(sPtr[index], MinXR, MaxXR);
                v = XMVectorMultiplyAdd(v, Scale, vError);

                XMVECTOR target;
                if (pDiffusionErrors)
                {
                    target = XMVectorRound(v);
                    vError = XMVectorSubtract(v, target);
                    vError = XMVectorDivide(vError, Scale);

                    // Distribute error to next scanline and next pixel
                    pDiffusionErrors[index - delta] = XMVectorMultiplyAdd(g_ErrorWeight3, vError, pDiffusionErrors[index - delta]);
                    pDiffusionErrors[index + 1] = XMVectorMultiplyAdd(g_ErrorWeight5, vError, pDiffusionErrors[index + 1]);
                    pDiffusionErrors[index + 2 + delta] = XMVectorMultiplyAdd(g_ErrorWeight1, vError, pDiffusionErrors[index + 2 + delta]);
                    vError = XMVectorMultiply(vError, g_ErrorWeight7);
                }
                else
                {
                    // Applied ordered dither
                    target = XMVectorAdd(v, ordered[index & 3]);
                    target = XMVectorRound(target);
                }

                target = XMVectorAdd(target, Bias);
                target = XMVectorClamp(target, g_XMZero, g_Scale10pc);

                XMFLOAT4A tmp;
                XMStoreFloat4A(&tmp, target);

                auto dPtr = &dest[index];
                if (dPtr >= ePtr) break;
                dPtr->x = uint16_t(static_cast<uint16_t>(tmp.x) & 0x3FF);
                dPtr->y = uint16_t(static_cast<uint16_t>(tmp.y) & 0x3FF);
                dPtr->z = uint16_t(static_cast<uint16_t>(tmp.z) & 0x3FF);
                dPtr->w = static_cast<uint16_t>(tmp.w);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        STORE_SCANLINE(XMUBYTEN4, g_Scale8pc, true, true, uint8_t, 0xFF, y, false)

    case DXGI_FORMAT_R8G8B8A8_UINT:
        STORE_SCANLINE(XMUBYTE4, g_Scale8pc, true, false, uint8_t, 0xFF, y, false)

    case DXGI_FORMAT_R8G8B8A8_SNORM:
        STORE_SCANLINE(XMBYTEN4, g_Scale7pc, false, true, int8_t, 0xFF, y, false)

    case DXGI_FORMAT_R8G8B8A8_SINT:
        STORE_SCANLINE(XMBYTE4, g_Scale7pc, false, false, int8_t, 0xFF, y, false)

    case DXGI_FORMAT_R16G16_UNORM:
        STORE_SCANLINE2(XMUSHORTN2, g_Scale16pc, true, true, uint16_t, 0xFFFF, y)

    case DXGI_FORMAT_R16G16_UINT:
        STORE_SCANLINE2(XMUSHORT2, g_Scale16pc, true, false, uint16_t, 0xFFFF, y)

    case DXGI_FORMAT_R16G16_SNORM:
        STORE_SCANLINE2(XMSHORTN2, g_Scale15pc, false, true, int16_t, 0xFFFF, y)

    case DXGI_FORMAT_R16G16_SINT:
        STORE_SCANLINE2(XMSHORT2, g_Scale15pc, false, false, int16_t, 0xFFFF, y)

    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        if (size >= sizeof(uint32_t))
        {
            static const XMVECTORF32 Clamp = { { { 1.f,  255.f, 0.f, 0.f } } };
            static const XMVECTORF32 Scale = { { { 16777215.f,   1.f, 0.f, 0.f } } };
            static const XMVECTORF32 Scale2 = { { { 16777215.f, 255.f, 0.f, 0.f } } };

            uint32_t * __restrict dest = static_cast<uint32_t*>(pDestination);
            for (size_t i = 0; i < count; ++i)
            {
                auto index = static_cast<ptrdiff_t>((y & 1) ? (count - i - 1) : i);
                ptrdiff_t delta = (y & 1) ? -2 : 0;

                XMVECTOR v = XMVectorClamp(sPtr[index], g_XMZero, Clamp);
                v = XMVectorAdd(v, vError);
                v = XMVectorMultiply(v, Scale);

                XMVECTOR target;
                if (pDiffusionErrors)
                {
                    target = XMVectorRound(v);
                    vError = XMVectorSubtract(v, target);
                    vError = XMVectorDivide(vError, Scale);

                    // Distribute error to next scanline and next pixel
                    pDiffusionErrors[index - delta] = XMVectorMultiplyAdd(g_ErrorWeight3, vError, pDiffusionErrors[index - delta]);
                    pDiffusionErrors[index + 1] = XMVectorMultiplyAdd(g_ErrorWeight5, vError, pDiffusionErrors[index + 1]);
                    pDiffusionErrors[index + 2 + delta] = XMVectorMultiplyAdd(g_ErrorWeight1, vError, pDiffusionErrors[index + 2 + delta]);
                    vError = XMVectorMultiply(vError, g_ErrorWeight7);
                }
                else
                {
                    // Applied ordered dither
                    target = XMVectorAdd(v, ordered[index & 3]);
                    target = XMVectorRound(target);
                }

                target = XMVectorClamp(target, g_XMZero, Scale2);

                XMFLOAT4A tmp;
                XMStoreFloat4A(&tmp, target);

                auto dPtr = &dest[index];
                if (dPtr >= ePtr) break;
                *dPtr = (static_cast<uint32_t>(tmp.x) & 0xFFFFFF)
                    | ((static_cast<uint32_t>(tmp.y) & 0xFF) << 24);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_R8G8_UNORM:
        STORE_SCANLINE2(XMUBYTEN2, g_Scale8pc, true, true, uint8_t, 0xFF, y)

    case DXGI_FORMAT_R8G8_UINT:
        STORE_SCANLINE2(XMUBYTE2, g_Scale8pc, true, false, uint8_t, 0xFF, y)

    case DXGI_FORMAT_R8G8_SNORM:
        STORE_SCANLINE2(XMBYTEN2, g_Scale7pc, false, true, int8_t, 0xFF, y)

    case DXGI_FORMAT_R8G8_SINT:
        STORE_SCANLINE2(XMBYTE2, g_Scale7pc, false, false, int8_t, 0xFF, y)

    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
        STORE_SCANLINE1(uint16_t, g_Scale16pc, true, true, 0xFFFF, y, false)

    case DXGI_FORMAT_R16_UINT:
        STORE_SCANLINE1(uint16_t, g_Scale16pc, true, false, 0xFFFF, y, false)

    case DXGI_FORMAT_R16_SNORM:
        STORE_SCANLINE1(int16_t, g_Scale15pc, false, true, 0xFFFF, y, false)

    case DXGI_FORMAT_R16_SINT:
        STORE_SCANLINE1(int16_t, g_Scale15pc, false, false, 0xFFFF, y, false)

    case DXGI_FORMAT_R8_UNORM:
        STORE_SCANLINE1(uint8_t, g_Scale8pc, true, true, 0xFF, y, false)

    case DXGI_FORMAT_R8_UINT:
        STORE_SCANLINE1(uint8_t, g_Scale8pc, true, false, 0xFF, y, false)

    case DXGI_FORMAT_R8_SNORM:
        STORE_SCANLINE1(int8_t, g_Scale7pc, false, true, 0xFF, y, false)

    case DXGI_FORMAT_R8_SINT:
        STORE_SCANLINE1(int8_t, g_Scale7pc, false, false, 0xFF, y, false)

    case DXGI_FORMAT_A8_UNORM:
        STORE_SCANLINE1(uint8_t, g_Scale8pc, true, true, 0xFF, y, true)

    case DXGI_FORMAT_B5G6R5_UNORM:
        if (size >= sizeof(XMU565))
        {
            XMU565 * __restrict dest = static_cast<XMU565*>(pDestination);
            for (size_t i = 0; i < count; ++i)
            {
                auto index = static_cast<ptrdiff_t>((y & 1) ? (count - i - 1) : i);
                ptrdiff_t delta = (y & 1) ? -2 : 0;

                XMVECTOR v = XMVectorSwizzle<2, 1, 0, 3>(sPtr[index]);
                v = XMVectorSaturate(v);
                v = XMVectorAdd(v, vError);
                v = XMVectorMultiply(v, g_Scale565pc);

                XMVECTOR target;
                if (pDiffusionErrors)
                {
                    target = XMVectorRound(v);
                    vError = XMVectorSubtract(v, target);
                    vError = XMVectorDivide(vError, g_Scale565pc);

                    // Distribute error to next scanline and next pixel
                    pDiffusionErrors[index - delta] = XMVectorMultiplyAdd(g_ErrorWeight3, vError, pDiffusionErrors[index - delta]);
                    pDiffusionErrors[index + 1] = XMVectorMultiplyAdd(g_ErrorWeight5, vError, pDiffusionErrors[index + 1]);
                    pDiffusionErrors[index + 2 + delta] = XMVectorMultiplyAdd(g_ErrorWeight1, vError, pDiffusionErrors[index + 2 + delta]);
                    vError = XMVectorMultiply(vError, g_ErrorWeight7);
                }
                else
                {
                    // Applied ordered dither
                    target = XMVectorAdd(v, ordered[index & 3]);
                    target = XMVectorRound(target);
                }

                target = XMVectorClamp(target, g_XMZero, g_Scale565pc);

                XMFLOAT4A tmp;
                XMStoreFloat4A(&tmp, target);

                auto dPtr = &dest[index];
                if (dPtr >= ePtr) break;
                dPtr->x = uint16_t(static_cast<uint16_t>(tmp.x) & 0x1F);
                dPtr->y = uint16_t(static_cast<uint16_t>(tmp.y) & 0x3F);
                dPtr->z = uint16_t(static_cast<uint16_t>(tmp.z) & 0x1F);
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_B5G5R5A1_UNORM:
        if (size >= sizeof(XMU555))
        {
            XMU555 * __restrict dest = static_cast<XMU555*>(pDestination);
            for (size_t i = 0; i < count; ++i)
            {
                auto index = static_cast<ptrdiff_t>((y & 1) ? (count - i - 1) : i);
                ptrdiff_t delta = (y & 1) ? -2 : 0;

                XMVECTOR v = XMVectorSwizzle<2, 1, 0, 3>(sPtr[index]);
                v = XMVectorSaturate(v);
                v = XMVectorAdd(v, vError);
                v = XMVectorMultiply(v, g_Scale5551pc);

                XMVECTOR target;
                if (pDiffusionErrors)
                {
                    target = XMVectorRound(v);
                    vError = XMVectorSubtract(v, target);
                    vError = XMVectorDivide(vError, g_Scale5551pc);

                    // Distribute error to next scanline and next pixel
                    pDiffusionErrors[index - delta] = XMVectorMultiplyAdd(g_ErrorWeight3, vError, pDiffusionErrors[index - delta]);
                    pDiffusionErrors[index + 1] = XMVectorMultiplyAdd(g_ErrorWeight5, vError, pDiffusionErrors[index + 1]);
                    pDiffusionErrors[index + 2 + delta] = XMVectorMultiplyAdd(g_ErrorWeight1, vError, pDiffusionErrors[index + 2 + delta]);
                    vError = XMVectorMultiply(vError, g_ErrorWeight7);
                }
                else
                {
                    // Applied ordered dither
                    target = XMVectorAdd(v, ordered[index & 3]);
                    target = XMVectorRound(target);
                }

                target = XMVectorClamp(target, g_XMZero, g_Scale5551pc);

                XMFLOAT4A tmp;
                XMStoreFloat4A(&tmp, target);

                auto dPtr = &dest[index];
                if (dPtr >= ePtr) break;
                dPtr->x = uint16_t(static_cast<uint16_t>(tmp.x) & 0x1F);
                dPtr->y = uint16_t(static_cast<uint16_t>(tmp.y) & 0x1F);
                dPtr->z = uint16_t(static_cast<uint16_t>(tmp.z) & 0x1F);
                dPtr->w = (XMVectorGetW(target) > threshold) ? 1u : 0u;
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        STORE_SCANLINE(XMUBYTEN4, g_Scale8pc, true, true, uint8_t, 0xFF, y, true)

    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        if (size >= sizeof(XMUBYTEN4))
        {
            XMUBYTEN4 * __restrict dest = static_cast<XMUBYTEN4*>(pDestination);
            for (size_t i = 0; i < count; ++i)
            {
                auto index = static_cast<ptrdiff_t>((y & 1) ? (count - i - 1) : i);
                ptrdiff_t delta = (y & 1) ? -2 : 0;

                XMVECTOR v = XMVectorSwizzle<2, 1, 0, 3>(sPtr[index]);
                v = XMVectorSaturate(v);
                v = XMVectorAdd(v, vError);
                v = XMVectorMultiply(v, g_Scale8pc);

                XMVECTOR target;
                if (pDiffusionErrors)
                {
                    target = XMVectorRound(v);
                    vError = XMVectorSubtract(v, target);
                    vError = XMVectorDivide(vError, g_Scale8pc);

                    // Distribute error to next scanline and next pixel
                    pDiffusionErrors[index - delta] = XMVectorMultiplyAdd(g_ErrorWeight3, vError, pDiffusionErrors[index - delta]);
                    pDiffusionErrors[index + 1] = XMVectorMultiplyAdd(g_ErrorWeight5, vError, pDiffusionErrors[index + 1]);
                    pDiffusionErrors[index + 2 + delta] = XMVectorMultiplyAdd(g_ErrorWeight1, vError, pDiffusionErrors[index + 2 + delta]);
                    vError = XMVectorMultiply(vError, g_ErrorWeight7);
                }
                else
                {
                    // Applied ordered dither
                    target = XMVectorAdd(v, ordered[index & 3]);
                    target = XMVectorRound(target);
                }

                target = XMVectorClamp(target, g_XMZero, g_Scale8pc);

                XMFLOAT4A tmp;
                XMStoreFloat4A(&tmp, target);

                auto dPtr = &dest[index];
                if (dPtr >= ePtr) break;
                dPtr->x = uint8_t(static_cast<uint8_t>(tmp.x) & 0xFF);
                dPtr->y = uint8_t(static_cast<uint8_t>(tmp.y) & 0xFF);
                dPtr->z = uint8_t(static_cast<uint8_t>(tmp.z) & 0xFF);
                dPtr->w = 0;
            }
            return true;
        }
        return false;

    case DXGI_FORMAT_B4G4R4A4_UNORM:
        STORE_SCANLINE(XMUNIBBLE4, g_Scale4pc, true, true, uint8_t, 0xF, y, true)

    case WIN11_DXGI_FORMAT_A4B4G4R4_UNORM:
        if (size >= sizeof(XMUNIBBLE4))
        {
            XMUNIBBLE4 * __restrict dest = static_cast<XMUNIBBLE4*>(pDestination);
            for (size_t i = 0; i < count; ++i)
            {
                auto index = static_cast<ptrdiff_t>((y & 1) ? (count - i - 1) : i);
                ptrdiff_t delta = (y & 1) ? -2 : 0;

                XMVECTOR v = XMVectorSaturate(sPtr[index]);
                v = XMVectorSwizzle<3, 2, 1, 0>(v);
                v = XMVectorAdd(v, vError);
                v = XMVectorMultiply(v, g_Scale4pc);

                XMVECTOR target;
                if (pDiffusionErrors)
                {
                    target = XMVectorRound(v);
                    vError = XMVectorSubtract(v, target);
                    vError = XMVectorDivide(vError, g_Scale4pc);

                    // Distribute error to next scanline and next pixel
                    pDiffusionErrors[index - delta] = XMVectorMultiplyAdd(g_ErrorWeight3, vError, pDiffusionErrors[index - delta]);
                    pDiffusionErrors[index + 1] = XMVectorMultiplyAdd(g_ErrorWeight5, vError, pDiffusionErrors[index + 1]);
                    pDiffusionErrors[index + 2 + delta] = XMVectorMultiplyAdd(g_ErrorWeight1, vError, pDiffusionErrors[index + 2 + delta]);
                    vError = XMVectorMultiply(vError, g_ErrorWeight7);
                }
                else
                {
                    // Applied ordered dither
                    target = XMVectorAdd(v, ordered[index & 3]);
                    target = XMVectorRound(target);
                }

                target = XMVectorClamp(target, g_XMZero, g_Scale4pc);

                XMFLOAT4A tmp;
                XMStoreFloat4A(&tmp, target);

                auto dPtr = &dest[index];
                if (dPtr >= ePtr) break;
                dPtr->x = uint8_t(static_cast<uint8_t>(tmp.x) & 0xF);
                dPtr->y = uint8_t(static_cast<uint8_t>(tmp.y) & 0xF);
                dPtr->z = uint8_t(static_cast<uint8_t>(tmp.z) & 0xF);
                dPtr->w = uint8_t(static_cast<uint8_t>(tmp.w) & 0xF);
            }
            return true;
        }
        return false;

    case XBOX_DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM:
        STORE_SCANLINE(XMXDECN4, g_Scale9pc, false, true, uint16_t, 0x3FF, y, false)

    case XBOX_DXGI_FORMAT_R4G4_UNORM:
        if (size >= sizeof(uint8_t))
        {
            uint8_t * __restrict dest = static_cast<uint8_t*>(pDestination);
            for (size_t i = 0; i < count; ++i)
            {
                auto index = static_cast<ptrdiff_t>((y & 1) ? (count - i - 1) : i);
                ptrdiff_t delta = (y & 1) ? -2 : 0;

                XMVECTOR v = XMVectorSaturate(sPtr[index]);
                v = XMVectorAdd(v, vError);
                v = XMVectorMultiply(v, g_Scale4pc);

                XMVECTOR target;
                if (pDiffusionErrors)
                {
                    target = XMVectorRound(v);
                    vError = XMVectorSubtract(v, target);
                    vError = XMVectorDivide(vError, g_Scale4pc);

                    // Distribute error to next scanline and next pixel
                    pDiffusionErrors[index - delta] = XMVectorMultiplyAdd(g_ErrorWeight3, vError, pDiffusionErrors[index - delta]);
                    pDiffusionErrors[index + 1] = XMVectorMultiplyAdd(g_ErrorWeight5, vError, pDiffusionErrors[index + 1]);
                    pDiffusionErrors[index + 2 + delta] = XMVectorMultiplyAdd(g_ErrorWeight1, vError, pDiffusionErrors[index + 2 + delta]);
                    vError = XMVectorMultiply(vError, g_ErrorWeight7);
                }
                else
                {
                    // Applied ordered dither
                    target = XMVectorAdd(v, ordered[index & 3]);
                    target = XMVectorRound(target);
                }

                target = XMVectorClamp(target, g_XMZero, g_Scale4pc);

                XMFLOAT4A tmp;
                XMStoreFloat4A(&tmp, target);

                auto dPtr = &dest[index];
                if (dPtr >= ePtr) break;
                *dPtr = static_cast<uint8_t>((unsigned(tmp.x) & 0xF) | ((unsigned(tmp.y) & 0xF) << 4));
            }
            return true;
        }
        return false;

    default:
        return StoreScanline(pDestination, size, format, pSource, count, threshold);
    }
}

#pragma warning(pop)

#undef STORE_SCANLINE
#undef STORE_SCANLINE2
#undef STORE_SCANLINE1

namespace
{
    //-------------------------------------------------------------------------------------
    // Selection logic for using WIC vs. our own routines
    //-------------------------------------------------------------------------------------
    inline bool UseWICConversion(
        _In_ TEX_FILTER_FLAGS filter,
        _In_ DXGI_FORMAT sformat,
        _In_ DXGI_FORMAT tformat,
        _Out_ WICPixelFormatGUID& pfGUID,
        _Out_ WICPixelFormatGUID& targetGUID) noexcept
    {
    #ifndef _WIN32
        UNREFERENCED_PARAMETER(filter);
        UNREFERENCED_PARAMETER(sformat);
        UNREFERENCED_PARAMETER(tformat);
        UNREFERENCED_PARAMETER(pfGUID);
        UNREFERENCED_PARAMETER(targetGUID);
        return false;
    #else
        memset(&pfGUID, 0, sizeof(GUID));
        memset(&targetGUID, 0, sizeof(GUID));

        if (filter & TEX_FILTER_FORCE_NON_WIC)
        {
            // Explicit flag indicates use of non-WIC code paths
            return false;
        }

        if (!DXGIToWIC(sformat, pfGUID) || !DXGIToWIC(tformat, targetGUID))
        {
            // Source or target format are not WIC supported native pixel formats
            return false;
        }

        if (filter & TEX_FILTER_FORCE_WIC)
        {
            // Explicit flag to use WIC code paths, skips all the case checks below
            return true;
        }

        if (filter & TEX_FILTER_SEPARATE_ALPHA)
        {
            // Alpha is not premultiplied, so use non-WIC code paths
            return false;
        }

        if (filter & TEX_FILTER_FLOAT_X2BIAS)
        {
            // X2 Scale & Bias conversions not supported by WIC code paths
            return false;
        }

        // Check for special cases
    #if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
        if (sformat == DXGI_FORMAT_R16G16B16A16_FLOAT
            || sformat == DXGI_FORMAT_R16_FLOAT
            || tformat == DXGI_FORMAT_R16G16B16A16_FLOAT
            || tformat == DXGI_FORMAT_R16_FLOAT)
        {
            // Use non-WIC code paths as these conversions are not supported by Xbox version of WIC
            return false;
        }
    #endif

        switch (sformat)
        {
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32_FLOAT:
        case DXGI_FORMAT_R32G32_FLOAT:
        case DXGI_FORMAT_R32_FLOAT:
        case DXGI_FORMAT_D32_FLOAT:
            switch (tformat)
            {
            case DXGI_FORMAT_R16G16B16A16_FLOAT:
            case DXGI_FORMAT_R16G16_FLOAT:
            case DXGI_FORMAT_R16_FLOAT:
                // WIC conversions for FP32->FP16 can result in NaN values instead of clamping for min/max value
                return false;

            default:
                break;
            }
            break;

        default:
            break;
        }

        switch (sformat)
        {
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32_FLOAT:
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
            switch (tformat)
            {
            case DXGI_FORMAT_R16_FLOAT:
            case DXGI_FORMAT_R32_FLOAT:
            case DXGI_FORMAT_D32_FLOAT:
                // WIC converts via UNORM formats and ends up converting colorspaces for these cases
            case DXGI_FORMAT_A8_UNORM:
                // Conversion logic for these kinds of textures is unintuitive for WIC code paths
                return false;

            default:
                break;
            }
            break;

        case DXGI_FORMAT_R16_FLOAT:
            switch (tformat)
            {
            case DXGI_FORMAT_R32_FLOAT:
            case DXGI_FORMAT_D32_FLOAT:
                // WIC converts via UNORM formats and ends up converting colorspaces for these cases
            case DXGI_FORMAT_A8_UNORM:
                // Conversion logic for these kinds of textures is unintuitive for WIC code paths
                return false;

            default:
                break;
            }
            break;

        case DXGI_FORMAT_A8_UNORM:
            // Conversion logic for these kinds of textures is unintuitive for WIC code paths
            return false;

        default:
            switch (tformat)
            {
            case DXGI_FORMAT_A8_UNORM:
                // Conversion logic for these kinds of textures is unintuitive for WIC code paths
                return false;

            default:
                break;
            }
        }

        // Check for implicit color space changes
        if (IsSRGB(sformat))
            filter |= TEX_FILTER_SRGB_IN;

        if (IsSRGB(tformat))
            filter |= TEX_FILTER_SRGB_OUT;

        if ((filter & (TEX_FILTER_SRGB_IN | TEX_FILTER_SRGB_OUT)) == (TEX_FILTER_SRGB_IN | TEX_FILTER_SRGB_OUT))
        {
            filter &= ~(TEX_FILTER_SRGB_IN | TEX_FILTER_SRGB_OUT);
        }

        const auto wicsrgb = CheckWICColorSpace(pfGUID, targetGUID);

        if (wicsrgb != (filter & (TEX_FILTER_SRGB_IN | TEX_FILTER_SRGB_OUT)))
        {
            // WIC will perform a colorspace conversion we didn't request
            return false;
        }

        return true;
    #endif // WIN32
    }

    //-------------------------------------------------------------------------------------
    // Convert the source image using WIC
    //-------------------------------------------------------------------------------------
    HRESULT ConvertUsingWIC(
        _In_ const Image& srcImage,
        _In_ const WICPixelFormatGUID& pfGUID,
        _In_ const WICPixelFormatGUID& targetGUID,
        _In_ TEX_FILTER_FLAGS filter,
        _In_ float threshold,
        _In_ const Image& destImage)
    {
    #ifndef _WIN32
        UNREFERENCED_PARAMETER(srcImage);
        UNREFERENCED_PARAMETER(pfGUID);
        UNREFERENCED_PARAMETER(targetGUID);
        UNREFERENCED_PARAMETER(filter);
        UNREFERENCED_PARAMETER(threshold);
        UNREFERENCED_PARAMETER(destImage);
        return E_NOTIMPL;
    #else
        assert(srcImage.width == destImage.width);
        assert(srcImage.height == destImage.height);

        bool iswic2 = false;
        auto pWIC = GetWICFactory(iswic2);
        if (!pWIC)
            return E_NOINTERFACE;

        ComPtr<IWICFormatConverter> FC;
        HRESULT hr = pWIC->CreateFormatConverter(FC.GetAddressOf());
        if (FAILED(hr))
            return hr;

        // Note that WIC conversion ignores the TEX_FILTER_SRGB_IN and TEX_FILTER_SRGB_OUT flags,
        // but also always assumes UNORM <-> FLOAT conversions are changing color spaces sRGB <-> scRGB

        BOOL canConvert = FALSE;
        hr = FC->CanConvert(pfGUID, targetGUID, &canConvert);
        if (FAILED(hr) || !canConvert)
        {
            // This case is not an issue for the subset of WIC formats that map directly to DXGI
            return E_UNEXPECTED;
        }

        if (srcImage.rowPitch > UINT32_MAX || srcImage.slicePitch > UINT32_MAX
            || destImage.rowPitch > UINT32_MAX || destImage.slicePitch > UINT32_MAX)
            return HRESULT_E_ARITHMETIC_OVERFLOW;

        ComPtr<IWICBitmap> source;
        hr = pWIC->CreateBitmapFromMemory(static_cast<UINT>(srcImage.width), static_cast<UINT>(srcImage.height), pfGUID,
            static_cast<UINT>(srcImage.rowPitch), static_cast<UINT>(srcImage.slicePitch),
            srcImage.pixels, source.GetAddressOf());
        if (FAILED(hr))
            return hr;

        hr = FC->Initialize(source.Get(), targetGUID, GetWICDither(filter), nullptr,
            static_cast<double>(threshold) * 100.0, WICBitmapPaletteTypeMedianCut);
        if (FAILED(hr))
            return hr;

        hr = FC->CopyPixels(nullptr, static_cast<UINT>(destImage.rowPitch), static_cast<UINT>(destImage.slicePitch), destImage.pixels);
        if (FAILED(hr))
            return hr;

        return S_OK;
    #endif // WIN32
    }

    //-------------------------------------------------------------------------------------
    // Convert the source image (not using WIC)
    //-------------------------------------------------------------------------------------
    HRESULT ConvertCustom(
        _In_ const Image& srcImage,
        _In_ TEX_FILTER_FLAGS filter,
        _In_ const Image& destImage,
        _In_ float threshold,
        size_t z,
        const std::function<bool __cdecl(size_t, size_t)>& statusCallback) noexcept
    {
        assert(srcImage.width == destImage.width);
        assert(srcImage.height == destImage.height);

        const uint8_t *pSrc = srcImage.pixels;
        uint8_t *pDest = destImage.pixels;
        if (!pSrc || !pDest)
            return E_POINTER;

        size_t width = srcImage.width;

        if (filter & TEX_FILTER_DITHER_DIFFUSION)
        {
            // Error diffusion dithering (aka Floyd-Steinberg dithering)
            auto scanline = make_AlignedArrayXMVECTOR(uint64_t(width) * 2 + 2);
            if (!scanline)
                return E_OUTOFMEMORY;

            XMVECTOR* pDiffusionErrors = scanline.get() + width;
            memset(pDiffusionErrors, 0, sizeof(XMVECTOR)*(width + 2));

            for (size_t h = 0; h < srcImage.height; ++h)
            {
                if (statusCallback)
                {
                    if (!statusCallback(h, srcImage.height))
                    {
                        return E_ABORT;
                    }
                }

                if (!LoadScanline(scanline.get(), width, pSrc, srcImage.rowPitch, srcImage.format))
                    return E_FAIL;

                ConvertScanline(scanline.get(), width, destImage.format, srcImage.format, filter);

                if (!StoreScanlineDither(pDest, destImage.rowPitch, destImage.format, scanline.get(), width, threshold, h, z, pDiffusionErrors))
                    return E_FAIL;

                pSrc += srcImage.rowPitch;
                pDest += destImage.rowPitch;
            }
        }
        else
        {
            auto scanline = make_AlignedArrayXMVECTOR(width);
            if (!scanline)
                return E_OUTOFMEMORY;

            if (filter & TEX_FILTER_DITHER)
            {
                // Ordered dithering
                for (size_t h = 0; h < srcImage.height; ++h)
                {
                    if (statusCallback)
                    {
                        if (!statusCallback(h, srcImage.height))
                        {
                            return E_ABORT;
                        }
                    }

                    if (!LoadScanline(scanline.get(), width, pSrc, srcImage.rowPitch, srcImage.format))
                        return E_FAIL;

                    ConvertScanline(scanline.get(), width, destImage.format, srcImage.format, filter);

                    if (!StoreScanlineDither(pDest, destImage.rowPitch, destImage.format, scanline.get(), width, threshold, h, z, nullptr))
                        return E_FAIL;

                    pSrc += srcImage.rowPitch;
                    pDest += destImage.rowPitch;
                }
            }
            else
            {
                // No dithering
                for (size_t h = 0; h < srcImage.height; ++h)
                {
                    if (statusCallback)
                    {
                        if (!statusCallback(h, srcImage.height))
                        {
                            return E_ABORT;
                        }
                    }

                    if (!LoadScanline(scanline.get(), width, pSrc, srcImage.rowPitch, srcImage.format))
                        return E_FAIL;

                    ConvertScanline(scanline.get(), width, destImage.format, srcImage.format, filter);

                    if (!StoreScanline(pDest, destImage.rowPitch, destImage.format, scanline.get(), width, threshold))
                        return E_FAIL;

                    pSrc += srcImage.rowPitch;
                    pDest += destImage.rowPitch;
                }
            }
        }

        return S_OK;
    }

    //-------------------------------------------------------------------------------------
    DXGI_FORMAT PlanarToSingle(_In_ DXGI_FORMAT format) noexcept
    {
        switch (format)
        {
        case DXGI_FORMAT_NV12:
        case DXGI_FORMAT_NV11:
            return DXGI_FORMAT_YUY2;

        case DXGI_FORMAT_P010:
            return DXGI_FORMAT_Y210;

        case DXGI_FORMAT_P016:
            return DXGI_FORMAT_Y216;

            // We currently do not support conversion for Xbox One specific 16-bit depth formats

            // We can't do anything with DXGI_FORMAT_420_OPAQUE because it's an opaque blob of bits

            // We don't support conversion of JPEG Hardware decode formats

        default:
            return DXGI_FORMAT_UNKNOWN;
        }
    }

    //-------------------------------------------------------------------------------------
    // Convert the image from a planar to non-planar image
    //-------------------------------------------------------------------------------------
#define CONVERT_420_TO_422( srcType, destType )\
        {\
            const size_t rowPitch = srcImage.rowPitch;\
            \
            const auto sourceE = reinterpret_cast<const srcType*>(pSrc + srcImage.slicePitch);\
            auto pSrcUV = pSrc + (srcImage.height * rowPitch);\
            \
            for(size_t y = 0; y < srcImage.height; y+= 2)\
            {\
                auto sPtrY0 = reinterpret_cast<const srcType*>(pSrc);\
                auto sPtrY2 = reinterpret_cast<const srcType*>(pSrc + rowPitch);\
                auto sPtrUV = reinterpret_cast<const srcType*>(pSrcUV);\
                \
                destType * __restrict dPtr0 = reinterpret_cast<destType*>(pDest);\
                destType * __restrict dPtr1 = reinterpret_cast<destType*>(pDest + destImage.rowPitch);\
                \
                for(size_t x = 0; x < srcImage.width; x+= 2)\
                {\
                    if ((sPtrUV+1) >= sourceE) break;\
                    \
                    const srcType u = *(sPtrUV++);\
                    const srcType v = *(sPtrUV++);\
                    \
                    dPtr0->x = *(sPtrY0++);\
                    dPtr0->y = u;\
                    dPtr0->z = *(sPtrY0++);\
                    dPtr0->w = v;\
                    ++dPtr0;\
                    \
                    dPtr1->x = *(sPtrY2++);\
                    dPtr1->y = u;\
                    dPtr1->z = *(sPtrY2++);\
                    dPtr1->w = v;\
                    ++dPtr1;\
                }\
                \
                pSrc += rowPitch * 2;\
                pSrcUV += rowPitch;\
                \
                pDest += destImage.rowPitch * 2;\
            }\
        }

#ifdef __clang__
#pragma clang diagnostic ignored "-Wextra-semi-stmt"
#endif

    HRESULT ConvertToSinglePlane_(_In_ const Image& srcImage, _In_ const Image& destImage) noexcept
    {
        assert(srcImage.width == destImage.width);
        assert(srcImage.height == destImage.height);

        const uint8_t *pSrc = srcImage.pixels;
        uint8_t *pDest = destImage.pixels;
        if (!pSrc || !pDest)
            return E_POINTER;

        switch (srcImage.format)
        {
        case DXGI_FORMAT_NV12:
            assert(destImage.format == DXGI_FORMAT_YUY2);
            if ((srcImage.width % 2) != 0 || (srcImage.height % 2) != 0)
                return E_INVALIDARG;

            CONVERT_420_TO_422(uint8_t, XMUBYTEN4);
            return S_OK;

        case DXGI_FORMAT_P010:
            assert(destImage.format == DXGI_FORMAT_Y210);
            if ((srcImage.width % 2) != 0 || (srcImage.height % 2) != 0)
                return E_INVALIDARG;

            CONVERT_420_TO_422(uint16_t, XMUSHORTN4);
            return S_OK;

        case DXGI_FORMAT_P016:
            assert(destImage.format == DXGI_FORMAT_Y216);
            if ((srcImage.width % 2) != 0 || (srcImage.height % 2) != 0)
                return E_INVALIDARG;

            CONVERT_420_TO_422(uint16_t, XMUSHORTN4);
            return S_OK;

        case DXGI_FORMAT_NV11:
            assert(destImage.format == DXGI_FORMAT_YUY2);
            if ((srcImage.width % 4) != 0)
                return E_INVALIDARG;

            // Convert 4:1:1 to 4:2:2
            {
                const size_t rowPitch = srcImage.rowPitch;

                const uint8_t* sourceE = pSrc + srcImage.slicePitch;
                const uint8_t* pSrcUV = pSrc + (srcImage.height * rowPitch);

                for (size_t y = 0; y < srcImage.height; ++y)
                {
                    const uint8_t* sPtrY = pSrc;
                    const uint8_t* sPtrUV = pSrcUV;

                    XMUBYTEN4 * __restrict dPtr = reinterpret_cast<XMUBYTEN4*>(pDest);

                    for (size_t x = 0; x < srcImage.width; x += 4)
                    {
                        if ((sPtrUV + 1) >= sourceE) break;

                        const uint8_t u = *(sPtrUV++);
                        const uint8_t v = *(sPtrUV++);

                        dPtr->x = *(sPtrY++);
                        dPtr->y = u;
                        dPtr->z = *(sPtrY++);
                        dPtr->w = v;
                        ++dPtr;

                        dPtr->x = *(sPtrY++);
                        dPtr->y = u;
                        dPtr->z = *(sPtrY++);
                        dPtr->w = v;
                        ++dPtr;
                    }

                    pSrc += rowPitch;
                    pSrcUV += (rowPitch >> 1);

                    pDest += destImage.rowPitch;
                }
            }
            return S_OK;

        default:
            return E_UNEXPECTED;
        }
    }

#undef CONVERT_420_TO_422
}


//=====================================================================================
// Entry-points
//=====================================================================================

//-------------------------------------------------------------------------------------
// Convert image
//-------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT DirectX::Convert(
    const Image& srcImage,
    DXGI_FORMAT format,
    TEX_FILTER_FLAGS filter,
    float threshold,
    ScratchImage& image) noexcept
{
    ConvertOptions options = {};
    options.filter = filter;
    options.threshold = threshold;

    return ConvertEx(srcImage, format, options, image, nullptr);
}

_Use_decl_annotations_
HRESULT DirectX::ConvertEx(
    const Image& srcImage,
    DXGI_FORMAT format,
    const ConvertOptions& options,
    ScratchImage& image,
    std::function<bool __cdecl(size_t, size_t)> statusCallback)
{
    if ((srcImage.format == format) || !IsValid(format))
        return E_INVALIDARG;

    if (!srcImage.pixels)
        return E_POINTER;

    if (IsCompressed(srcImage.format) || IsCompressed(format)
        || IsPlanar(srcImage.format) || IsPlanar(format)
        || IsPalettized(srcImage.format) || IsPalettized(format)
        || IsTypeless(srcImage.format) || IsTypeless(format))
        return HRESULT_E_NOT_SUPPORTED;

    if ((srcImage.width > UINT32_MAX) || (srcImage.height > UINT32_MAX))
        return E_INVALIDARG;

    HRESULT hr = image.Initialize2D(format, srcImage.width, srcImage.height, 1, 1);
    if (FAILED(hr))
        return hr;

    const Image *rimage = image.GetImage(0, 0, 0);
    if (!rimage)
    {
        image.Release();
        return E_POINTER;
    }

    if (statusCallback)
    {
        if (!statusCallback(0, rimage->height))
        {
            image.Release();
            return E_ABORT;
        }
    }

    WICPixelFormatGUID pfGUID, targetGUID;
    if (UseWICConversion(options.filter, srcImage.format, format, pfGUID, targetGUID))
    {
        hr = ConvertUsingWIC(srcImage, pfGUID, targetGUID, options.filter, options.threshold, *rimage);
    }
    else
    {
        hr = ConvertCustom(srcImage, options.filter, *rimage, options.threshold, 0, statusCallback);
    }

    if (FAILED(hr))
    {
        image.Release();
        return hr;
    }

    if (statusCallback)
    {
        if (!statusCallback(rimage->height, rimage->height))
        {
            image.Release();
            return E_ABORT;
        }
    }

    return S_OK;
}


//-------------------------------------------------------------------------------------
// Convert image (complex)
//-------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT DirectX::Convert(
    const Image* srcImages,
    size_t nimages,
    const TexMetadata& metadata,
    DXGI_FORMAT format,
    TEX_FILTER_FLAGS filter,
    float threshold,
    ScratchImage& result) noexcept
{
    ConvertOptions options = {};
    options.filter = filter;
    options.threshold = threshold;

    return ConvertEx(srcImages, nimages, metadata, format, options, result, nullptr);
}

_Use_decl_annotations_
HRESULT DirectX::ConvertEx(
    const Image* srcImages,
    size_t nimages,
    const TexMetadata& metadata,
    DXGI_FORMAT format,
    const ConvertOptions& options,
    ScratchImage& result,
    std::function<bool __cdecl(size_t, size_t)> statusCallback)
{
    if (!srcImages || !nimages || (metadata.format == format) || !IsValid(format))
        return E_INVALIDARG;

    if (IsCompressed(metadata.format) || IsCompressed(format)
        || IsPlanar(metadata.format) || IsPlanar(format)
        || IsPalettized(metadata.format) || IsPalettized(format)
        || IsTypeless(metadata.format) || IsTypeless(format))
        return HRESULT_E_NOT_SUPPORTED;

    if ((metadata.width > UINT32_MAX) || (metadata.height > UINT32_MAX))
        return E_INVALIDARG;

    if (statusCallback
        && nimages == 1
        && !metadata.IsVolumemap()
        && metadata.mipLevels == 1
        && metadata.arraySize == 1)
    {
        // If progress reporting is requested when converting a single 1D or 2D image, call
        // the ConvertEx overload that takes a single image.
        // This provides a better user experience as progress will be reported as the image
        // is being processed, instead of after processing has been completed.
        return ConvertEx(srcImages[0], format, options, result, statusCallback);
    }

    TexMetadata mdata2 = metadata;
    mdata2.format = format;
    HRESULT hr = result.Initialize(mdata2);
    if (FAILED(hr))
        return hr;

    if (nimages != result.GetImageCount())
    {
        result.Release();
        return E_FAIL;
    }

    const Image* dest = result.GetImages();
    if (!dest)
    {
        result.Release();
        return E_POINTER;
    }

    if (statusCallback)
    {
        if (!statusCallback(0, nimages))
        {
            result.Release();
            return E_ABORT;
        }
    }

    WICPixelFormatGUID pfGUID, targetGUID;
    const bool usewic = !metadata.IsPMAlpha() && UseWICConversion(options.filter, metadata.format, format, pfGUID, targetGUID);

    switch (metadata.dimension)
    {
    case TEX_DIMENSION_TEXTURE1D:
    case TEX_DIMENSION_TEXTURE2D:
        for (size_t index = 0; index < nimages; ++index)
        {
            const Image& src = srcImages[index];
            if (src.format != metadata.format)
            {
                result.Release();
                return E_FAIL;
            }

            if ((src.width > UINT32_MAX) || (src.height > UINT32_MAX))
            {
                result.Release();
                return E_FAIL;
            }

            const Image& dst = dest[index];
            assert(dst.format == format);

            if (src.width != dst.width || src.height != dst.height)
            {
                result.Release();
                return E_FAIL;
            }

            if (usewic)
            {
                hr = ConvertUsingWIC(src, pfGUID, targetGUID, options.filter, options.threshold, dst);
            }
            else
            {
                hr = ConvertCustom(src, options.filter, dst, options.threshold, 0, nullptr);
            }

            if (FAILED(hr))
            {
                result.Release();
                return hr;
            }

            if (statusCallback)
            {
                if (!statusCallback(index, nimages))
                {
                    result.Release();
                    return E_ABORT;
                }
            }
        }
        break;

    case TEX_DIMENSION_TEXTURE3D:
        {
            size_t index = 0;
            size_t d = metadata.depth;
            for (size_t level = 0; level < metadata.mipLevels; ++level)
            {
                for (size_t slice = 0; slice < d; ++slice, ++index)
                {
                    if (index >= nimages)
                    {
                        result.Release();
                        return E_FAIL;
                    }

                    const Image& src = srcImages[index];
                    if (src.format != metadata.format)
                    {
                        result.Release();
                        return E_FAIL;
                    }

                    if ((src.width > UINT32_MAX) || (src.height > UINT32_MAX))
                    {
                        result.Release();
                        return E_FAIL;
                    }

                    const Image& dst = dest[index];
                    assert(dst.format == format);

                    if (src.width != dst.width || src.height != dst.height)
                    {
                        result.Release();
                        return E_FAIL;
                    }

                    if (usewic)
                    {
                        hr = ConvertUsingWIC(src, pfGUID, targetGUID, options.filter, options.threshold, dst);
                    }
                    else
                    {
                        hr = ConvertCustom(src, options.filter, dst, options.threshold, slice, nullptr);
                    }

                    if (FAILED(hr))
                    {
                        result.Release();
                        return hr;
                    }

                    if (statusCallback)
                    {
                        if (!statusCallback(index, nimages))
                        {
                            result.Release();
                            return E_ABORT;
                        }
                    }
                }

                if (d > 1)
                    d >>= 1;
            }
        }
        break;

    default:
        result.Release();
        return E_FAIL;
    }

    if (statusCallback)
    {
        if (!statusCallback(nimages, nimages))
        {
            result.Release();
            return E_ABORT;
        }
    }

    return S_OK;
}


//-------------------------------------------------------------------------------------
// Convert image from planar to single plane (image)
//-------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT DirectX::ConvertToSinglePlane(const Image& srcImage, ScratchImage& image) noexcept
{
    if (!IsPlanar(srcImage.format))
        return E_INVALIDARG;

    if (!srcImage.pixels)
        return E_POINTER;

    const DXGI_FORMAT format = PlanarToSingle(srcImage.format);
    if (format == DXGI_FORMAT_UNKNOWN)
        return HRESULT_E_NOT_SUPPORTED;

    if ((srcImage.width > UINT32_MAX) || (srcImage.height > UINT32_MAX))
        return E_INVALIDARG;

    HRESULT hr = image.Initialize2D(format, srcImage.width, srcImage.height, 1, 1);
    if (FAILED(hr))
        return hr;

    const Image *rimage = image.GetImage(0, 0, 0);
    if (!rimage)
    {
        image.Release();
        return E_POINTER;
    }

    hr = ConvertToSinglePlane_(srcImage, *rimage);
    if (FAILED(hr))
    {
        image.Release();
        return hr;
    }

    return S_OK;
}


//-------------------------------------------------------------------------------------
// Convert image from planar to single plane (complex)
//-------------------------------------------------------------------------------------
_Use_decl_annotations_
HRESULT DirectX::ConvertToSinglePlane(
    const Image* srcImages,
    size_t nimages,
    const TexMetadata& metadata,
    ScratchImage& result) noexcept
{
    if (!srcImages || !nimages)
        return E_INVALIDARG;

    if (metadata.IsVolumemap())
    {
        // Direct3D does not support any planar formats for Texture3D
        return HRESULT_E_NOT_SUPPORTED;
    }

    const DXGI_FORMAT format = PlanarToSingle(metadata.format);
    if (format == DXGI_FORMAT_UNKNOWN)
        return HRESULT_E_NOT_SUPPORTED;

    if ((metadata.width > UINT32_MAX) || (metadata.height > UINT32_MAX))
        return E_INVALIDARG;

    TexMetadata mdata2 = metadata;
    mdata2.format = format;
    HRESULT hr = result.Initialize(mdata2);
    if (FAILED(hr))
        return hr;

    if (nimages != result.GetImageCount())
    {
        result.Release();
        return E_FAIL;
    }

    const Image* dest = result.GetImages();
    if (!dest)
    {
        result.Release();
        return E_POINTER;
    }

    for (size_t index = 0; index < nimages; ++index)
    {
        const Image& src = srcImages[index];
        if (src.format != metadata.format)
        {
            result.Release();
            return E_FAIL;
        }

        if ((src.width > UINT32_MAX) || (src.height > UINT32_MAX))
            return E_FAIL;

        const Image& dst = dest[index];
        assert(dst.format == format);

        if (src.width != dst.width || src.height != dst.height)
        {
            result.Release();
            return E_FAIL;
        }

        hr = ConvertToSinglePlane_(src, dst);
        if (FAILED(hr))
        {
            result.Release();
            return hr;
        }
    }

    return S_OK;
}


//-------------------------------------------------------------------------------------
// Returns the data type of a DXGI_FORMAT
//-------------------------------------------------------------------------------------
DirectX::FORMAT_TYPE DirectX::FormatDataType(_In_ DXGI_FORMAT fmt) noexcept
{
    const auto cflags = GetConvertFlags(fmt);

    switch (cflags & (CONVF_FLOAT | CONVF_UNORM | CONVF_UINT | CONVF_SNORM | CONVF_SINT))
    {
    case CONVF_FLOAT:
        return FORMAT_TYPE_FLOAT;

    case CONVF_UNORM:
        return FORMAT_TYPE_UNORM;

    case CONVF_UINT:
        return FORMAT_TYPE_UINT;

    case CONVF_SNORM:
        return FORMAT_TYPE_SNORM;

    case CONVF_SINT:
        return FORMAT_TYPE_SINT;

    default:
        return FORMAT_TYPE_TYPELESS;
    }
}
