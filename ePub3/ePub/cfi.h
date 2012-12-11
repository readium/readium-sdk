//
//  cfi.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-10.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#ifndef __ePub3__cfi__
#define __ePub3__cfi__

#include "epub3.h"
#include <vector>

EPUB3_BEGIN_NAMESPACE

class CFI
{
public:
    CFI() = default;
    CFI(const CFI& base, const CFI& start, const CFI& end);
    CFI(const std::string& str);
    CFI(const CFI& o) : _components(o._components) {}
    CFI(CFI&& o) : _components(std::move(o._components)) {}
    virtual ~CFI() {}
    
    std::string String() const { return Stringify(_components.begin(), _components.end()); }
    bool IsRangeTriplet() const { return (_options & RangeTriplet) == RangeTriplet; }
    bool Empty() const { return _components.empty(); }
    void Clear() { _components.clear(); }
    
    bool operator == (const CFI& o) const;
    bool operator == (const std::string& str) const;
    bool operator != (const CFI& o) const;
    bool operator != (const std::string& str) const;
    
    CFI& operator=(const CFI& o) { _components = o._components; return *this; }
    CFI& operator=(CFI&& o) { _components = std::move(o._components); return *this; }
    CFI& operator=(const std::string& str);
    
    CFI& Append(const CFI& cfi);
    CFI& Append(const std::string& str);
    CFI& operator+(const CFI& cfi) { return Append(cfi); }
    CFI& operator+(const std::string& str) { return Append(str); }
    
    class InvalidCFI : public std::logic_error
    {
    public:
        InvalidCFI(const std::string& str) : std::logic_error(str) {}
        InvalidCFI(const char * str) : std::logic_error(str) {}
        virtual ~InvalidCFI() {}
    };
    
protected:
    struct Component
    {
        enum Flags : uint8_t
        {
            Qualifier        = 1<<0,
            CharacterOffset  = 1<<1,
            TemporalOffset   = 1<<2,
            SpatialOffset    = 1<<3,
            Indirector       = 1<<4,
            TextQualifier    = 1<<5,
            
            SpatialTemporalOffset = TemporalOffset|SpatialOffset,
        };
        
        struct Point
        {
            float x;
            float y;
        };
        
        uint8_t     flags;
        uint32_t    nodeIndex;
        std::string qualifier;
        uint32_t    characterOffset;
        float       temporalOffset;
        Point       spatialOffset;
        std::string textQualifier;
        
        Component() = default;
        Component(const std::string& str);
        Component(const Component& o) = default;
        Component(Component&& o) : flags(o.flags), nodeIndex(o.nodeIndex), qualifier(std::move(o.qualifier)), characterOffset(o.characterOffset), temporalOffset(o.temporalOffset), spatialOffset(o.spatialOffset), textQualifier(std::move(o.textQualifier)) {}
        ~Component() = default;
        
        bool operator==(const Component& o) const;
        bool operator!=(const Component& o) const;
        Component& operator=(const Component& o);
        Component& operator=(Component&& o);
        Component& operator=(const std::string& str);
        
        bool HasFlag(Flags flag) const { return (flags & flag) == flag; }
        bool HasQualifier() const { return HasFlag(Qualifier); }
        bool HasCharacterOffset() const { return HasFlag(CharacterOffset); }
        bool HasTemporalOffset() const { return HasFlag(TemporalOffset); }
        bool HasSpatialOffset() const { return HasFlag(SpatialOffset); }
        bool IsIndirector() const { return HasFlag(Indirector); }
        bool HasTextQualifier() const { return HasFlag(TextQualifier); }
        bool HasSpatialTemporalOffset() const { return HasFlag(SpatialTemporalOffset); }
        
    private:
        void Parse(const std::string& str);
    };
    
    enum Options : uint8_t
    {
        RangeTriplet        = 1<<0,
    };
    
    typedef std::vector<Component>  ComponentList;
    
    ComponentList   _components;
    uint8_t         _options;
    
    ComponentList   _rangeStart;
    ComponentList   _rangeEnd;
    
    // Package should be able to work with components
    friend class Package;
    
    size_t TotalComponents() const;
    std::string SubCFIFromIndex(size_t index) const;
    std::string Stringify(ComponentList::const_iterator start, ComponentList::const_iterator end) const;
    static void AppendComponents(std::stringstream& stream, ComponentList::const_iterator start, ComponentList::const_iterator end);
    
    typedef std::vector<std::string>    StringList;
    static StringList CFIComponentStrings(const std::string& cfi, const std::string& delimiter = "/");
    static StringList RangedCFIComponents(const std::string& cfi) { return CFIComponentStrings(cfi, ","); }
    static bool CompileComponentsToList(const StringList& strings, ComponentList* list);
    bool CompileCFI(const std::string& str);
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__cfi__) */
