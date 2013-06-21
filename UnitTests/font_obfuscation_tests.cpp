//
//  font_obfuscation_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-04-03.
//  Copyright (c) 2012-2013 The Readium Foundation and contributors.
//  
//  The Readium SDK is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "../ePub3/ePub/container.h"
#include "../ePub3/ePub/font_obfuscation.h"
#include "../ePub3/ePub/package.h"
#include "../ePub3/utilities/byte_stream.h"
#include "catch.hpp"

#define EPUB_PATH "TestData/wasteland-otf-obf-20120118.epub"
#define FONT_SUBPATH "EPUB/OldStandard-Regular.obf.otf"
#define FONT_MANIFEST_ID "font.OldStandard.regular"

using namespace ePub3;

TEST_CASE("Obfuscated fonts are decrypted properly", "")
{
    ContainerPtr c = Container::OpenContainer(EPUB_PATH);
    PackagePtr pkg = c->DefaultPackage();
    ManifestItemPtr manifestItem = pkg->ManifestItemWithID(FONT_MANIFEST_ID);
    auto encInfo = c->EncryptionInfoForPath(FONT_SUBPATH);
    
    // should match this manifest item & encInfo
    FontObfuscator obfuscator(c.get());
    REQUIRE(obfuscator.TypeSniffer()(manifestItem.get(), encInfo.get()));
    
    // should not match this manifest item
    REQUIRE_FALSE(obfuscator.TypeSniffer()(pkg->ManifestItemWithID("nav").get(), encInfo.get()));
    
    // should not match with no encInfo
    REQUIRE_FALSE(obfuscator.TypeSniffer()(manifestItem.get(), nullptr));
    
    // should not match with a different algorithm
    EncryptionInfo otherEncInfo(c);
    otherEncInfo.SetPath(FONT_SUBPATH);
    otherEncInfo.SetAlgorithm("http://www.w3.org/2001/04/xmlenc#rsa-1_5");
    REQUIRE_FALSE(obfuscator.TypeSniffer()(manifestItem.get(), &otherEncInfo));
    
    // Read the first 1080 bytes of the font file
    auto stream = c->ReadStreamAtPath(FONT_SUBPATH);
    REQUIRE_FALSE(stream == nullptr);
    REQUIRE(stream->IsOpen());
    
    uint8_t ident[4] = { 'O', 'T', 'T', 'O' };
    uint8_t bytes[1080];
    ssize_t numRead = stream->ReadBytes(bytes, 1080);
    REQUIRE(numRead == 1080);
    REQUIRE_FALSE(memcmp(bytes, ident, 4) == 0);
    
    size_t outLen = 0;
    void* output = obfuscator.FilterData(bytes, numRead, &outLen);
    REQUIRE(outLen != 0);
    REQUIRE(memcmp(output, ident, 4) == 0);
    
    if ( output != bytes )
        delete [] reinterpret_cast<uint8_t*>(output);
}
