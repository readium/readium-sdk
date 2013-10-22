//
//  package.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-28.
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

#ifndef __ePub3__package__
#define __ePub3__package__

#include <ePub3/epub3.h>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <libxml/tree.h>
#include <ePub3/utilities/owned_by.h>
#include <ePub3/spine.h>
#include <ePub3/manifest.h>
#include <ePub3/cfi.h>
#include <ePub3/nav_element.h>
#include <ePub3/archive_xml.h>
#include <ePub3/utilities/utfstring.h>
#include <ePub3/utilities/iri.h>
#include <ePub3/content_handler.h>
#include <ePub3/media_support_info.h>
#include <ePub3/property_holder.h>
#include <ePub3/utilities/xml_identifiable.h>

EPUB3_BEGIN_NAMESPACE

class Archive;
class Metadata;
class NavigationTable;
class ByteStream;
class Container;
class PackageBase;
class Package;

typedef shared_ptr<Package>     PackagePtr;

/**
 The PackageBase class implements the low-level components and all storage of an OPF
 package document.  It provides direct access to spine, manifest, and metadata tables,
 while the Package class provides a higher-level API on top of these.
 
 @remarks The PackageBase class holds owning references for all Metadata, ContentHandlers,
 top-level NavigationTables, and ManifestItems. It also holds an owning reference to
 the first SpineItem in its spine; each SpineItem holds an owning reference to the
 SpineItem that follows it. Lastly, it a reference to the XML document for its
 source OPF file.
 
 @ingroup epub-model
 */
class PackageBase
{
public:
    ///
    /// A lookup table for navigation tables, indexed by type.
    typedef std::map<string, shared_ptr<NavigationTable>>   NavigationMap;
    ///
    /// An array of content handler objects.
    typedef shared_vector<ContentHandler>                   ContentHandlerList;
    ///
    /// A map of media-type to content-handler lists.
    typedef std::map<string, ContentHandlerList>            ContentHandlerMap;
    ///
    /// A list of property IRIs.
    typedef std::vector<IRI>                                PropertyIRIList;
    ///
    /// An XML-ID lookup table for relevant types
    typedef std::map<string, shared_ptr<XMLIdentifiable>>   XMLIDLookup;
    
private:
    /** There is no default constructor for PackageBase. */
                            PackageBase() _DELETED_;
    /** There is no copy constructor for PackageBase. */
                            PackageBase(const PackageBase&) _DELETED_;

protected:
    /**
     Constructs a new PackageBase object.
     
     The type, at present, is assumed to be `application/oebps-package+xml`; the
     parameter is here for future-proofing in case of alternative package types in
     later standards.
     @param owner The Container which owns this object.
     @param type The MIME type of the document, as read from the OCF `root-file`
     element.
     */
    EPUB3_EXPORT            PackageBase(const shared_ptr<Container>& owner, const string& type);
    /** C++11 'move' constructor-- claims ownership of its argument's internals. */
    EPUB3_EXPORT            PackageBase(PackageBase&&);
    
public:
    virtual                 ~PackageBase();
    
    /**
     Parses the contents of the package at the given location.
     @param path The container-relative path to the XML OPF file.
     @result Returns `true` if the package was parsed successfully, `false` otherwise.
     */
    virtual bool            Open(const string& path);
    
    /**
     Returns the path used to construct this object minus the filename, e.g.
     
     ~~~{.cpp}
     Package pkg(&myArchive, "EPUB/package.opf", theType);
     assert(pkg.BasePath() == "EPUB/");
     ~~~
     */
    virtual const string&   BasePath()              const       { return _pathBase; }
    
    /// @{
    /// @name Raw Table Accessors
    
    ///
    /// Returns an immutable reference to the manifest table.
    const ManifestTable&    Manifest()              const       { return _manifest; }
    ///
    /// Returns an immutable reference to the map of navigation tables.
    const NavigationMap&    NavigationTables()      const       { return _navigation; }
    
    /// @}
    
    /// @{
    /// @name Spine Accessors
    
    /**
     Returns the first item in the Spine.
     */
    shared_ptr<SpineItem>   FirstSpineItem()        const       { return _spine; }
    
    /**
     Locates a spine item by position.
     @param idx The zero-based position of the item to return.
     @result A pointer to the requested spine item, or `nullptr` if the index was
     out of bounds.
     */
    EPUB3_EXPORT
    shared_ptr<SpineItem>   SpineItemAt(size_t idx) const;

