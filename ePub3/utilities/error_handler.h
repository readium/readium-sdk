//
//  error_handler.h
//  ePub3
//
//  Created by Jim Dovey on 2013-05-14.
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

#ifndef __ePub3__error_handler__
#define __ePub3__error_handler__

#include <ePub3/utilities/basic.h>
#include <array>
#include <system_error>
#include <functional>

EPUB3_BEGIN_NAMESPACE

class error_details;

/**
 Error handler function type.
 @param err The error code being raised.
 @result Return `true` to continue, ignoring the error, `false` to throw the error.
 */
typedef std::function<bool(const error_details& err)>   ErrorHandlerFn;

///
/// The default error handler. Always returns `false` except for Critical spec violations.
EPUB3_EXPORT
bool            DefaultErrorHandler(const error_details& err);

///
/// Retrieves the current error handler function.
EPUB3_EXPORT
ErrorHandlerFn  ErrorHandler();

///
/// Sets the current error hander function.
EPUB3_EXPORT
void            SetErrorHandler(ErrorHandlerFn fn);

enum class EPUBSpec
{
    OpenContainerFormat,                        // 0x00
    OpenPublicationFormat,                      // 0x01
    ContentDocuments,                           // 0x02
    MediaOverlays,                              // 0x03
    CanonicalFragmentIdentifiers,           

	UnknownSpec,
    NUM_SPECS = UnknownSpec
};

enum class ViolationSeverity
{
    Minor,                                      ///< Does not conform to a SHOULD specification.
    Medium,                                     ///< Does not conform to a MUST specification, but is usually recoverable.
    Major,                                      ///< Does not conform to a MUST specification, and will likely cause unexpected behaviour.
    Critical                                    ///< Will cause the EPUB to be non-functional.
};

extern EPUB3_EXPORT std::array<std::string, static_cast<std::size_t>(EPUBSpec::NUM_SPECS)> EPUBSpecNames;
extern EPUB3_EXPORT std::array<std::string, static_cast<std::size_t>(EPUBSpec::NUM_SPECS)> EPUBSpecURLs;

////////////////////////////////////////////////////////////////////////////////////
//
// PLEASE NOTE
//
// The layout of the enumeration below is VERY IMPORTANT, as it will be used to
// update the contents of the gErrorInfoTable variable in error_handler.cpp.
//
// That tool is written in Go (UTF-8 built-ins and lovely APIs, yeah!), and can be
// found in $(project-root)/BuildTools/src/BuildErrorTable.go.
//
////////////////////////////////////////////////////////////////////////////////////
enum class EPUBError
{
    NoError,                                ///< No error condition was detected.
    
    ////////////////////////////////////////////////////////////////////////////////
    // Open Container Format 3.0
    // EPUBSpec::OpenContainerFormat
    
    OCFErrorBase                            = 100,
    
    // ?? 1.2
    OCFResourceNotInManifest,               ///< All resources accessed by a publication rendition MUST be listed in its manifest. Major.
    
    // ?? 2.2
    OCFNoMetaInfDirectory,                  ///< Critical. All containers MUST have a META-INF directory. Critical.
    
    // ?? 2.3
    OCFInvalidRelativeIRI,                  ///< Medium. IRIs should all be relative; META-INF contents are relative to root, not self. Medium.
    
    // ?? 2.4
    OCFInvalidFileNameCharacter,            ///< File names MUST use UTF-8 encoding and avoid some Unicode code points. Medium.
    
    // ?? 2.5.1
    OCFNoContainerFile,                     ///< Containers MUST have a META-INF/container.xml file. Critical.
    OCFNoRootfilesInContainer,              ///< 'container.xml' MUST contain <rootfiles> element with at least one <rootfile>. Critical.
    OCFNonRelativeRootfileURL,              ///< A <rootfile> element's 'full-path' attribute MUST be a relative IRI. Medium.
    OCFInvalidRootfileURL,                  ///< The URL to a package file does not identify a valid resource. Critical.
    
