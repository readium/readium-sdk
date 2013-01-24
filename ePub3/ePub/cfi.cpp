//
//  cfi.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-10.
//  Copyright (c) 2012-2013 The Readium Foundation.
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

#include "cfi.h"
#include <sstream>

EPUB3_BEGIN_NAMESPACE

CFI::CFI(const CFI& base, const CFI& start, const CFI& end) : _components(base._components), _rangeStart(start._components), _rangeEnd(end._components), _options(RangeTriplet)
{
}
CFI::CFI(const string& str) : CFI()
{
    if ( CompileCFI(str) == false )
        throw InvalidCFI(std::string("Invalid CFI string: ") + str.stl_str());
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
        throw std::range_error((std::stringstream() << "Index " << index << " is out of bounds.").str());
    
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
                throw std::range_error((std::stringstream() << "CFI '" << cfi << "' has an unterminated qualifier").str());
            
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
    else if ( str.size() == 0 || str[0] != '/' )
    {
        // invalid CFI
        return false;
    }
    
    StringList rangePieces = RangedCFIComponents(cfi);
    if ( rangePieces.size() != 1 && rangePieces.size() != 3 )
        return false;
    
    if ( CompileComponentsToList(CFIComponentStrings(rangePieces[0]), &_components) == false )
        return false;
    
    if ( rangePieces.size() == 3 )
    {
        if ( CompileComponentsToList(CFIComponentStrings(rangePieces[1]), &_rangeStart) == false )
            return false;
        if ( CompileComponentsToList(CFIComponentStrings(rangePieces[2]), &_rangeEnd) == false )
            return false;
        
        // now sanity-check the range delimiters:
        
        // neither should be empty
        if ( _rangeStart.empty() || _rangeEnd.empty() )
            return false;
        
        // check the offsets at the end of each— they should be the same type
        if ( (_rangeStart.back().flags & Component::OffsetsMask) != (_rangeEnd.back().flags & Component::OffsetsMask) )
            return false;
        
        // where the delimiters' component ranges overlap, start must be <= end
        auto maxsz = std::max(_rangeStart.size(), _rangeEnd.size());
        bool inequalNodeIndexFound = false;
        for ( decltype(maxsz) i = 0; i < maxsz; i++ )
        {
            if ( _rangeStart[i].nodeIndex > _rangeEnd[i].nodeIndex )
                return false;
            else if ( !inequalNodeIndexFound && _rangeStart[i].nodeIndex < _rangeEnd[i].nodeIndex )
                inequalNodeIndexFound = true;
        }
        
        // if the two ranges are equal aside from their offsets, the end offset must be > the start offset
        if ( !inequalNodeIndexFound && _rangeStart.size() == _rangeEnd.size() )
        {
            Component &s = _rangeStart.back(), &e = _rangeEnd.back();
            if ( s.HasCharacterOffset() && s.characterOffset > e.characterOffset )
            {
                return false;
            }
            else
            {
                if ( s.HasTemporalOffset() && s.temporalOffset > e.temporalOffset )
                    return false;
                if ( s.HasSpatialOffset() && s.spatialOffset > e.spatialOffset )
                    return false;
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
    catch (...)
    {
        return false;
    }
    
    return true;
}

#if 0
#pragma mark - CFI Component
#endif

CFI::Component::Component(const string& str) : Component()
{
    Parse(str);
}
void CFI::Component::Parse(const string &str)
{
    if ( str.empty() )
        throw std::invalid_argument("Empty string supplied to CFI::Component");
    
    std::istringstream iss(str.stl_str());
    
    // read an integer
    iss >> nodeIndex;
    if ( nodeIndex == 0 && iss.fail() )
        throw std::invalid_argument(_Str("No node value at start of CFI::Component string '", str, "'"));
    
    while ( !iss.eof() )
    {
        char next = 0;
        iss >> next;
        
        switch ( next )
        {
            case '[':
            {
                size_t pos = iss.tellg();
                iss.ignore(std::numeric_limits<std::streamsize>::max(), ']');
                size_t end = ((size_t)iss.tellg()) - 1;
                
                if ( iss.eof() )
                    throw std::invalid_argument(_Str("Invalid string supplied to CFI::Component: ", str));
                
                if ( characterOffset != 0 )
                {
                    // this is a text qualifier
                    textQualifier = str.substr(pos, end-pos);
                    flags |= TextQualifier;
                }
                else
                {
                    // it's a position qualifier
                    qualifier = str.substr(pos, end-pos);
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
    
    return *this;
}
CFI::Component& CFI::Component::operator=(const string &str)
{
    flags = 0;
    nodeIndex = 0;
    characterOffset = 0;
    temporalOffset = 0.0;
    spatialOffset = {0.0, 0.0};
    qualifier.clear();
    textQualifier.clear();
    
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