    EPUB3_EXPORT
    size_t                  IndexOfSpineItemWithIDRef(const string& idref)  const;
    
    /// @}
    
    /// @{
    /// @name Manifest Item Accessors
    
    /**
     Looks up and returns a specific manifest item by its unique identifier.
     @param ident The unique identifier for the item to retrieve.
     @result A pointer to the requested item, or `nullptr` if no such item exists.
     */
    EPUB3_EXPORT
    shared_ptr<ManifestItem>    ManifestItemWithID(const string& ident)         const;
    
    /**
     Generates the subpath part of a CFI used to locate a given manifest item.
     
     The subpath will contain the correct steps to locate the requested item through
     the spine.  This fragment will include an element qualifier, e.g.
     
     ~~~{.cpp}
     string s = pkg.CFISubpathForManifestItemWithID("item1");
     // returns "/6/4[item1]!"
     ~~~
     @param ident The unique identifier for the manifest item to locate.
     @result A string containing the part of an ePub3 CFI which leads to the
     *content* of the referenced item.  If the identified manifest item is not referenced by a spine item,
     an empty string will be returned.
     @remarks The CFI spec states that all items be accessed via the spine.  Is it
     allowed to have a CFI which directly references a manifest item in the case
     where it's not referenced by a spine item?
     */
    EPUB3_EXPORT
    string                  CFISubpathForManifestItemWithID(const string& ident) const;
    
    /**
     Get an array of manifest items which have *all* the given properties.
     @param A vector of manifest `<item>` property names, e.g. `"nav"`, `"cover"`, etc.
     @result A vector containing pointers to any matching manifest items.
     */
    EPUB3_EXPORT
    const shared_vector<ManifestItem> ManifestItemsWithProperties(PropertyIRIList properties) const;
    
    /// @}
    
    /**
     Returns a navigation table identified by type.
     @param type An `epub:type` attribute value, such as `"toc"` or `"lot"`.
     @result A pointer to the relevant navigation table, or `nullptr` if no such
     table was found.
     */
    EPUB3_EXPORT
    shared_ptr<NavigationTable> NavigationTable(const string& type)            const;
    
    /**
     Returns a ByteStream for reading from the specified file in the package's Archive.
     @param path The path of the item to read.
     @result An auto-pointer to a new ByteStream instance.
     @ingroup utilities
     */
    EPUB3_EXPORT
    unique_ptr<ByteStream>        ReadStreamForItemAtPath(const string& path)                         const;
    
    /// Returns the CFI node index for the `<spine>` element within the package
    /// document.
    uint32_t                SpineCFIIndex()                 const   { return _spineCFIIndex; }
    
protected:
    shared_ptr<Archive>     _archive;           ///< The archive from which the package was loaded.
    xmlDocPtr               _opf;               ///< The XML document representing the package.
    string                  _pathBase;          ///< The base path of the document within the archive.
    string                  _type;              ///< The MIME type of the package document.
    ManifestTable           _manifest;          ///< All manifest items, indexed by unique identifier.
    NavigationMap           _navigation;        ///< All navigation tables, indexed by type.
    ContentHandlerMap       _contentHandlers;   ///< All installed content handlers, indexed by media-type.
    shared_ptr<SpineItem>   _spine;             ///< The first item in the spine (SpineItems are a linked list).
    XMLIDLookup             _xmlIDLookup;       ///< Lookup table for all items with XML ID values.
    
    // used to verify/correct CFIs
    uint32_t                _spineCFIIndex;     ///< The CFI index for the `<spine>` element in the package document.
    
    ///
    /// Unpacks the _opf document. Implemented by the subclass, to make PackageBase pure-virtual.
    virtual bool            Unpack() = 0;
    
    /**
     Locates a spine item based on the corresponding CFI component.
     
     May update the supplied CFI component following target correction.
     */
    shared_ptr<SpineItem>   ConfirmOrCorrectSpineItemQualifier(shared_ptr<SpineItem> pItem, CFI::Component* pComponent) const;
    