    // ?? 2.5.2
    OCFInvalidEncryptionFile,               ///< 'META-INF/encryption.xml' MUST be valid according to XML-ENC 1.1 schema. Major.
    OCFEncryptedFileStoredCompressed,       ///< Encrypted files SHOULD be compressed, then encrypted, then stored, not encrypted then stored-as-compressed. Minor.
    OCFInvalidEncryptedFile,                ///< The 'mimetype' file, contents of the META-INF directory, and package documents MUST NOT be encrypted. Medium.
    
    // ?? 2.5.6
    OCFInvalidSignatureFile,                ///< 'META-INF/signature.xml' MUST be valid according to XML-DSig 1.1 schema. Major.
    
    // ?? 3.2
    OCFMultiPartZip,                        ///< An OCF container MUST NOT be split across multiple ZIP archives. Critical.
    OCFNonDeflateCompression,               ///< A container's files MUST only use STORE or DEFLATE compression, values 0 or 8. Medium.
    OCFZipEncryptionEncountered,            ///< A container MUST NOT use ZIP encryption. Medium.
    OCFNonUTF8FileNames,                    ///< A container MUST use UTF-8 for all file names. Minor.
    OCFInvalidZipVersion,                   ///< A container's ZIP archive version MUST be 10, 20 (for Deflate), or 45 (for ZIP64). Medium.
    OCFInvalidZipHeader,                    ///< A container MUST NOT have a ZIP Decryption Header or Archive Extra Data Record. Medium.
    
    // ?? 3.3
    OCFMimetypeFileNotFound,                ///< A container MUST have a 'mimetype' file. Major.
    OCFMimetypeLocationIncorrect,           ///< A container's 'mimetype' file MUST be the first item in the archive. Medium.
    OCFMimetypeStorageIncorrect,            ///< A container's 'mimetype' file MUST NOT be compressed or encrypted. Medium.
    OCFMimetypeContentIncorrect,            ///< A container's 'mimetype' file MUST contain 'application/epub+zip' in UTF-8 with no padding. Medium.
    OCFMimetypeHasExtraHeaderField,         ///< A container's 'mimetype' file MUST NOT have an extra field in its ZIP header. Medium.
    
    // ?? 4.4
    OCFFontEncryptedIllegally,              ///< Fonts MUST be encrypted only using Font Obfuscation as per OCF section 4.2. Medium.
    
    OCFErrorMax,
    
    
    ////////////////////////////////////////////////////////////////////////////////
    // Open Package Format 3.0
    // EPUBSpec::OpenPublicationFormat
    
    OPFErrorBase                            = 200,
    
    // ?? 2.1
    OPFNoNavDocument,                       ///< Publications MUST contain exactly one navigation document. Major.
    OPFMultipleNavDocuments,                ///< Publications MUST contain exactly one navigation document. Major.
    
    // ?? 3.2
    OPFInvalidPackageDocument,              ///< Package documents MUST validate according to OPF schema. Major.
    OPFInvalidPackageDocumentExtension,     ///< Package documents SHOULD have a .opf extension. Minor.
    
    // ?? 3.4.1
    OPFPackageHasNoVersion,                 ///< <package> element MUST have version 3.0 (or 2.x for backward compatibility). Medium.
    
    // ?? 3.4.2
    OPFNoMetadata,                          ///< Package MUST contain a <metadata> element as the first child of <package>. Critical.
    OPFMetadataOutOfOrder,                  ///< Package <metadata> element MUST be the first child of <package>. Medium.
    OPFMissingIdentifierMetadata,           ///< Package metadata MUST include 'dc:identifier'. Critical.
    OPFMissingTitleMetadata,                ///< Package metadata MUST include 'dc:title'. Major.
    OPFMissingLanguageMetadata,             ///< Package metadata MUST include 'dc:language'. Major.
    OPFInvalidRefinementAttribute,          ///< A metadata 'refine' attribute MUST be a valid relative IRI.
    OPFInvalidRefinementTarget,             ///< A metadata 'refine' attribute MUST reference an existing item.
    
