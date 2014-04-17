//
//  cfi.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-10.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//  
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
//  
//  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this notice, 
//  Readium Foundation reserves the right to license this material under a different separate license, 
//  and if you have done so, the terms of that separate license control and the following references 
//  to GPL do not apply).
//  
//  This program is free software: you can redistribute it and/or modify it under the terms of the GNU 
//  Affero General Public License as published by the Free Software Foundation, either version 3 of 
//  the License, or (at your option) any later version. You should have received a copy of the GNU 
//  Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __ePub3__cfi__
#define __ePub3__cfi__

#include <ePub3/epub3.h>
#include <ePub3/utilities/utfstring.h>
#include <vector>

EPUB3_BEGIN_NAMESPACE

/**
 The CFI class implements an EPUB 3 Content Fragment Identifier.
 
 A CFI is similar in principle to a DOMRange: it identifies a particular location
 within a publication.
 
 @ingroup epub-model
 */
class CFI
#if EPUB_PLATFORM(WINRT)
	: public NativeBridge
#endif
{
public:
    ///
    /// Value for side-bias for a character location.
    enum SideBias : uint8_t
    {
        Unspecified     = 0,
        Before,
        After,
    };
    
public:
    ///
    /// Create an empty CFI.
                    CFI() : _components(), _rangeStart(), _rangeEnd(), _options(0) {}
    /**
     Create a ranged CFI from a base and two relative CFIs.
     @param base A CFI consisting of components common to both the start and end of
     the resulting range.
     @param start A relative CFI denoting the path from `base` to the start of a
     range.
     @param end A relative CFI denoting the path from `base` to the end of a range.
     */
    EPUB3_EXPORT    CFI(const CFI& base, const CFI& start, const CFI& end);
    /**
     Create a CFI from a string representation.
     @param str A string representation of a CFI; the `epubcfi(...)` wrapping is
     optional.
     */
    EPUB3_EXPORT    CFI(const string& str);
    ///
    /// Create a copy of an existing CFI.
                    CFI(const CFI& o) : _components(o._components), _rangeStart(o._rangeStart), _rangeEnd(o._rangeEnd), _options(o._options) {}
    /**
     Creates a relative CFI from a particular node index within another.
     
     This can be used to create a document-content CFI from a container-level CFI.
     For example, given the following code:
     
         CFI mainCFI("/6/4!/2/11:5");
         CFI subCFI(mainCFI, 2);
         std::cout << subCFI.String() << std::endl;
     
     ...this will be printed to the console:
     
        /2/11:5
     
     @param o A CFI from which to extract a sub-component.
     @param fromIndex The index in `o` at which to start copying components.
     */
    EPUB3_EXPORT    CFI(const CFI& o, size_t fromIndex);
    ///
    /// C++11 move constructor.
    EPUB3_EXPORT    CFI(CFI&& o) : _components(std::move(o._components)), _rangeStart(std::move(o._rangeStart)), _rangeEnd(std::move(o._rangeEnd)), _options(o._options) {}
    virtual         ~CFI() {}
    
    /**
     Obtains a string representation of the CFI.
     @result A CFI string, including the `epubcfi(...)` wrapper.
     */
    string          String()                const           { return Stringify(_components.begin(), _components.end()); }
    
    /**
     Determine whether a CFI represents a location or a range.
     @result Returns `true` if the CFI represents a range, `false` otherwise.
     */
    bool            IsRangeTriplet()        const           { return (_options & RangeTriplet) == RangeTriplet; }
    
    /**
     Determine if a CFI is empty.
     @result Returns `true` if the CFI has no components, `false` otherwise.
     */
    bool            Empty()                 const           { return _components.empty(); }
    ///
    /// Clears a CFI, making it empty.
    void            Clear()                                 { _components.clear(); }
    
    ///
    /// Determines whether two CFIs are equal.
    EPUB3_EXPORT
    bool            operator==(const CFI& o)        const;
    ///
    /// Determines whether a CFI is equal to a CFI string representation.
    EPUB3_EXPORT
    bool            operator==(const string& str)   const;
    ///
    /// Determines whether two CFIs are inequal.
    EPUB3_EXPORT
    bool            operator!=(const CFI& o)        const;
    ///
    /// Determines whether a CFI is inequal to a CFI string representation.
    EPUB3_EXPORT
    bool            operator!=(const string& str)   const;
    
    ///
    /// Assigns a new value to a CFI by copying.
    EPUB3_EXPORT
    CFI&            Assign(const CFI& o);
    CFI&            operator=(const CFI& o)                 { return Assign(o); }
    
    ///
    /// Assigns a new value to a CFI by moving.
    EPUB3_EXPORT
    CFI&            Assign(CFI&& o);
    CFI&            operator=(CFI&& o)                      { return Assign(std::move(o)); }
    
    ///
    /// Assigns a new value to a CFI using a sub-path of another CFI
    /// @see CFI(const CFI&, size_t)
    EPUB3_EXPORT
    CFI&            Assign(const CFI& o, size_t fromIndex);
    
    ///
    /// Assigns a new value to a CFI from a CFI string representation.
    CFI&            operator=(const string& str)            { return Assign(str); }
    EPUB3_EXPORT
    CFI&            Assign(const string& str);
    
    /**
     Appends the components of one CFI to another.
     @note It is not possible to append components to a ranged CFI.
     @throws RangedCFIAppendAttempt if the LHS is a ranged CFI.
     */
    EPUB3_EXPORT
    CFI&            Append(const CFI& cfi);
    CFI&            operator+=(const CFI& cfi)              { return Append(cfi); }
    CFI             operator+(const CFI& cfi)       const   { return CFI(*this).Append(cfi); }
    
    /**
     Appends the components of a CFI string representation.
     @note It is not possible to append components to a ranged CFI.
     @throws RangedCFIAppendAttempt if the LHS is a ranged CFI.
     */
    EPUB3_EXPORT
    CFI&            Append(const string& str);
    CFI&            operator+=(const string& str)           { return Append(str); }
    CFI             operator+(const string& str)    const   { return CFI(*this).Append(str); }
    
    ///
    /// Returns the side-bias of a character-offset CFI.
    SideBias        CharacterSideBias() const _NOEXCEPT     { return _components.back().sideBias; }
    
    ///
    /// The exception thrown when an invalid CFI string is encountered.
    class InvalidCFI : public std::logic_error
    {
    public:
        InvalidCFI(const std::string& str) _GCC_NOTHROW : std::logic_error(str) {}
        InvalidCFI(const char * str) _GCC_NOTHROW : std::logic_error(str) {}
        virtual ~InvalidCFI() _GCC_NOTHROW {}
    };
    ///
    /// An attempt was made to append components to a ranged CFI, which is not valid.
    class RangedCFIAppendAttempt : public std::logic_error
    {
    public:
        RangedCFIAppendAttempt(const std::string& str) _GCC_NOTHROW : std::logic_error(str) {}
        RangedCFIAppendAttempt(const char * str) _GCC_NOTHROW : std::logic_error(str) {}
        virtual ~RangedCFIAppendAttempt() _GCC_NOTHROW {}
    };
    
protected:
    ///
    /// A single component of a CFI.
    struct Component
    {
        ///
        /// Bitfield values identifying special status for a CFI component.
        enum Flags : uint8_t
        {
            Qualifier        = 1<<0,    ///< The component contains an `id` or `idref` qualifier, i.e. `[bob]`
            CharacterOffset  = 1<<1,    ///< The component has a character offset value, i.e. `:12`
            TemporalOffset   = 1<<2,    ///< The component has a temporal offset value, i.e. `~87.24`.
            SpatialOffset    = 1<<3,    ///< The component has a spatial offset value, i.e. `@150:220`
            Indirector       = 1<<4,    ///< The component specifies stepping into the identified node (i.e. `!`)
            TextQualifier    = 1<<5,    ///< The component contains a text qualifier, i.e. `[this]`
            
            SpatialTemporalOffset   = TemporalOffset|SpatialOffset, ///< Component contains both temporal and spatial offsets.
            OffsetsMask             = CharacterOffset|TemporalOffset|SpatialOffset,
        };
        
        ///
        /// A simple structure which defines a spatial location and can perform comparisons.
        struct Point
        {
            float x;
            float y;
            
            // impementations are out-of-line, since Xcode does weird things if operator < is defined inline
            bool    operator>(const Point& o)   const;
            bool    operator<(const Point& o)   const;
            bool    operator>=(const Point& o)  const;
            bool    operator<=(const Point& o)  const;
            bool    operator==(const Point& o)  const;
            bool    operator!=(const Point& o)  const;
                
        };
        
        uint8_t         flags;              ///< The bitfield containing values from the Flags enumeration.
        uint32_t        nodeIndex;          ///< The numeric index for the node identified by component.
        string          qualifier;          ///< The value of any `id` or `idref` qualifier.
        uint32_t        characterOffset;    ///< The value of any character offset.
        float           temporalOffset;     ///< The value, in seconds, of any temporal offset.
        Point           spatialOffset;      ///< The value of any spatial offset.
        string          textQualifier;      ///< The content of any character location qualifier.
        SideBias        sideBias;
        
        ////////////////////////////////////////////////////////////////////////////
        
        ///
        /// Creates a component from a string.
                        Component(const string& str);
        ///
        /// Creates a numeric component with no flags.
        Component(uint32_t __nodeIdx=0) : flags(0), nodeIndex(__nodeIdx), qualifier(), characterOffset(0), temporalOffset(), spatialOffset(), textQualifier(), sideBias(SideBias::Unspecified) {}
        ///
        /// Copy constructor.
                        Component(const Component& o) : flags(o.flags), nodeIndex(o.nodeIndex), qualifier(o.qualifier), characterOffset(o.characterOffset), temporalOffset(o.temporalOffset), spatialOffset(o.spatialOffset), textQualifier(o.textQualifier), sideBias(o.sideBias) {}
        ///
        /// Move constructor.
                        Component(Component&& o) : flags(o.flags), nodeIndex(o.nodeIndex), qualifier(std::move(o.qualifier)), characterOffset(o.characterOffset), temporalOffset(o.temporalOffset), spatialOffset(o.spatialOffset), textQualifier(std::move(o.textQualifier)), sideBias(o.sideBias) {}
                        ~Component() {};
        
        ///
        /// Compare conpoments for equality.
        bool            operator==(const Component& o)      const;
        bool            operator!=(const Component& o)      const;
        
        ///
        /// Assign a new value to a component.
        Component&      operator=(const Component& o);
        Component&      operator=(Component&& o);
        Component&      operator=(const string& str);
        
        bool            HasFlag(Flags flag)                 const _NOEXCEPT { return (flags & flag) == flag; }
        bool            HasQualifier()                      const _NOEXCEPT { return HasFlag(Qualifier); }
        bool            HasCharacterOffset()                const _NOEXCEPT { return HasFlag(CharacterOffset); }
        bool            HasTemporalOffset()                 const _NOEXCEPT { return HasFlag(TemporalOffset); }
        bool            HasSpatialOffset()                  const _NOEXCEPT { return HasFlag(SpatialOffset); }
        bool            IsIndirector()                      const _NOEXCEPT { return HasFlag(Indirector); }
        bool            HasTextQualifier()                  const _NOEXCEPT { return HasFlag(TextQualifier); }
        bool            HasSpatialTemporalOffset()          const _NOEXCEPT { return HasFlag(SpatialTemporalOffset); }
        
    private:
        void            Parse(const string& str);
    };
    
    ///
    /// Options bitfield for a CFI.
    enum Options : uint8_t
    {
        RangeTriplet        = 1<<0,     ///< The CFI represents a range, not a location.
    };
    
    typedef std::vector<Component>  ComponentList;
    
    ComponentList   _components;        ///< The CFI's discrete components.
    uint8_t         _options;           ///< CFI options, i.e. Options::RangeTriplet.
    
    ComponentList   _rangeStart;        ///< The components for the start of a range.
    ComponentList   _rangeEnd;          ///< The components for the end of a range.
    
    // PackageBase should be able to work with components
    friend class    PackageBase;
    friend class    Package;
    
    ///
    /// The total number of components in a CFI, including range components.
    size_t              TotalComponents()                   const;
    ///
    /// Generates a sub-path CFI from a given index.
    /// @see CFI(const CFI&, size_t)
    string              SubCFIFromIndex(size_t index)       const;
    ///
    /// Generates a string representation of the CFI between two components.
    string              Stringify(ComponentList::const_iterator start, ComponentList::const_iterator end)   const;
    
    ///
    /// Appends components to a string stream. Used by Stringify().
    static void         AppendComponents(std::stringstream& stream, ComponentList::const_iterator start, ComponentList::const_iterator end);
    
    typedef std::vector<string>    StringList;
    
    ///
    /// Breaks a CFI string into its components.
    static StringList   CFIComponentStrings(const string& cfi, const string& delimiter = string("/"));
    ///
    /// Breaks a ranged CFI string into base, start, and end CFI strings
    static StringList   RangedCFIComponents(const string& cfi)          { return CFIComponentStrings(cfi, ","); }
    ///
    /// Compiles CFI component strings into a component list.
    static bool         CompileComponentsToList(const StringList& strings, ComponentList* list);
    ///
    /// Top-level CFI compilation method.
    bool                CompileCFI(const string& str);
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__cfi__) */