    ///
    /// Loads navigation tables from a given manifest item (which has the `"nav"` property).
    static NavigationList   NavTablesFromManifestItem(shared_ptr<PackageBase> owner, shared_ptr<ManifestItem> pItem);

#if EPUB_COMPILER_SUPPORTS(CXX_DEFAULT_TEMPLATE_ARGS)
    template <class _Tp, class = typename std::enable_if
                            <
                                std::is_base_of<XMLIdentifiable, _Tp>::value
                            >::type>
    void                            StoreXMLIdentifiable(shared_ptr<_Tp> ptr)
#else
    template <class _Tp>
    void                    StoreXMLIdentifiable(shared_ptr<_Tp> ptr,
        typename std::enable_if<std::is_base_of<XMLIdentifiable, _Tp>::value, void>::type ** = 0)
#endif
    {
        if ( !ptr->XMLIdentifier().empty() )
        {
#if EPUB_HAVE(CXX_MAP_EMPLACE)
            _xmlIDLookup.emplace(ptr->XMLIdentifier(), ptr);
#else
            _xmlIDLookup[ptr->XMLIdentifier()] = ptr;
#endif
        }
    }

    shared_ptr<Archive> Archive() const { return _archive; }
};

/**
 The Package class implements a high-level API for interacting with OPF packages,
 including convenience accessors for well-known metadata items.
 
 @remarks A Package instance holds owning references to all LoadEventHandlers and
 MediaSupportInfo objects attached to it.
 
 @see PackageBase for other important memory ownership information.
 
 @ingroup epub-model
 */
class Package : public PackageBase, public std::enable_shared_from_this<Package>, public PropertyHolder, public OwnedBy<Container>
{
public:
    /**
     Callback type for load events.
     
     The Package object just calls this function when a URL should be loaded-- the
     function should handle all the actual loading/display mechanism itself.
     @param url The url of the item to load.
     */
    typedef std::function<void(const IRI& url)>     LoadEventHandler;
    
    /**
     A list of media types (i.e. MIME types) used in the package's manifest.
     
     Each type is paired with an instance of MediaSupportInfo which describes the
     support for that media type.
     */
    typedef std::map<string, MediaSupportInfo>      MediaSupportList;
    
private:
                            Package()                                   _DELETED_;
                            Package(const Package&)                     _DELETED_;

public:
    EPUB3_EXPORT            Package(const shared_ptr<Container>& owner, const string& type);
                            Package(Package&& o) : OwnedBy(std::move(o)), PackageBase(std::move(o)) {}
    virtual                 ~Package() {}
    
    virtual bool            Open(const string& path);
    bool                    _OpenForTest(xmlDocPtr doc, const string& basePath);
    
    ///
    /// The full Unique Identifier, built from the package unique-id and the modification date.
    virtual string          UniqueID()              const;
    ///
    /// A version of the UniqueID which is suitable for use as a hostname.
    virtual string          URLSafeUniqueID()       const;
    ///
    /// The package's unique-id on its own, without the revision modifier.
    virtual string          PackageID()             const;
    ///
    /// MIME type of this package document (usually `application/oebps-package+xml`).
    virtual const string&   Type()                  const       { return _type; }
    ///
    /// OPF version of this package document.
    virtual string          Version()               const;
    
    /// @{
    /// @name Event/Content Handlers
    
    /**
     Attaches a handler for load-content events.
     @param handler A function to be called with the URL of each item loaded.
     */
    virtual void            SetLoadHandler(LoadEventHandler handler) { _loadEventHandler = handler; }
    
    /**
     Fires a load-content event for a given URL, calling the handler passed to SetLoadHandler.
     @param url The URL of the item being loaded.
     */
    virtual void            FireLoadEvent(const IRI& url) const;
    
    /**
     Adds a new handler for a particular (foreign) media type.
     
     Assumes ownership of the input ContentHandler pointer.
     @param handler A ContentHandler instance.
     */
    virtual void            AddMediaHandler(shared_ptr<ContentHandler> handler) { _contentHandlers[handler->MediaType()].push_back(handler); }
    
    /// @}
    
    /// @{
    /// @name Spine, Manifest, and CFI
    
    /**
     Returns the SpineItem having a given IDRef.
     @param idref The IDRef for which to search.
     @result A pointer to the located SpineItem, or `nullptr` if none was found.
     */
    EPUB3_EXPORT
    shared_ptr<SpineItem>   SpineItemWithIDRef(const string& idref)         const;
    
    /**
     Creates a CFI which locates a given ManifestItem.
     @param item A pointer to the ManifestItem to locate.
     @result A new CFI, as specific as possible, for the input ManifestItem.
     */
    EPUB3_EXPORT
    const CFI               CFIForManifestItem(shared_ptr<ManifestItem> item)    const;
    
    /**
     Creates a CFI which locates a given SpineItem.
     @param item A pointer to the SpineItem to locate.
     @result A new CFI, as specific as possible, for the input SpineItem.
     */
    EPUB3_EXPORT
    const CFI               CFIForSpineItem(shared_ptr<SpineItem> item)          const;
    