    // ?? 3.4.9
    OPFLinkReferencesManifestItem,          ///< <link> 'href' attribute MUST NOT identify an object in the manifest. Medium.
    OPFLinkMissingHref,                     ///< <link> 'href' attribute is empty or not present. Medium.
    
    // ?? 3.4.10
    OPFNoManifest,                          ///< Package MUST contain a <manifest> element as the second child of <package>. Critical.
    OPFManifestOutOfOrder,                  ///< Package <manifest> element MUST be second child of <package>. Medium.
    OPFNoManifestItems,                     ///< <manifest> MUST contain at least one <item> element. Major.
    
    // ?? 3.4.12
    OPFNoSpine,                             ///< Package MUST contain a <spine> element as the third child of <package>. Critical.
    OPFSpineOutOfOrder,                     ///< Package <spine> element MUST be third child of <package>. Medium.
    OPFNoSpineItems,                        ///< <spine> MUST contain at least one <itemref> element. Critical.
    OPFNoPrimarySpineItems,                 ///< <spine> MUST contain at least one primari <itemref> element (linear=yes). Major.
    
    // ?? 3.4.13
    OPFMissingSpineIdref,                   ///< Spine <itemref> MUST have an idref attribute. Critical.
    OPFInvalidSpineIdref,                   ///< Spine <itemref> idref attribute MUST reference an <item> in the manifest. Critical.
    OPFSpineTargetNoContentDocument,        ///< Spine <itemref> targets MUST include an EPUB Content Document in their fallback chain, EVEN IF the targetted manifest item's resource is a Core Media Type. Major.
    
    // ?? 3.4.16
    OPFMultipleBindingsForMediaType,        ///< Each <mediaType> element within <bindings> MUST reference a unique media-type. Major.
    OPFCoreMediaTypeBindingEncountered,     ///< <mediaType> elements MUST NOT reference a Core Media Type. Major.
    OPFBindingHandlerNotFound,              ///< <mediaType> elements' handler attribute MUST reference an item in the <manifest>. Major.
    OPFBindingHandlerInvalidType,           ///< <mediaType> handler resources MUST be XHTML content documents. Critical.
    OPFBindingHandlerNotScripted,           ///< <mediaType> handler resources MUST have the scripted property. Major.
    OPFBindingHandlerNoMediaType,           ///< <mediaType> elements MUST have a 'media-type' attribute. Critical.
    
    // $ 3.4.17
    OPFCollectionMissingRole,               ///< A <collection> element MUST have a 'role' attribute. Major.
    OPFCollectionMetadataOutOfOrder,        ///< A <metadata> element within a <collection> MUST be the first child element of its parent <collection>. Minor.
    OPFCollectionSubcollectionOutOfOrder,   ///< A <collection> within another <collection> MUST be placed before any <link> child elements. Minor.
    OPFCollectionNoLinks,                   ///< A <collection> element MUST have one or more <link> child elements. Major.
    OPFCollectionRoleInvalid,               ///< A <collection>'s 'role' attribute MUST be either a full IRI or an NMTOKEN which resolves to an IRI known to the IDPF Collection Role registry. Medium.
    OPFCollectionRefinesInvalid,            ///< The 'refines' attribute MUST NOT reference an item outside the collection. Major.
    OPFCollectionIllegalMetaElement,        ///< The OPF2 <meta> element MUST NOT be used. Minor.
    OPFCollectionLinkIncludesRefinement,    ///< The 'refines' attribute MUST NOT be attached to a <link> within a <collection>. Medium.
    
    // ?? 4.1.1
    OPFPackageUniqueIDInvalid,              ///< The <package> tag's unique-identifier attribute MUST reference a <dc:identifier> element in the package's <metadata>. Major.
    
