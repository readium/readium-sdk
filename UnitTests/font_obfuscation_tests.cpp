//
//  font_obfuscation_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-04-03.
//
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//  3. Neither the name of the organization nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
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
    ManifestItemPtr nonFontItem = pkg->FirstSpineItem()->ManifestItem();
    auto encInfo = c->EncryptionInfoForPath(FONT_SUBPATH);
    
    // should match this manifest item & encInfo
    FontObfuscator obfuscator(c);
    FilterContext* ctx = obfuscator.MakeFilterContext(manifestItem);
    
    REQUIRE(obfuscator.TypeSniffer()(manifestItem));
    
    // should not match this manifest item
    REQUIRE_FALSE(obfuscator.TypeSniffer()(pkg->ManifestItemWithID("nav")));
    
    // should not match with a different algorithm
    EncryptionInfo otherEncInfo(c);
    auto alg = encInfo->Algorithm();
    encInfo->SetAlgorithm("http://www.w3.org/2001/04/xmlenc#rsa-1_5");
    REQUIRE_FALSE(obfuscator.TypeSniffer()(manifestItem));
    encInfo->SetAlgorithm(alg);
    
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
    void* output = obfuscator.FilterData(ctx, bytes, numRead, &outLen);
    REQUIRE(outLen != 0);
    REQUIRE(memcmp(output, ident, 4) == 0);
    
    if ( output != bytes )
        delete [] reinterpret_cast<uint8_t*>(output);
    
    delete ctx;
}