    // note that the CFI is purposely non-const so the package can correct it (cf. epub-cfi §3.5)
    /**
     Obtains the ManifestItem referenced by a given CFI.
     
     This method will fix the input CFI if necessary/possible, based on any qualifiers
     included in the CFI.
     @param cfi The CFI whose corresponding ManifestItem to locate. This may be altered
        for correctness if its qualifiers do not match the target item.
     @param pRemainingCFI A pointer to a CFI instance whose value will be set to the
        fragment of `cfi` which refers to the content of the document referenced by
        the returned ManifestItem, i.e. a document-relative locator. If no fragment
        remains (`cfi` referred only to the top-level document) then it will be set
        to the empty CFI.
     @result The ManifestItem corresponding to the input CFI, or `nullptr` otherwise.
     */
    EPUB3_EXPORT
    shared_ptr<ManifestItem>    ManifestItemForCFI(CFI& cfi, CFI* pRemainingCFI) const;
    
    /**
     A convenience method used to obtain a libxml2 `xmlDocPtr` from a CFI.
     
     This method calls ManifestItemForCFI() internally, so will fix the input
     CFI based on any qualifiers.
     @param cfi The CFI whose corresponding ManifestItem to locate. This may be altered
     for correctness if its qualifiers do not match the target item.
     @param pRemainingCFI A pointer to a CFI instance whose value will be set to the
     fragment of `cfi` which refers to the content of the document referenced by
     the returned ManifestItem, i.e. a document-relative locator. If no fragment
     remains (`cfi` referred only to the top-level document) then it will be set
     to the empty CFI.
     @result An `xmlDocPtr` for the selected document, or `nullptr` upon failure.
     */
    xmlDocPtr               DocumentForCFI(CFI& cfi, CFI* pRemainingCFI) const {
        return ManifestItemForCFI(cfi, pRemainingCFI)->ReferencedDocument();
    }
    
    /**
     Obtains an element from the spine by index.
     @param idx The (zero-based) index of the spine item to retrieve.
     @result The SpineItem at the supplied index, or `nullptr` if the index was out
     of bounds.
     */
    shared_ptr<SpineItem>   operator[](size_t idx)          const   { return SpineItemAt(idx); }
    
    /**
     Obtains the ManifestItem with a given identifier.
     @param ident The identifier of the ManifestItem to retrieve.
     @result The ManifestItem with the given identifier, or `nullptr` if no item
     had that identifier.
     */
    shared_ptr<ManifestItem>    operator[](const string& ident) const   { return ManifestItemWithID(ident); }
    
    /// @}
    
    /// @{
    /// @name Raw Data Access
    
    unique_ptr<ArchiveReader>   ReaderForRelativePath(const string& path)       const {
        return _archive->ReaderAtPath((_pathBase + path).stl_str());
    }
    unique_ptr<ArchiveXmlReader>    XmlReaderForRelativePath(const string& path)    const {
        return unique_ptr<ArchiveXmlReader>(new ArchiveXmlReader(ReaderForRelativePath(path)));
    }
    EPUB3_EXPORT
    unique_ptr<ByteStream>        ReadStreamForRelativePath(const string& path)   const;
    
    /// @}
    
    /// @{
    /// @name Navigation Tables
    
    ///
    /// Returns the table of contents for this package.
    shared_ptr<class NavigationTable>   TableOfContents()       const       { return NavigationTable("toc"); }
    ///
    /// Return the list of figures, if any exists.
    shared_ptr<class NavigationTable>   ListOfFigures()         const       { return NavigationTable("lof"); }
    ///
    /// Returns the list of illustrations, if any exists.
    shared_ptr<class NavigationTable>   ListOfIllustrations()   const       { return NavigationTable("loi"); }
    ///
    /// Returns the list of tables, if any exists.
    shared_ptr<class NavigationTable>   ListOfTables()          const       { return NavigationTable("lot"); }
    ///
    /// Returns the page list, if any exists.
    shared_ptr<class NavigationTable>   PageList()              const       { return NavigationTable("page-list"); }
    
    /// @}
    
    /// @{
    /// @name High-Level Metadata API
    
    /**
     Retrieves the title of the publication.
     @param localized Set to `true` (the default) to obtain a localized value if
     one is available. The localization to use is determined by calling
     PackageBase::Locale().
     @result The title of the publication.
     */
    EPUB3_EXPORT
    const string&           Title(bool localized=true)              const;
    