    // ?? 4.1.2
    OPFMissingModificationDateMetadata,     ///< <package> metadata MUST include a dcterms:modified metadata element. Major.
    OPFModificationDateInvalid,             ///< 'dcterms:modified' property value MUST be an xml schema dateTime of the form CCYY-MM-DD'T'hh:mm:ss'Z' (represented here in Unicode date-time format string syntax). Medium.
    
    // ?? 4.2.3
    OPFIllegalPrefixRedeclaration,          ///< A Reserved Vocabulary member's prefix MUST NOT be re-declared in a prefix attribute. Major.
    OPFIllegalVocabularyIRIRedefinition,    ///< A Reserved Vocabulary member's IRI MUST NOT be assigned to another prefix. Major.
    OPFIllegalPrefixDefinition,             ///< The prefix '_' is reserved for future compatibility with RDFa so MUST NOT be defined. Major.
    
    // ?? 4.3.3
    OPFIllegalLinkRelValue,                 ///< <link> rel attributes 'marc21xml-record', 'mods-record', 'onix-record', 'xml-signature', 'xmp-record' MUST NOT be used when the refines attribute is present. Major.
    
    // ?? 4.3.4
    OPFMultipleCoverImageItems,             ///< A <manifest> MUST NOT contain more than one item with the 'cover-image' attribute. Major.
    
    // ?? 4.3.5
    OPFConflictingSpineItemSpreads,         ///< A spine <itemref> MUST NOT contain both the 'page-spread-left' and 'page-spread-right' properties. Major.
    
    // ?? 5.2.2
    OPFNoFallbackForForeignMediaType,       ///< Content Documents referenced from the <spine> that are not Core Media types MUST have a Core Media fallback. Major.
    OPFInvalidManifestFallbackRef,          ///< A manifest item's fallback attribute MUST provide the identifier of a valid, different manifest item. Critical.
    OPFFallbackChainHasNoContentDocument,   ///< A fallback chain MUST contain a valid EPUB Content Document. Medium.
    OPFFallbackChainCircularReference,      ///< A fallback chain MUST NOT contain circular references. Critical.
    
    // ?? 5.4
    OPFNonConformantXMLResource,            ///< Resources that are XML-based Media Types MUST conform to XML 1.0 and XML-Namespaces. Major.
    OPFExternalIdentifiersInXMLResource,    ///< XML-based resources MUST NOT have external identifiers in their Document Type Definitions. Medium.
    OPFXMLResourceUsesXInclude,             ///< XML-based resources MUST NOT make use of XInclude. Major.
    OPFXMLResourceInvalidEncoding,          ///< XML-based resources MUST be encoded in UTF-8 or UTF-16. Major.
    
    OPFErrorMax,
    
    
    ////////////////////////////////////////////////////////////////////////////////
    // Content Documents 3.0
    // EPUBSpec::ContentDocuments
    
    ContentDocumentErrorBase                = 300,
    
    // ?? 1.2
    InvalidXHTML5Document,                  ///< XHTML Content Documents MUST conform to XHTML/HTML5 specifications. Major.
    XHTMLDocumentIncorrectExtension,        ///< XHTML Content Documents SHOULD use the .xhtml filename extension. Minor.
    
    // ?? 2.2.4.1
    NavElementHasNoType,                    ///< Top-level <nav> elements in a Navigation Document MUST contain an epub:type attribute. Major.
    NavElementUnexpectedType,               ///< The epub:type designates an incorrect value. Medium.
    NavElementInvalidChildren,              ///< <nav> elements in a Navigation Document MUST only contain <hgroup>, <h1..6> and <ol> elements as direct descendants. Medium.
    NavElementInvalidChildOrder,            ///< <h...> elements within <nav> elements in a Navigation Document MUST occur only once, and as the first child. Medium.
    NavListElementInvalidChild,             ///< Navigation Document <li> elements MUST contain one <a> element and an optional <ol> element, OR contain one <span> element and one required <ol> element. Medium.
    
