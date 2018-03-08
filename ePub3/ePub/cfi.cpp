//
//  cfi.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-10.
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

#include "cfi.h"
#include <ePub3/utilities/error_handler.h>
#include <sstream>

EPUB3_BEGIN_NAMESPACE

CFI::CFI(const CFI& base, const CFI& start, const CFI& end) :
#if EPUB_PLATFORM(WINRT)
NativeBridge(),
#endif
_components(base._components), _rangeStart(start._components), _rangeEnd(end._components), _options(RangeTriplet)
{
}
CFI::CFI(const string& str) :
#if EPUB_PLATFORM(WINRT)
NativeBridge(),
#endif
_components(), _rangeStart(), _rangeEnd(), _options(0)
{
    if ( CompileCFI(str) == false )
        HandleError(EPUBError::CFIParseFailed, _Str("Invalid CFI string: ", str.stl_str()));
}
CFI::CFI(const CFI& base, size_t fromIndex) :
#if EPUB_PLATFORM(WINRT)
NativeBridge(),
#endif
_components(), _rangeStart(), _rangeEnd(), _options(0)
{
	Assign(base, fromIndex);
}
bool CFI::operator==(const ePub3::CFI &o) const
{
    if ( _options != o._options )
        return false;
    
    if ( _components != o._components )
        return false;
    
    if ( IsRangeTriplet() )
    {
        return _rangeStart == o._rangeStart && _rangeEnd == o._rangeEnd;
    }
    
    return true;
}
bool CFI::operator!=(const ePub3::CFI &o) const
{
    return !(this->operator==(o));
}
bool CFI::operator==(const string &str) const
{
    // what's the best approach? There are three:
    // 1. Convert this to string, compare strings
    // 2. Convert str to CFI, compare CFIs
    // 3. Parse string, comparing each component in turn.
    
    // for now I'm going with 1, although 3 is obviously the best choice
    return String() == str;
}
bool CFI::operator!=(const string &str) const
{
    return !(this->operator==(str));
}
CFI& CFI::Assign(const string &str)
{
    CFI tmp(str);
    return Assign(std::move(tmp));
}
CFI& CFI::Assign(const CFI& o)
{
    _components = o._components;
    _rangeStart = o._rangeStart;
    _rangeEnd = o._rangeEnd;
    _options = o._options;
    return *this;
}
CFI& CFI::Assign(const ePub3::CFI &o, size_t fromIndex)
{
    if ( fromIndex >= o._components.size() )
        throw std::out_of_range(_Str("Component index ", fromIndex, " out of range [0..", _components.size(), "]"));
    
    _components.assign(o._components.begin()+fromIndex, o._components.end());
    if ( o.IsRangeTriplet() )
    {
        _rangeStart = o._rangeStart;
        _rangeEnd = o._rangeEnd;
        _options |= RangeTriplet;
    }
    else if ( IsRangeTriplet() )
    {
        _rangeStart.clear();
        _rangeEnd.clear();
        _options &= ~RangeTriplet;
    }
    
    return *this;
}
CFI& CFI::Assign(CFI&& o)
{
    _components = std::move(o._components);
    _rangeStart = std::move(o._rangeStart);
    _rangeEnd = std::move(o._rangeEnd);
    _options = o._options;
    return *this;
}
CFI& CFI::Append(const ePub3::CFI& cfi)
{
    if ( IsRangeTriplet() )
    {
        // to what do we append ?
        throw RangedCFIAppendAttempt("Appending to a ranged CFI-- what to do here?");
    }
    
    _components.insert(_components.end(), cfi._components.begin(), cfi._components.end());
    if ( cfi.IsRangeTriplet() )
    {
        _rangeStart = cfi._rangeStart;
        _rangeEnd = cfi._rangeEnd;
        _options |= RangeTriplet;
    }
    
    return *this;
}
CFI& CFI::Append(const string &str)
{
    if ( IsRangeTriplet() )
    {
        // to what do we append ?
        throw RangedCFIAppendAttempt("Appending to a ranged CFI-- what to do here?");
    }
    
    CFI tmp(str);
    _components.insert(_components.end(), tmp._components.begin(), tmp._components.end());
    if ( tmp.IsRangeTriplet() )
    {
        _rangeStart = std::move(tmp._rangeStart);
        _rangeEnd = std::move(tmp._rangeEnd);
        _options |= RangeTriplet;
    }
    
    return *this;
}
size_t CFI::TotalComponents() const
{
    size_t result = _components.size();
    if ( IsRangeTriplet() )
        result += _rangeStart.size() + _rangeEnd.size();
    return result;
}
string CFI::SubCFIFromIndex(size_t index) const
{
    if ( index >= TotalComponents() )
        throw std::range_error(_Str("Index ", index, " is out of bounds."));
    
    return Stringify(_components.begin()+index, _components.end());
}
string CFI::Stringify(ComponentList::const_iterator start, ComponentList::const_iterator end) const
{
    std::stringstream builder;
    builder << "epubcfi(";
    AppendComponents(builder, start, end);
    if ( end == _components.end() && IsRangeTriplet() )
    {
        builder << ",";
        AppendComponents(builder, _rangeStart.begin(), _rangeStart.end());
        builder << ",";
        AppendComponents(builder, _rangeEnd.begin(), _rangeEnd.end());
    }
    builder << ")";
    
    return builder.str();
}
void CFI::AppendComponents(std::stringstream& builder, ComponentList::const_iterator start, ComponentList::const_iterator end)
{
    auto pos = start;
    while ( pos != end )
    {
        builder << "/" << pos->nodeIndex;
        if ( pos->HasQualifier() )
        {
            builder << "[" << pos->qualifier << "]";
        }
        if ( pos->HasCharacterOffset() )
        {
            builder << ":" << pos->characterOffset;
            
            if ( pos->HasTextQualifier() )
            {
                builder << "[" << pos->textQualifier << "]";
            }
        }
        else
        {
            if ( pos->HasTemporalOffset() )
            {
                builder << "~" << pos->temporalOffset;
            }
            if ( pos->HasSpatialOffset() )
            {
                builder << "@" << pos->spatialOffset.x << ":" << pos->spatialOffset.y;
            }
        }
        if ( pos->IsIndirector() )
        {
            builder << "!";
        }
        
        ++pos;
    }
}
CFI::StringList CFI::CFIComponentStrings(const string &cfi, const string& delimiter)
{
    CFI::StringList components;
    string breaks = delimiter + "[";
    string tmp;
    string::size_type pos = 0, loc = 0;
    
    while ( pos < cfi.size() )
    {
        loc = cfi.find_first_of(breaks, pos);
        if ( loc > pos )
        {
            if ( loc == string::npos )
            {
                tmp.append(cfi, pos, cfi.size()-pos);
                if ( !tmp.empty() )
                    components.push_back(tmp);
                tmp.clear();
                break;
            }
            else
            {
                tmp.append(cfi, pos, loc-pos);
            }
            pos = loc;
        }
        
        if ( cfi[loc] == '[' )
        {
            loc = cfi.find_first_of(']', loc);
            if ( loc == string::npos )
            {
                HandleError(EPUBError::CFIParseFailed, _Str("CFI '", cfi, "' has an unterminated qualifier"));
            }
            
            ++loc;
            tmp.append(cfi, pos, loc-pos);
        }
        else if ( cfi.find(delimiter, loc) == loc )
        {
            // delimiter found, push the current string
            if ( !tmp.empty() )
                components.push_back(tmp);
            tmp.clear();
            
            if ( loc == string::npos )
                break;
            loc++;
        }
        
        pos = loc;
    }
    
    if ( !tmp.empty() )
        components.push_back(tmp);
    
    return components;
}
bool CFI::CompileCFI(const string &str)
{
    // strip the 'epubcfi(...)' wrapping
    string cfi(str);
    if ( str.find("epubcfi(") == 0 )
    {
        cfi = cfi.substr(8, (str.size()-1)-8);
    }
    else if ( str.size() == 0 )
    {
        HandleError(EPUBError::CFIParseFailed, "Empty CFI string");
        return false;
    }
    else if ( str[0] != '/' )
    {
        HandleError(EPUBError::CFINonSlashStartCharacter);
    }
    
    StringList rangePieces = RangedCFIComponents(cfi);
    if ( rangePieces.size() != 1 && rangePieces.size() != 3 )
    {
        HandleError(EPUBError::CFIRangeComponentCountInvalid, _Str("Expected 1 or 3 range components, got ", rangePieces.size()));
        if ( rangePieces.size() == 0 )
            return false;
    }
    
    if ( CompileComponentsToList(CFIComponentStrings(rangePieces[0]), &_components) == false )
        return false;
    
    if ( rangePieces.size() >= 3 )
    {
        if ( CompileComponentsToList(CFIComponentStrings(rangePieces[1]), &_rangeStart) == false )
            return false;
        if ( CompileComponentsToList(CFIComponentStrings(rangePieces[2]), &_rangeEnd) == false )
            return false;
        
        // now sanity-check the range delimiters:
        
        // neither should be empty
        if ( _rangeStart.empty() || _rangeEnd.empty() )
        {
            HandleError(EPUBError::CFIRangeInvalid, "One of the supplied range components was empty.");
            return false;
        }
        
        // check the offsets at the end of each??? they should be the same type
        if ( (_rangeStart.back().flags & Component::OffsetsMask) != (_rangeEnd.back().flags & Component::OffsetsMask) )
        {
            HandleError(EPUBError::CFIRangeInvalid, "Offsets at the end of range components are of different types.");
            return false;
        }
        
        // ensure that there are no side-bias values
        if ( (_rangeStart.back().sideBias != SideBias::Unspecified) ||
             (_rangeEnd.back().sideBias != SideBias::Unspecified) )
        {
            HandleError(EPUBError::CFIRangeContainsSideBias);
            // can safely ignore this one
        }
        
        // where the delimiters' component ranges overlap, start must be <= end
        auto maxsz = std::max(_rangeStart.size(), _rangeEnd.size());
        bool inequalNodeIndexFound = false;
        for ( decltype(maxsz) i = 0; i < maxsz; i++ )
        {
            if ( _rangeStart[i].nodeIndex > _rangeEnd[i].nodeIndex )
            {
                HandleError(EPUBError::CFIRangeInvalid, "Range components appear to be out of order.");
            }
            else if ( !inequalNodeIndexFound && _rangeStart[i].nodeIndex < _rangeEnd[i].nodeIndex )
            {
                inequalNodeIndexFound = true;
            }
        }
        
        // if the two ranges are equal aside from their offsets, the end offset must be > the start offset
        if ( !inequalNodeIndexFound && _rangeStart.size() == _rangeEnd.size() )
        {
            Component &s = _rangeStart.back(), &e = _rangeEnd.back();
            if ( s.HasCharacterOffset() && s.characterOffset > e.characterOffset )
            {
                HandleError(EPUBError::CFIRangeInvalid, "Range components appear to be out of order.");
            }
            else
            {
                if ( s.HasTemporalOffset() && s.temporalOffset > e.temporalOffset )
                    HandleError(EPUBError::CFIRangeInvalid, "Range components appear to be out of order.");
                if ( s.HasSpatialOffset() && s.spatialOffset > e.spatialOffset )
                    HandleError(EPUBError::CFIRangeInvalid, "Range components appear to be out of order.");
            }
        }
        
        _options |= RangeTriplet;
    }
    
    return true;
}
bool CFI::CompileComponentsToList(const StringList &strings, ComponentList *list)
{
    try
    {
        for ( auto str : strings )
        {
            list->emplace_back(str);
        }
    }
    catch (const epub_spec_error&)
    {
        // re-throw any ePub spec errors
        throw;
    }
    catch (...)
    {
        return false;
    }
    
    return true;
}