    /**
     Retrieves the subtitle of the publication.
     @param localized Set to `true` (the default) to obtain a localized value if
     one is available. The localization to use is determined by calling
     PackageBase::Locale().
     @result The subtitle of the publication.
     */
    EPUB3_EXPORT
    const string&           Subtitle(bool localized=true)           const;
    
    /**
     Retrieves the short-form title of the publication.
     @param localized Set to `true` (the default) to obtain a localized value if
     one is available. The localization to use is determined by calling
     PackageBase::Locale().
     @result The short-form title of the publication.
     */
    EPUB3_EXPORT
    const string&           ShortTitle(bool localized=true)         const;
    
    /**
     Retrieves the collection title of the publication.
     @param localized Set to `true` (the default) to obtain a localized value if
     one is available. The localization to use is determined by calling
     PackageBase::Locale().
     @result The collection title of the publication.
     */
    EPUB3_EXPORT
    const string&           CollectionTitle(bool localized=true)    const;
    
    /**
     Retrieves the edition title of the publication.
     @param localized Set to `true` (the default) to obtain a localized value if
     one is available. The localization to use is determined by calling
     PackageBase::Locale().
     @result The edition title of the publication.
     */
    EPUB3_EXPORT
    const string&           EditionTitle(bool localized=true)       const;
    
    /**
     Retrieves the expanded title of the publication.
     @param localized Set to `true` (the default) to obtain a localized value if
     one is available. The localization to use is determined by calling
     PackageBase::Locale().
     @result The expanded title of the publication.
     */
    EPUB3_EXPORT
    const string&           ExpandedTitle(bool localized=true)      const;
    
    /**
     Retrieves the complete title of the publication.
     
     This will return the author-defined expanded title if available, otherwise it
     will construct a title from either:
     
     * All available titles ordered according to their 'display-seq' value.
     * All available titles in document order.
     @param localized Set to `true` (the default) to obtain a localized value if
     one is available. The localization to use is determined by calling
     PackageBase::Locale().
     @result The complete title of the publication.
     */
    EPUB3_EXPORT
    const string            FullTitle(bool localized=true)          const;
    
    ///
    /// A simple type which lists the names of a publication's creators.
    typedef std::vector<string>                 AttributionList;
    
    /**
     Retrieves the names of all authors/creators credited for this publication.
     @param localized Set to `true` (the default) to obtain localized values if
     available. The localization to use is determined by calling
     PackageBase::Locale().
     @result A list of authors, each name suitable for display.
     */
    EPUB3_EXPORT
    const AttributionList   AuthorNames(bool localized=true)        const;
    
    /**
     Retrieves the names of all authors/creators in sortable format.
     
     If a `file-as` refinement of an author/creator name is not available, that name
     will be returned in display format, i.e. the library will not attempt to
     synthesize a `file-as` value from the display name.
     @param localized Set to `true` (the default) to obtain localized values if
     available. The localization to use is determined by calling
     PackageBase::Locale().
     @result A list of authors, suitable for sorting.
     */
    EPUB3_EXPORT
    const AttributionList   AttributionNames(bool localized=true)   const;
    
    /**
     Retrieves a display-ready string listing all authors.
     @param localized Set to `true` (the default) to obtain a localized value if
     one is available. The localization to use is determined by calling
     PackageBase::Locale().
     @result A list of authors, collated and ready for display.
     */
    EPUB3_EXPORT
    const string            Authors(bool localized=true)            const;
    
    /**
     Retrieves a list of all contributors (authors, editors, committee members, etc.)
     @param localized Set to `true` (the default) to obtain localized values if
     available. The localization to use is determined by calling
     PackageBase::Locale().
     @result A list of contributors, each in display format.
     */
    EPUB3_EXPORT
    const AttributionList   ContributorNames(bool localized=true)   const;
    
    /**
     Retrieves a display-ready string listing all contributors.
     @param localized Set to `true` (the default) to obtain a localized value if
     one is available. The localization to use is determined by calling
     PackageBase::Locale().
     @result A list of contributors, collated and ready for display.
     */
    EPUB3_EXPORT
    const string            Contributors(bool localized=true)       const;
    
    /**
     Retrieves the language of the publication, if available.
     @result The publication's original language.
     */
    EPUB3_EXPORT
    const string&           Language()                              const;
    
    /**
     Retrieves the source of the publication, if available.
     @param localized Set to `true` (the default) to obtain a localized value if
     one is available. The localization to use is determined by calling
     PackageBase::Locale().
     @result The publication's original source.
     */
    EPUB3_EXPORT
    const string&           Source(bool localized=true)             const;
    