    // ?? 2.2.4.2.1
    NavNoTOCFound,                          ///< The 'toc' epub:type MUST occur on EXACTLY ONE <nav> element in the publication. Major.
    NavMultipleTOCsEncountered,             ///< The 'toc' epub:type MUST occur on EXACTLY ONE <nav> element in the publication. Major.
    
    // ?? 2.2.4.2.2
    NavMultiplePageListsEncountered,        ///< A publication MUST have no more than one <nav> element with an epub:type of 'page-list'. Major.
    
    // ?? 2.2.4.2.3
    NavMultipleLandmarksEncountered,        ///< A publication MUST have no more than one <nav> element with an epub:type of 'landmarks'. Major.
    
    // ?? 2.2.4.2.4
    NavTableHasNoTitle,                     ///< Navigation tables other than 'toc', 'page-list', and 'landmarks' MUST have a title as the first child of the <nav> element. Major.
    
    // ?? 2.3.3
    SVGContainsAnimations,                  ///< SVG Animation Elements and Animation Event Attributes MUST NOT occur. Medium.
    SVGInvalidForeignObjectContent,         ///< SVG <foreignObject> elements MUST contain only valid XHTML Content Document Flow content, and its requiredExtensions attribute, if given, MUST be set to 'http://www.idpf.org/2007/ops'. Medium.
    SVGInvalidTitle,                        ///< SVG <title> elements MUST contain only valid XHTML Content Document Phrasing content. Medium.
    
    // ?? 2.1.3.1.3
    GlossaryInvalidRootNode,                ///< Glossaries must use the <dl> element as their root node. Medium.
    
    ////// OMG SO MANY CONDITIONS FOR CONTENT THAT THE CORE LIBRARY ISN'T (YET) EVEN PROCESSING !!!
    
    ContentErrorMax,
    
    
    ////////////////////////////////////////////////////////////////////////////////
    // Media Overlays 3.0
    // EPUBSpec::MediaOverlays
    
    MediaOverlaysErrorBase                  = 400,
    
    // ?? 2.4.1
    MediaOverlayInvalidRootElement,         ///< The root element of all Media Overlay documents MUST be an <smil> element. Critical.
    MediaOverlayVersionMissing,             ///< The root <smil> element of a Media Overlay MUST have a version attribute. Medium.
    MediaOverlayInvalidVersion,             ///< The root <smil> element of a Media Overlay MUST have a version of '3.0'. Medium.
    
    // ?? 2.4.2
    MediaOverlayHeadIncorrectlyPlaced,      ///< The optional <head> element MUST only occur as the first child element of the root <smil> element. Medium.
    
    // ?? 2.4.4
    MediaOverlayNoBody,                     ///< The root <smil> element MUST contain a single <body> child element. Major.
    MediaOverlayMultipleBodies,             ///< The root <smil> element MUST contain a single <body> child element. Medium.
    MediaOverlayEmptyBody,                  ///< The <body> element MUST contain at least one <par> or <seq> child element. Major.
    
    // ?? 2.4.5
    MediaOverlayEmptySeq,                   ///< A <seq> element MUST contain at least one <par> or <seq> child element. Major.
    
    // ?? 2.4.6
    MediaOverlayEmptyPar,                   ///< A <par> element MUST contain a <text> child element. Major.
    
    // ?? 2.4.7
    MediaOverlayInvalidText,                ///< A <text> element MUST have a 'src' attribute. Critical.
    MediaOverlayInvalidTextSource,          ///< A <text> element's 'src' attribute MUST reference an item in the publication's <manifest>. Major.
    MediaOverlayTextSrcFragmentMissing,     ///< A <text> element's 'src' attribute MUST contain a fragment identifier. Medium.
    
    // ?? 2.4.8
    MediaOverlayInvalidAudio,               ///< An <audio> element MUST have a 'src' attribute. Critical.
    MediaOverlayInvalidAudioSource,         ///< An <audio> element's 'src' attribute MUST reference an item in the publication's <manifest>. Major.
    MediaOverlayInvalidAudioType,           ///< An <audio> element's 'src' attribute MUST reference an item which is a member of the EPUB 3 Core Media Types. Medium.

