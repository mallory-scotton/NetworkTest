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
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "utils/Types.hpp"
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <zlib.h>
#include <string>
#include <memory>
#include <unordered_map>
#include <filesystem>
#include <optional>
#include <iostream>
#include <variant>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Assets packer class to handle assets
///
///////////////////////////////////////////////////////////////////////////////
class AssetsPacker
{
public:
    using ImagePtr     = std::shared_ptr<sf::Image>;
    using SoundPtr     = std::shared_ptr<sf::SoundBuffer>;
    using FontPtr      = std::shared_ptr<sf::Font>;
    using DataPtr      = std::shared_ptr<UData>;
    using AssetVariant = std::variant<ImagePtr, SoundPtr, FontPtr, DataPtr>;
    using Path         = std::filesystem::path;

public:
    enum class AssetType
    {
        Image,
        Sound,
        Font,
        Data,
        Unknown
    };

    struct CompressedAsset
    {
        UData data;                         //<! The compressed data
        size_t size;                        //<! The size of the data
        AssetType type;                     //<! The type of asset
        union
        {
            struct
            {
                Uint32 width;               //<! The width of the image
                Uint32 height;              //<! The height of the image
            } image;                        //<! Image related metadata
            struct
            {
                Uint32 channelCount;        //<! The number of channel
                Uint32 sampleRate;          //<! The sample rate
                size_t sampleCount;         //<! The sample count
            } audio;                        //<! Audio related metadat
        };
    };

private:
    std::unordered_map<std::string, CompressedAsset> m_assets;
    int m_level;

private:
    static const char* ASSETS_SIGNATURE;

public:
    AssetsPacker(int level = Z_DEFAULT_COMPRESSION);

public:
    AssetsPacker& operator<<(const Path& filepath);

private:
    AssetType detectAssetType(const Path& filepath);

public:
    void pack(const std::string& filename) const;
    void unpack(const std::string& filename);

    std::optional<ImagePtr> getImage(const std::string& key);
    std::optional<SoundPtr> getSound(const std::string& key);
    std::optional<FontPtr> getFont(const std::string& key);
    std::optional<DataPtr> getData(const std::string& key);

    std::unordered_map<std::string, CompressedAsset> getAssets(void) const;
};

std::ostream& operator<<(std::ostream& os, const AssetsPacker& packer);

} // namespace tkd
