//
//  media-overlays_smil_data.cpp
//  ePub3
//
//  Created by Daniel Weck on 2013-09-18.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//  
//  Redistribution and use in source and binary forms, with or without modification, 
//  are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this 
//  list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, 
//  this list of conditions and the following disclaimer in the documentation and/or 
//  other materials provided with the distribution.
//  3. Neither the name of the organization nor the names of its contributors may be 
//  used to endorse or promote products derived from this software without specific 
//  prior written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
//  OF THE POSSIBILITY OF SUCH DAMAGE.

#include "media-overlays_smil_data.h"

EPUB3_BEGIN_NAMESPACE

SMILData::~SMILData()
{
    //printf("~SMILData(%s)\n", _manifestItem->Href().c_str());
    //printf("~SMILData()\n");
    //
}

const string & SMILData::TimeNode::Name() const
{
    throw std::runtime_error("TimeNode Name()");
    //return string::EmptyString;
}

SMILData::TimeNode::~TimeNode()
{
    //printf("~TimeNode()\n");
}

const bool SMILData::Media::IsAudio() const
{
    throw std::runtime_error("Media IsAudio()");
    //return Name() == @"audio";
}

const bool SMILData::Media::IsText() const
{
    throw std::runtime_error("Media IsText()");
    //return Name() == @"text";
}

SMILData::Media::~Media()
{
    //printf("~Media()\n");
}

SMILData::Audio::~Audio()
{
	//printf("~Audio()\n");
}

SMILData::Text::~Text()
{
    //printf("~Text()\n");
}

const bool SMILData::TimeContainer::IsParallel() const
{
    throw std::runtime_error("TimeContainer IsParallel()");
    //return Name() == @"par";
}

const bool SMILData::TimeContainer::IsSequence() const
{
    throw std::runtime_error("TimeContainer IsSequence()");
    //return Name() == @"seq";
}

SMILData::TimeContainer::~TimeContainer()
{
    //printf("~TimeContainer()\n");
}

SMILData::Sequence::~Sequence()
{
    //printf("~Sequence()\n");
}

SMILData::Parallel::~Parallel()
{
    //printf("~Parallel()\n");
}

const string SMILData::Sequence::_Name = string("seq");
const string SMILData::Parallel::_Name = string("par");
const string SMILData::Audio::_Name = string("audio");
const string SMILData::Text::_Name = string("text");

EPUB3_END_NAMESPACE