    MediaOverlayMissingDurationMetadata,           ///< A SMIL manifest item has no corresponding media:duration metadata. Medium.
    MediaOverlayMismatchDurationMetadata,           ///< A SMIL / metadata duration mismatch. Medium.
    MediaOverlayInvalidSmilClockValue,           ///< A given SMIL Clock Value (media:duration, clipBegin or clipEnd) has an invalid syntax. Medium.
    MediaOverlayCannotParseSMILXML,           ///< A given SMIL XML file fails to parse. Major.
    MediaOverlayUnknownSMILElement,           ///< A given SMIL file contains an unknown XML element. Major.
    MediaOverlaySMILTextParallelParent,           ///< A given SMIL file contains a text XML element that does not have a parallel parent time container. Major.
    MediaOverlaySMILAudioParallelParent,           ///< A given SMIL file contains an audio XML element that does not have a parallel parent time container. Major.
    MediaOverlaySMILParallelSequenceParent,           ///< A given SMIL file contains a parallel time container XML element that does not have a sequence parent time container. Major.
    MediaOverlaySMILSequenceSequenceParent,           ///< A given SMIL file contains a sequence time container XML element that does not have a sequence parent time container. Major.
    MediaOverlayInvalidTextRefSource,          ///< A <body> <seq> or <par> element's 'epub:textref' attribute MUST reference an item in the publication's <manifest>. Medium.


    MediaOverlayErrorMax,
    
    
    ////////////////////////////////////////////////////////////////////////////////
    // Canonical Fragment Identifiers
    // EPUBSpec::CanonicalFragmentIdentifiers
    
    CFIErrorBase                            = 500,
    
    // ?? 2.2
    CFIContainsLeadingZeroes,               ///< Numbers in CFIs MUST NOT have leading zeroes. Medium.
    CFIContainsTrailingFractionZeroes,      ///< Fractional numbers in CFIs MUST NOT have trailing zeroes in their fractional part. Medium.
    CFIContainsEmptyFraction,               ///< Integral numbers in CFIs MUST be represented as integers. Medium.
    CFIContainsTruncatedWholePart,          ///< Fraction numbers in CFIs in the range 1 > N > 0 MUST have a '0.' prefix. Medium.
    
    // ?? 2.3
    CFIParseFailed,                         ///< A CFI could not be parsed-- all special characters MUST be prefixed with a circumflex (^) character. Major.
    
    // ?? 3.1.1
    CFINonSlashStartCharacter,              ///< A CFI reference MUST begin with a slash (/) character. Major.
    CFIInvalidSpineLocation,                ///< An inter-publication CFI's first traversal step MUST be the location of the <spine> element within the publication's package document (usually '6'). Major.
    CFITooShort,                            ///< A CFI with only one component can't reasonably be expected to point to anything useful. Major.
    CFIUnexpectedComponent,                 ///< A CFI's second component is expected to be an indirector via the spine. Medium.
    CFIStepOutOfBounds,                     ///< A CFI's step value was beyond the bounds of available elements. Critical.
    
    // ?? 3.1.2
    CFINonAssertedXMLID,                    ///< A CFI step referencing an XML node with an 'id' attribute MUST assert that ID as part of the step. Minor.
    
    // ?? 3.1.3
    CFIInvalidIndirectionStartNode,         ///< A CFI indirection clause can only step into resources identified by: OPF <itemref> through <item> 'href' attribute, HTML5 <iframe> or <embed> 'src' attribute, HTML5 <object> 'data' attribute, or SVG <image> and <use> 'xlink:href' attributes. Major.
    CFIIndirectionTargetMissing,            ///< The 'href', 'src', 'data' &c. value used for indirection MUST be present. Major.
    CFIIndirectionTargetNotFound,           ///< The target IRI of an indirection step references a missing resource. Major.
    
