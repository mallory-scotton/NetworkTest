///////////////////////////////////////////////////////////////////////////////
///
/// MIT License
///
/// Copyright(c) 2025 TekyoDrift
///
/// Permission is hereby granted, free of charge, to any person obtaining a
/// copy of this software and associated documentation files (the "Software"),
/// to deal in the Software without restriction, including without limitation
/// the rights to use, copy, modify, merge, publish, distribute, sublicense,
/// and/or sell copies of the Software, and to permit persons to whom the
/// Software is furnished to do so, subject to the following coditions:
///
/// The above copyright notice and this permission notice shall be included
/// in all copies or substantial portions of the Software?
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
/// DEALINGS IN THE SOFTWARE.
///
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "resources/Compressor.hpp"
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
UData Compressor::compress(const UData& data, int level)
{
    uLong maxSize = compressBound(data.size());
    UData cdata(maxSize);
    uLong csize = maxSize;

    int result = compress2(
        cdata.data(),
        &csize,
        data.data(),
        data.size(),
        level
    );
    if (result != Z_OK)
        throw std::runtime_error("Compression failed");
    cdata.resize(csize);
    return (cdata);
}

///////////////////////////////////////////////////////////////////////////////
UData Compressor::decompress(const UData& cdata, uLong size)
{
    UData data(size);
    uLong dsize = size;

    int result = uncompress(
        data.data(),
        &dsize,
        cdata.data(),
        cdata.size()
    );
    if (result != Z_OK)
        throw std::runtime_error("Decompression failed");
    return (data);
}

} // namespace tkd