    /**
     Retrieves the publication's copyright information.
     @param localized Set to `true` (the default) to obtain a localized value if
     one is available. The localization to use is determined by calling
     PackageBase::Locale().
     @result The publication's copyright ownership/assignment statement.
     */
    EPUB3_EXPORT
    const string&           CopyrightOwner(bool localized=true)     const;
    
    /**
     Retrieves a string indicating the last modification date of this package.
     @result The package's modification date, if specified.
     */
    EPUB3_EXPORT
    const string&           ModificationDate()                      const;
    
    /**
     Returns the publication's ISBN number, if available.
     @result An ISBN, or the empty string if none is specified.
     */
    EPUB3_EXPORT
    const string            ISBN()                                  const;
    
    typedef std::vector<string>                     StringList;
    
    /**
     Retrieves a list of the publication's subjects.
     @param localized Set to `true` (the default) to obtain localized values if
     available. The localization to use is determined by calling
     PackageBase::Locale().
     @result The publication's subjects.
     */
    EPUB3_EXPORT
    const StringList        Subjects(bool localized=true)           const;
    
    /**
     Retrieves the page progression direction for the publication.
     @result A PageProgressionDirection value.
     */
    EPUB3_EXPORT
    PageProgression         PageProgressionDirection()              const;
    
    /// @}
    
    /// @{
    /// @name Media Handling
    
    ///
    /// A list of media types which have an installed handler of class MediaHandler.
    EPUB3_EXPORT
    const StringList            MediaTypesWithDHTMLHandlers()                   const;
    
    /**
     Obtains all installed handlers for a particular media type.
     @param mediaType The media-type whose handler list to retrieve.
     @result A list of installed handlers for this media type.
     */
    EPUB3_EXPORT
    const ContentHandlerList    HandlersForMediaType(const string& mediaType)   const;
    
    /**
     Retrieves the handler that will be used for a certain media type.
     @param mediaType The media-type whose handler to retrieve.
     @result The handler for this media type.
     */
    EPUB3_EXPORT
    shared_ptr<MediaHandler>    OPFHandlerForMediaType(const string& mediaType) const;
    
    ///
    /// Returns a list of all media types seen in the manifest.
    EPUB3_EXPORT
    const StringList        AllMediaTypes()                 const;
    ///
    /// Returns a list of all unsupported media types.
    EPUB3_EXPORT
    const StringList        UnsupportedMediaTypes()         const;
    
    /**
     The package's current media support list.
     
     The package loading system will create this based on the known core types and
     any DHTML media handlers defined in the package itself.
     @result The package's media support information.
     */
    const MediaSupportList& MediaSupport()                  const       { return _mediaSupport; }
    
    /**
     The package's current media support list, supports editing in-place.
     
     The package loading system will create this based on the known core types and
     any DHTML media handlers defined in the package itself.
     @result The package's media support information.
     */
    MediaSupportList&       MediaSupport()                              { return _mediaSupport; }
    
    /**
     Sets the media support information for the package.
     @param list A list of MediaSupportInfo objects detailing support for each
     media type defined in the manifest. This may contain information for types not
     present in this package's manifest, for convenience.
     */
    virtual void            SetMediaSupport(const MediaSupportList& list);
    
    /**
     Sets the media support information for the package.
     @param list A list of MediaSupportInfo objects detailing support for each
     media type defined in the manifest. This may contain information for types not
     present in this package's manifest, for convenience.
     */
    virtual void            SetMediaSupport(MediaSupportList&& list);
    
    /// @}
    
protected:
    ///
    /// Extracts information from the OPF XML document.
    virtual bool            Unpack();
    ///
    /// Used to handle the `prefix` attribute of the OPF `<package>` element.
    void                    InstallPrefixesFromAttributeValue(const string& attrValue);
    
    // default is `true`
    EPUB3_EXPORT
    static bool             gValidateSchema;
    
public:
    ///
    /// Whether the XML parser will validate an OPF file against its schema (default is `true`).
    static bool             ValidatesSchema()                   { return gValidateSchema; }
    ///
    /// Enable or disable OPF schema validation.
    static void             SetValidatesSchema(bool validate)   { gValidateSchema = validate; }
    
protected:
    LoadEventHandler        _loadEventHandler;      ///< The current handler for load events.
    MediaSupportList        _mediaSupport;          ///< A list of media types with their support details.
    
    void                    InitMediaSupport();
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__package__) */