    // ?? 3.1.4
    CFICharOffsetOnIllegalElement,          ///< A character offset MUST NOT be applied to an element, EXCEPT an HTML5 <img> element containing an 'alt' attribute. Major.
    CFICharOffsetInNonTerminatingStep,      ///< A character offset MUST only appear in the terminating step of a CFI. Major.
    CFICharOffsetOutOfBounds,               ///< A character offset MUST NOT be greater than the number of UTF-16 characters in the selected node. Major.
    
    // ?? 3.1.5
    CFITemporalOffsetInvalidResource,       ///< A CFI temporal offset MUST ONLY be used on an audio or video resource. Major.
    CFITemporalOffsetInNonTerminatingStep,  ///< A CFI temporal offset MUST ONLY appear in the terminating step of a CFI. Major.
    
    // ?? 3.1.6
    CFISpatialOffsetInvalidFormat,          ///< A CFI spatial offset MUST be of the form 'xxx:yyy'. Major.
    CFISpatialOffsetOutOfBounds,            ///< A CFI spatial offset MUST ONLY render coordinates in the range 0..100. Major.
    
    // ?? 3.1.8
    CFITextAssertionInvalidPlacement,       ///< A CFI text assertion MUST ONLY occur after a character offset terminating step. Major.
    
    // ?? 3.1.9
    CFISideBiasInvalidPlacement,            ///< A CFI side-bias assertion MUST ONLY occur at the end of a CFI. Major.
    CFISideBiasInvalidSide,                 ///< A CFI side-bias assertion MUST ONLY assert the values 'b' or 'a'. Major.
    
    // ?? 3.4
    CFIRangeInvalid,                        ///< A CFI range statement appears to be invalid. Did you forget to escape (^) something? Major.
    CFIRangeContainsSideBias,               ///< A ranged CFI MUST NOT contain any side-bias assertions. Medium.
    CFIRangeComponentCountInvalid,          ///< A CFI appears to have a number of range components other than 1 (no range) or 3 (a valid range). Medium.
    
    CFIErrorMax
    
};

class epub_spec_error : public std::runtime_error
{
    std::error_code     __ec;
    
public:
    EPUB3_EXPORT            epub_spec_error(EPUBError __ev, const std::string& __what_arg);
    EPUB3_EXPORT            epub_spec_error(EPUBError __ev, const char* __what_arg);
    EPUB3_EXPORT            epub_spec_error(EPUBError __ev);
    
    EPUB3_EXPORT            ~epub_spec_error()  _NOEXCEPT;
    
    FORCE_INLINE
    const std::error_code&  code()      const   _NOEXCEPT   { return __ec; }
    
    EPUB3_EXPORT
	ViolationSeverity       Severity()  const;

	EPUB3_EXPORT
	EPUBError				SpecErrorCode()	const _NOEXCEPT { return EPUBError(__ec.value()); }

	EPUB3_EXPORT
	EPUBSpec				Specification()	const;
    
private:
    static std::string __init(const std::error_code& code, std::string what);
    
};

EPUB3_EXPORT const std::string&         SeverityString(ViolationSeverity __s);
EPUB3_EXPORT const std::error_code      ErrorCodeForEPUBError(EPUBError ev)  _NOEXCEPT;
EPUB3_EXPORT const std::string          DetailedErrorMessage(EPUBError ev);
EPUB3_EXPORT const std::error_category& epub_spec_category() _NOEXCEPT;
EPUB3_EXPORT       EPUBSpec				SpecFromEPUBError(EPUBError ev);

