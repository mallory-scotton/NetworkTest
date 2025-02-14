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
#include "resources/AssetsPacker.hpp"
#include "resources/Compressor.hpp"
#include <fstream>
#include <sstream>
#include <limits>
#include <iomanip>
#include <filesystem>
#include <stdexcept>
#include <memory>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
#define RCASTOF(arg) reinterpret_cast<char*>(&arg), sizeof(arg)
#define CRCASTOF(arg) reinterpret_cast<const char*>(&arg), sizeof(arg)

///////////////////////////////////////////////////////////////////////////////
const char* AssetsPacker::ASSETS_SIGNATURE = "TKDASSETS";

///////////////////////////////////////////////////////////////////////////////
AssetsPacker::AssetsPacker(int level)
    : m_level(level)
{}

///////////////////////////////////////////////////////////////////////////////
AssetsPacker& AssetsPacker::operator<<(
    const std::pair<std::string, Path>& entry
)
{
    const std::string& key = entry.first;
    const Path& filepath = entry.second;

    AssetType type = detectAssetType(filepath);
    if (type == AssetType::Unknown)
        throw std::runtime_error("Unknown asset type: " + filepath.string());

    std::ifstream file(filepath, std::ios::binary);
    if (!file)
        throw std::runtime_error("Cannot open file: " + filepath.string());

    auto raw = UData(std::istreambuf_iterator<char>(file), {});
    auto data = Compressor::compress(raw);

    CompressedAsset asset{data, raw.size(), type, {}};

    if (type == AssetType::Image) {
        sf::Image img;
        if (img.loadFromFile(filepath.string())) {
            asset.image.width = img.getSize().x;
            asset.image.height = img.getSize().y;
        }
    } else if (type == AssetType::Sound) {
        sf::SoundBuffer buffer;
        if (buffer.loadFromFile(filepath.string())) {
            asset.audio.channelCount = buffer.getChannelCount();
            asset.audio.sampleRate = buffer.getSampleRate();
            asset.audio.sampleCount = buffer.getSampleCount();
        }
    }

    m_assets[key] = std::move(asset);
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////
AssetsPacker::AssetType AssetsPacker::detectAssetType(const Path& filepath)
{
    std::string ext = filepath.extension().string();
    if (ext == ".png" || ext == ".jpg" || ext == ".jpeg")
        return (AssetType::Image);
    if (ext == ".wav" || ext == ".ogg" || ext == ".flac")
        return (AssetType::Sound);
    if (ext == ".ttf" || ext == ".otf")
        return (AssetType::Font);
    if (ext == ".bin" || ext == ".dat")
        return (AssetType::Data);
    return (AssetType::Unknown);
}

///////////////////////////////////////////////////////////////////////////////
std::string AssetsPacker::formatSize(size_t size)
{
    const char* units[] = {"o", "Ko", "Mo", "Go"};
    int idx = 0;
    double fSize = static_cast<double>(size);

    while (fSize >= 1024.0 && idx < 3) {
        fSize /= 1024.0;
        idx++;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << fSize << " " << units[idx];
    return (oss.str());
}

///////////////////////////////////////////////////////////////////////////////
void AssetsPacker::pack(const std::string& filename) const
{
    std::ofstream out(filename, std::ios::binary);
    if (!out)
        throw std::runtime_error("Cannot open file: " + filename);

    // Write the assets signature
    out.write(ASSETS_SIGNATURE, 9);

    // Write the number of assets
    Uint32 count = static_cast<Uint32>(m_assets.size());
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& [key, asset] : m_assets) {
        // Write the asset key length and key
        Uint32 length = static_cast<Uint32>(key.size());
        out.write(CRCASTOF(length));
        out.write(key.data(), length);

        // Write the asset type
        out.write(CRCASTOF(asset.type));
        // Write the asset data size
        out.write(CRCASTOF(asset.size));

        // Write the metadata if applicable
        if (asset.type == AssetType::Image) {
            out.write(CRCASTOF(asset.image.width));
            out.write(CRCASTOF(asset.image.height));
        } else if (asset.type == AssetType::Sound) {
            out.write(CRCASTOF(asset.audio.channelCount));
            out.write(CRCASTOF(asset.audio.sampleRate));
            out.write(CRCASTOF(asset.audio.sampleCount));
        }

        // Write the compressed data
        out.write(
            reinterpret_cast<const char*>(asset.data.data()),
            asset.size
        );
    }
}

///////////////////////////////////////////////////////////////////////////////
void AssetsPacker::unpack(const std::string& filename)
{
    std::ifstream in(filename, std::ios::binary);
    if (!in)
        throw std::runtime_error("Cannot open file: " + filename);

    // Read the signature of the file
    char signature[9];
    in.read(signature, 9);
    if (std::string(signature, 9) != ASSETS_SIGNATURE)
        throw std::runtime_error("Invalid asset file signature");

    // Read the number of assets in the file
    Uint32 count = 0;
    in.read(RCASTOF(count));

    // Clear the previous assets
    m_assets.clear();

    for (Uint32 i = 0; i < count; i++) {
        // Read the key length and the key
        Uint32 length = 0;
        in.read(RCASTOF(length));
        std::string key(length, '\0');
        in.read(key.data(), length);

        // Read the type of the asset
        AssetType type;
        in.read(RCASTOF(type));

        // Read the data size of the asset
        size_t size;
        in.read(RCASTOF(size));

        // Create a new asset base on the size and type
        CompressedAsset asset{{}, size, type, {}};

        // Read the metadata if applicable
        if (type == AssetType::Image) {
            in.read(RCASTOF(asset.image.width));
            in.read(RCASTOF(asset.image.height));
        } else if (type == AssetType::Sound) {
            in.read(RCASTOF(asset.audio.channelCount));
            in.read(RCASTOF(asset.audio.sampleRate));
            in.read(RCASTOF(asset.audio.sampleCount));
        }

        // Read the compressed data
        asset.data.resize(size);
        in.read(reinterpret_cast<char*>(asset.data.data()), size);

        // Save the asset in the map of assets
        m_assets[key] = std::move(asset);
    }
}

///////////////////////////////////////////////////////////////////////////////
void AssetsPacker::addAsset(const std::string& key, const Path& filepath)
{
    *this << std::make_pair(key, filepath);
}

///////////////////////////////////////////////////////////////////////////////
std::optional<AssetsPacker::ImagePtr> AssetsPacker::getImage(
    const std::string& key
)
{
    auto it = m_assets.find(key);
    if (it == m_assets.end() || it->second.type != AssetType::Image)
        return (std::nullopt);

    auto data = Compressor::decompress(it->second.data, it->second.size);

    auto img = std::make_shared<sf::Image>();
    if (!img->loadFromMemory(data.data(), data.size()))
        return (std::nullopt);

    return (img);
}

///////////////////////////////////////////////////////////////////////////////
std::optional<AssetsPacker::SoundPtr> AssetsPacker::getSound(
    const std::string& key
)
{
    auto it = m_assets.find(key);
    if (it == m_assets.end() || it->second.type != AssetType::Sound)
        return (std::nullopt);

    auto data = Compressor::decompress(it->second.data, it->second.size);

    auto buffer = std::make_shared<sf::SoundBuffer>();
    if (!buffer->loadFromMemory(data.data(), data.size()))
        return (std::nullopt);

    return (buffer);
}

///////////////////////////////////////////////////////////////////////////////
std::optional<AssetsPacker::FontPtr> AssetsPacker::getFont(
    const std::string& key
)
{
    auto it = m_assets.find(key);
    if (it == m_assets.end() || it->second.type != AssetType::Font)
        return (std::nullopt);

    auto data = Compressor::decompress(it->second.data, it->second.size);

    auto font = std::make_shared<sf::Font>();
    if (!font->loadFromMemory(data.data(), data.size()))
        return (std::nullopt);

    return (font);
}

///////////////////////////////////////////////////////////////////////////////
std::optional<AssetsPacker::DataPtr> AssetsPacker::getData(
    const std::string& key
)
{
    auto it = m_assets.find(key);
    if (it == m_assets.end() || it->second.type != AssetType::Data)
        return (std::nullopt);

    auto data = Compressor::decompress(it->second.data, it->second.size);

    return (std::make_shared<UData>(data));
}

///////////////////////////////////////////////////////////////////////////////
std::unordered_map<
    std::string, AssetsPacker::CompressedAsset
> AssetsPacker::getAssets(void) const
{
    return (m_assets);
}

///////////////////////////////////////////////////////////////////////////////
void AssetsPacker::clear(void)
{
    m_assets.clear();
}

///////////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& os, const AssetsPacker& packer)
{
    std::unordered_map<
        std::string, AssetsPacker::CompressedAsset
    > assets = packer.getAssets();
    size_t size = assets.size(), idx = 0;

    os << '[' << size << ']' << std::endl;

    size_t length = 0;
    for (const auto& [key, _] : assets) {
        if (key.length() > length)
            length = key.length();
    }

    os << std::setprecision(3);
    for (const auto& [key, asset] : assets) {
        if (idx != size - 1)
            os << " ├─ ";
        else
            os << " └─ ";

        if (asset.type == AssetsPacker::AssetType::Image)
            os << "\033[1;32m█\033[1;0m";
        else if (asset.type == AssetsPacker::AssetType::Sound)
            os << "\033[1;36m█\033[1;0m";
        else if (asset.type == AssetsPacker::AssetType::Font)
            os << "\033[1;35m█\033[1;0m";
        else if (asset.type == AssetsPacker::AssetType::Data)
            os << "\033[1;33m█\033[1;0m";

        os << ' ' << key;
        for (size_t i = key.length(); i < length + 4; i++)
            os << ' ';
        os  << AssetsPacker::formatSize(asset.size) << std::endl;
        idx++;
    }
    os << std::defaultfloat;
    return (os);
}

} // namespace tkd