#if 0
#pragma mark - CFI Component
#endif

CFI::Component::Component(const string& str) : flags(0), nodeIndex(0), qualifier(), characterOffset(0), temporalOffset(), spatialOffset(), textQualifier(), sideBias(SideBias::Unspecified)
{
    Parse(str);
}
void CFI::Component::Parse(const string &str)
{
    if ( str.empty() )
    {
        HandleError(EPUBError::CFIParseFailed, "Empty string supplied to CFI::Component");
        return;
    }
    
    std::string utf8 = str.stl_str();
    std::istringstream iss(utf8);
    
    // read an integer
    iss >> nodeIndex;
    if ( nodeIndex == 0 && iss.fail() )
    {
        HandleError(EPUBError::CFIParseFailed, _Str("No node value at start of CFI::Component string '", str, "'"));
        return;
    }
    
    while ( !iss.eof() )
    {
        char next = 0;
        iss >> next;
        
        switch ( next )
        {
            case '[':
            {
                size_t pos = static_cast<size_t>(iss.tellg());
                iss.ignore(std::numeric_limits<std::streamsize>::max(), ']');
                size_t end = ((size_t)iss.tellg()) - 1;
                
                if ( iss.eof() )
                {
                    HandleError(EPUBError::CFIParseFailed);
                    return;
                }
                
                if ( characterOffset != 0 )
                {
                    // this is a text qualifier
                    flags |= TextQualifier;
                    std::string sub = utf8.substr(pos, end-pos);
                    
                    // is there a side-bias?
                    auto biasPos = sub.find(";s=");
                    if ( biasPos == std::string::npos )
                    {
                        textQualifier = std::move(sub);
                    }
                    else
                    {
                        textQualifier = sub.substr(0, biasPos);
                        if ( sub.size() > biasPos + 3 )
                        {
                            switch ( sub[biasPos+3] )
                            {
                                case 'b':
                                    sideBias = SideBias::Before;
                                    break;
                                case 'a':
                                    sideBias = SideBias::After;
                                    break;
                                default:
                                    sideBias = SideBias::Unspecified;
                                    break;
                            }
                        }
                    }
                }
                else
                {
                    // it's a position qualifier
                    qualifier = utf8.substr(pos, end-pos);
                    flags |= Qualifier;
                }
                
                break;
            }
                
            case '~':
            {
                // character offsets and spatial/temporal offsets are mutually exclusive
                if ( HasCharacterOffset() )
                    break;
                
                // read a numeral
                iss >> temporalOffset;
                flags |= TemporalOffset;
                break;
            }
                
            case '@':
            {
                // character offsets and spatial/temporal offsets are mutually exclusive
                if ( HasCharacterOffset() )
                    break;
                
                // two floats, separated by a colon
                float x, y;
                
                // read x
                iss >> x;
                
                // check for and skip delimiter
                if ( iss.peek() != ':' )
                    break;
                iss.ignore(1);
                
                // read y
                iss >> y;
                
                spatialOffset.x = x;
                spatialOffset.y = y;
                flags |= SpatialOffset;
                break;
            }
                
            case ':':
            {
                // character offsets and spatial/temporal offsets are mutually exclusive
                if ( HasSpatialTemporalOffset() )
                    break;
                
                iss >> characterOffset;
                flags |= CharacterOffset;
                break;
            }
                
            case '!':
            {
                // must be the last character, and no offsets
                if ( ((int)iss.peek()) != -1 || HasSpatialTemporalOffset() || HasCharacterOffset() )
                    break;
                
                flags |= Indirector;
                break;
            }
                
            default:
                break;
        }
    }
}
bool CFI::Component::operator==(const ePub3::CFI::Component &o) const
{
    // holy compound statements Batman!
    if ( (flags != o.flags) ||
         (nodeIndex != o.nodeIndex) ||
         (HasQualifier() && qualifier != o.qualifier) ||
         (HasCharacterOffset() && characterOffset != o.characterOffset) ||
         (HasSpatialOffset() && (spatialOffset.x != o.spatialOffset.x || spatialOffset.y != o.spatialOffset.y)) ||
         (HasTemporalOffset() && temporalOffset != o.temporalOffset) ||
         (HasTextQualifier() && textQualifier != o.textQualifier) )
    {
        return false;
    }
    
    return true;
}
bool CFI::Component::operator!=(const ePub3::CFI::Component &o) const
{
    return !(this->operator==(o));
}
CFI::Component& CFI::Component::operator=(const ePub3::CFI::Component &o)
{
    flags = o.flags;
    nodeIndex = o.nodeIndex;
    qualifier = o.qualifier;
    characterOffset = o.characterOffset;
    temporalOffset = o.temporalOffset;
    spatialOffset = o.spatialOffset;
    textQualifier = o.textQualifier;
    sideBias = o.sideBias;
    
    return *this;
}
CFI::Component& CFI::Component::operator=(ePub3::CFI::Component &&o)
{
    flags = o.flags;
    nodeIndex = o.nodeIndex;
    qualifier = std::move(o.qualifier);
    characterOffset = o.characterOffset;
    temporalOffset = o.temporalOffset;
    spatialOffset = o.spatialOffset;
    textQualifier = std::move(o.textQualifier);
    sideBias = o.sideBias;
    
    return *this;
}
CFI::Component& CFI::Component::operator=(const string &str)
{
    flags = 0;
    nodeIndex = 0;
    characterOffset = 0;
    temporalOffset = 0.0;
    spatialOffset.x = 0.0; spatialOffset.y = 0.0;
    qualifier.clear();
    textQualifier.clear();
    sideBias = SideBias::Unspecified;
    
    Parse(str);
    return *this;
}

bool CFI::Component::Point::operator<(const ePub3::CFI::Component::Point &o) const
{
    return x < o.x && y < o.y;
}
bool CFI::Component::Point::operator>(const ePub3::CFI::Component::Point &o) const
{
    return x > o.x || y > o.y;
}
bool CFI::Component::Point::operator<=(const ePub3::CFI::Component::Point &o) const
{
    return x <= o.x && y <= o.y;
}
bool CFI::Component::Point::operator>=(const ePub3::CFI::Component::Point &o) const
{
    return x >= o.x || y > o.y;
}
bool CFI::Component::Point::operator==(const ePub3::CFI::Component::Point &o) const
{
    return x == o.x && y == o.y;
}
bool CFI::Component::Point::operator!=(const ePub3::CFI::Component::Point &o) const
{
    return x != o.x || y != o.y;
}

EPUB3_END_NAMESPACE