// Lo, the std::system:error class's code() method was not marked virtual,
// and there was much fannying about.
class error_details
{
private:
	bool __is_spec_error_;
	union
	{
		const std::system_error*		__system_error_;
		const epub_spec_error*			__spec_error_;
	};

public:
	error_details(const std::system_error& __sysErr)
		: __is_spec_error_(false), __system_error_(&__sysErr)
		{}
	error_details(const epub_spec_error& __specErr)
		: __is_spec_error_(true), __spec_error_(&__specErr)
		{}
	~error_details() {}

private:
	error_details(const error_details&) _DELETED_;
	error_details& operator=(const error_details&) _DELETED_;

public:
	bool is_spec_error() const {
		return __is_spec_error_;
	}
	bool is_system_error() const {
		return !__is_spec_error_;
	}

	int code() const {
		if (__is_spec_error_)
			return __spec_error_->code().value();
		else
			return __system_error_->code().value();
	}
	const char* message() const {
		if (__is_spec_error_)
			return __spec_error_->what();
		else
			return __system_error_->what();
	}

	const std::error_category& category() const {
		if (__is_spec_error_)
			return __spec_error_->code().category();
		else 
			return __system_error_->code().category();
	}
	
	EPUBSpec epub_spec() const {
		if (__is_spec_error_)
			return __spec_error_->Specification();
		else
			throw std::logic_error("Attempt to get an EPUBSpec from a non-epub_spec_error exception");
	}

	ViolationSeverity severity() const {
		if (__is_spec_error_)
			return __spec_error_->Severity();
		else
			throw std::logic_error("Attempt to get a ViolationSeverity from a non-epub_spec_error exception");
	}

	EPUBError epub_error_code() const {
		if (__is_spec_error_)
			return __spec_error_->SpecErrorCode();
		else
			throw std::logic_error("Attempt to get an EPUBError from a non-epub_spec_error exception");
	}

	_NORETURN_
	void throw_error() const {
		if (__is_spec_error_)
			throw *__spec_error_;
		else
			throw *__system_error_;
	}

};

static inline FORCE_INLINE
void __DispatchError(const std::system_error& __err)
{
    if ( ErrorHandler()(error_details(__err)) == false )
        throw __err;
}
static inline FORCE_INLINE
void __DispatchError(const epub_spec_error& __err)
{
    if ( ErrorHandler()(error_details(__err)) == false )
        throw __err;
}

static inline FORCE_INLINE
void HandleError(int __code, const std::error_category& __cat)
{
    std::system_error __err(__code, __cat);
    __DispatchError(__err);
}
static inline FORCE_INLINE
void HandleError(int __code, const std::error_category& __cat, const std::string& __msg)
{
    std::system_error __err(__code, __cat, __msg);
    __DispatchError(__err);
}
static inline FORCE_INLINE
void HandleError(int __code, const std::error_category& __cat, const char* __msg)
{
    std::system_error __err(__code, __cat, __msg);
    __DispatchError(__err);
}
static inline FORCE_INLINE
void HandleError(std::errc __code)
{
    std::system_error __err(std::make_error_code(__code));
    __DispatchError(__err);
}
static inline FORCE_INLINE
void HandleError(std::errc __code, const std::string& __msg)
{
    std::system_error __err(std::make_error_code(__code), __msg);
    __DispatchError(__err);
}
static inline FORCE_INLINE
void HandleError(std::errc __code, const char* __msg)
{
    std::system_error __err(std::make_error_code(__code), __msg);
    __DispatchError(__err);
}
static inline FORCE_INLINE
void HandleError(EPUBError __code)
{
    epub_spec_error __err(__code);
    __DispatchError(__err);
}
static inline FORCE_INLINE
void HandleError(EPUBError __code, const std::string& __msg)
{
    epub_spec_error __err(__code, __msg);
    __DispatchError(__err);
}
static inline FORCE_INLINE
void HandleError(EPUBError __code, const char* __msg)
{
    epub_spec_error __err(__code, __msg);
    __DispatchError(__err);
}

#if EPUB_PLATFORM(WIN)
# define _THROW_WIN_ERROR_(err) HandleError(static_cast<int>(err), std::system_category())
# define _THROW_LAST_ERROR_() _THROW_WIN_ERROR_(::GetLastError())
#endif

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__error_handler__) */
