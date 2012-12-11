//
//  cfi.cpp
//  ePub3
//
//  Created by Jim Dovey on 2012-12-10.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#include "cfi.h"
#include <sstream>

EPUB3_BEGIN_NAMESPACE

CFI::CFI(const CFI& base, const CFI& start, const CFI& end) : _components(base._components), _rangeStart(start._components), _rangeEnd(end._components), _options(RangeTriplet)
{
}
CFI::CFI(const std::string& str)
{
    if ( CompileCFI(str) == false )
        throw InvalidCFI(std::string("Invalid CFI string: ") + str);
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
bool CFI::operator==(const std::string &str) const
{
    // what's the best approach? There are three:
    // 1. Convert this to string, compare strings
    // 2. Convert str to CFI, compare CFIs
    // 3. Parse string, comparing each component in turn.
    
    // for now I'm going with 1.
    return String() == str;
}
bool CFI::operator!=(const std::string &str) const
{
    return !(this->operator==(str));
}
CFI& CFI::operator=(const std::string &str)
{
    CFI tmp(str);
    return this->operator=(std::move(tmp));
}
CFI& CFI::Append(const ePub3::CFI& cfi)
{
    if ( IsRangeTriplet() )
    {
        // to what do we append ?
        throw std::logic_error("Appending to a ranged CFI-- what to do here?");
    }
    
    _components.insert(_components.end(), cfi._components.begin(), cfi._components.end());
    if ( cfi.IsRangeTriplet() )
    {
        _rangeStart = cfi._rangeStart;
        _rangeEnd = cfi._rangeEnd;
    }
    
    return *this;
}
CFI& CFI::Append(const std::string &str)
{
    if ( IsRangeTriplet() )
    {
        // to what do we append ?
        throw std::logic_error("Appending to a ranged CFI-- what to do here?");
    }
    
    CFI tmp(str);
    _components.insert(_components.end(), tmp._components.begin(), tmp._components.end());
    if ( tmp.IsRangeTriplet() )
    {
        _rangeStart = std::move(tmp._rangeStart);
        _rangeEnd = std::move(tmp._rangeEnd);
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
std::string CFI::SubCFIFromIndex(size_t index) const
{
    if ( index >= TotalComponents() )
        throw std::range_error((std::stringstream() << "Index " << index << " is out of bounds.").str());
    
    return Stringify(_components.begin()+index, _components.end());
}
std::string CFI::Stringify(ComponentList::const_iterator start, ComponentList::const_iterator end) const
{
    std::stringstream builder("epubcfi(");
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
CFI::StringList CFI::CFIComponentStrings(const std::string &cfi, const std::string& delimiter)
{
    CFI::StringList components;
    std::string breaks = delimiter + "[";
    std::string tmp;
    std::string::size_type pos = 0, loc = 0;
    
    while ( pos != cfi.size() )
    {
        loc = cfi.find_first_of(breaks, pos);
        if ( loc > pos )
        {
            if ( loc == std::string::npos )
                tmp.append(cfi, pos, cfi.size()-pos);
            else
                tmp.append(cfi, pos, loc-pos);
            pos = loc;
        }
        
        if ( cfi[loc] == '[' )
        {
            loc = cfi.find_first_of(']', loc);
            if ( loc == std::string::npos )
                throw std::range_error((std::stringstream() << "CFI '" << cfi << "' has an unterminated qualifier").str());
            
            ++loc;
            tmp.append(cfi, pos, loc-pos);
        }
        else if ( cfi.find(delimiter, loc) == loc )
        {
            // delimiter found, push the current string
            components.push_back(tmp);
            tmp.clear();
        }
        
        pos = loc;
    }
    
    return components;
}
bool CFI::CompileCFI(const std::string &str)
{
    // strip the 'epubcfi(...)' wrapping
    std::string cfi(str);
    if ( str.find("epubcfi(") == 0 )
    {
        cfi = cfi.substr(8, str.size()-1);
    }
    else if ( str[0] != '/' )
    {
        // invalid CFI
        return false;
    }
    
    StringList rangePieces = RangedCFIComponents(cfi);
    if ( rangePieces.empty() )
        return false;
    
    if ( CompileComponentsToList(CFIComponentStrings(rangePieces[0]), &_components) == false )
        return false;
    
    if ( rangePieces.size() == 3 )
    {
        _options |= RangeTriplet;
        if ( CompileComponentsToList(CFIComponentStrings(rangePieces[1]), &_rangeStart) == false )
            return false;
        if ( CompileComponentsToList(CFIComponentStrings(rangePieces[2]), &_rangeEnd) == false )
            return false;
    }
    
    return true;
}
bool CFI::CompileComponentsToList(const StringList &strings, ComponentList *list)
{
    try
    {
        for ( auto str : strings )
        {
            list->push_back(Component(str));
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

CFI::Component::Component(const std::string& str)
{
    Parse(str);
}
void CFI::Component::Parse(const std::string &str)
{
    if ( str.empty() )
        throw std::invalid_argument("Empty string supplied to CFI::Component");
    
    std::istringstream iss(str);
    
    // read an integer
    iss >> nodeIndex;
    
    while ( !iss.eof() )
    {
        char next;
        iss >> next;
        
        switch ( next )
        {
            case '[':
            {
                size_t pos = iss.tellg();
                iss.ignore(std::numeric_limits<std::streamsize>::max(), ']');
                size_t end = iss.tellg();
                iss.ignore(1);  // step past the closing delimiter
                
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
                if ( !iss.eof() || HasSpatialTemporalOffset() || HasCharacterOffset() )
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
CFI::Component& CFI::Component::operator=(const std::string &str)
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

EPUB3_END_NAMESPACE
