//
//  media-overlays_smil_model.cpp
//  ePub3
//
//  Created by Daniel Weck on 2013-09-15.
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

#include "media-overlays_smil_model.h"
#include "package.h"
#include <ePub3/media-overlays_smil_utils.h>
#include "error_handler.h"

EPUB3_BEGIN_NAMESPACE

        MediaOverlaysSmilModel::~MediaOverlaysSmilModel()
        {
        }

        MediaOverlaysSmilModel::MediaOverlaysSmilModel(const PackagePtr& package) //shared_ptr<Package>
        : OwnedBy(package)
        {
            uint32_t totalDurationFromMetadata = checkMetadata(package);

            uint32_t totalDurationFromSMILs = checkSMILs(package);

            if (totalDurationFromMetadata != totalDurationFromSMILs)
            {
                std::stringstream s;
                s << "Media Overlays duration mismatch (milliseconds): METADATA " << (long) totalDurationFromMetadata << " != SMILs " << (long) totalDurationFromSMILs;
                HandleError(EPUBError::MediaOverlayMismatchDurationMetadata, _Str(s.str()));
            }
            else
            {
                printf("Media Overlays SMILs parsed, total duration checked okay (milliseconds): %ld\n", (long)totalDurationFromSMILs);
            }
        }

        uint32_t MediaOverlaysSmilModel::checkSMILs(const PackagePtr& package)
        {
            uint32_t accumulatedDurationMilliseconds = 0;

            //std::map<string, shared_ptr<ManifestItem>>
            const ManifestTable& manifestTable = package->Manifest();

            for (ManifestTable::const_iterator iter = manifestTable.begin(); iter != manifestTable.end(); iter++)
            {
                ManifestItemPtr item = iter->second; //shared_ptr<ManifestItem>

                //string
                const ManifestItem::MimeType& mediaType = item->MediaType();

                if (mediaType != "application/smil+xml")
                {
                    continue;
                }

                printf("Media Overlays SMIL PARSING: %s\n", item->Href().c_str());

                //unique_ptr<ArchiveXmlReader> xmlReader = package->XmlReaderForRelativePath(item->Href());
                xmlDocPtr doc = item->ReferencedDocument();

                if (doc == nullptr)
                {
                    HandleError(EPUBError::MediaOverlayCannotParseSMILXML, _Str("Cannot parse XML: ", item->Href().c_str()));
                }

#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
XPathWrangler xpath(doc, {{"epub", ePub3NamespaceURI}, {"smil", SMILNamespaceURI}});
#else
                XPathWrangler::NamespaceList __ns;
                __ns["epub"] = ePub3NamespaceURI;
                __ns["smil"] = SMILNamespaceURI;
                XPathWrangler xpath(doc, __ns);
#endif
                xpath.NameDefaultNamespace("smil");

                xmlNodeSetPtr nodes = xpath.Nodes("/smil:smil");

                if (nodes->nodeNr == 0)
                {
                    HandleError(EPUBError::MediaOverlayInvalidRootElement, _Str("'smil' root element not found: ", item->Href().c_str()));
                }
                else if (nodes->nodeNr > 1)
                {
                    HandleError(EPUBError::MediaOverlayInvalidRootElement, _Str("Multiple 'smil' root elements found: ", item->Href().c_str()));
                }

                if (nodes->nodeNr != 1)
                {
                    xmlXPathFreeNodeSet(nodes);
                    return 0;
                }

                const xmlNodePtr smil = nodes->nodeTab[0];

                string version = _getProp(smil, "version", SMILNamespaceURI);
                if (version.empty())
                {
                    HandleError(EPUBError::MediaOverlayVersionMissing, _Str("SMIL version not found: ", item->Href().c_str()));
                }
                else if (version != "3.0")
                {
                    HandleError(EPUBError::MediaOverlayInvalidVersion, _Str("Invalid SMIL version (", version, "): ", item->Href().c_str()));
                }

                xmlXPathFreeNodeSet(nodes);

                nodes = xpath.Nodes("./smil:body", smil);

                if (nodes->nodeNr == 0)
                {
                    HandleError(EPUBError::MediaOverlayNoBody, _Str("'body' element not found: ", item->Href().c_str()));
                }
                else if (nodes->nodeNr > 1)
                {
                    HandleError(EPUBError::MediaOverlayMultipleBodies, _Str("multiple 'body' elements found: ", item->Href().c_str()));
                }

                if (nodes->nodeNr != 1)
                {
                    xmlXPathFreeNodeSet(nodes);
                    return 0;
                }

                const xmlNodePtr body = nodes->nodeTab[0];

                uint32_t smilDur = checkSMIL(item, body, xpath);

                printf("Media Overlays SMIL DURATION (milliseconds): %ld\n", (long) smilDur);

                accumulatedDurationMilliseconds += smilDur;

                xmlXPathFreeNodeSet(nodes);
            }

            return accumulatedDurationMilliseconds;
        }

        uint32_t MediaOverlaysSmilModel::checkSMIL(const ManifestItemPtr& item, const xmlNodePtr element, const XPathWrangler xpath)
        {
            if (element == nullptr || element->type != XML_ELEMENT_NODE)
            {
                return 0;
            }

            uint32_t accumulatedDurationMilliseconds = 0;

            std::string elementName(reinterpret_cast<const char *>(element->name));

            if (elementName == "body" || elementName == "seq")
            {
                //const string& str = reinterpret_cast<const char *>(xmlNodeGetContent(linkedChildrenList));
            }
            else if (elementName == "par")
            {

            }
            else if (elementName == "audio")
            {
                uint32_t clipBeginMilliseconds = 0;
                uint32_t clipEndMilliseconds = 0;

                string clipBeginStr = _getProp(element, "clipBegin", SMILNamespaceURI);
                //printf("Media Overlays CLIP BEGIN: %s\n", clipBeginStr.c_str());
                if (!clipBeginStr.empty())
                {
                    try
                    {
                        clipBeginMilliseconds = ePub3::SmilClockValuesParser::ToWholeMilliseconds(clipBeginStr);
                    }
                    catch (const std::invalid_argument& exc)
                    {
                        HandleError(EPUBError::MediaOverlayInvalidSmilClockValue, _Str(item->Href().c_str(), " -- clipBegin=", clipBeginStr, " => invalid SMIL Clock Value syntax"));
                    }
                    //catch (...)
                    //{
                    //}
                }

                string clipEndStr = _getProp(element, "clipEnd", SMILNamespaceURI);
                //printf("Media Overlays CLIP END: %s\n", clipEndStr.c_str());
                if (!clipEndStr.empty())
                {
                    try
                    {
                        clipEndMilliseconds = ePub3::SmilClockValuesParser::ToWholeMilliseconds(clipEndStr);
                    }
                    catch (const std::invalid_argument& exc)
                    {
                        HandleError(EPUBError::MediaOverlayInvalidSmilClockValue, _Str(item->Href().c_str(), " -- clipEnd=", clipEndStr, " => invalid SMIL Clock Value syntax"));
                    }
                    //catch (...)
                    //{
                    //}
                }

                uint32_t clipDuration = clipEndMilliseconds - clipBeginMilliseconds;

                if (clipDuration <= 0)
                {
                    HandleError(EPUBError::MediaOverlayInvalidAudio, _Str(item->Href().c_str(), " -- clipBegin=", clipBeginStr, ", clipEnd=", clipEndStr, " => invalid time values"));
                }
                else
                {
                    accumulatedDurationMilliseconds += clipDuration;
                }
            }
            else if (elementName == "text")
            {

            }

            xmlNodePtr linkedChildrenList = element->children;
            if (linkedChildrenList != nullptr)
            {
                for (; linkedChildrenList != nullptr; linkedChildrenList = linkedChildrenList->next)
                {
                    if (linkedChildrenList->type != XML_ELEMENT_NODE)
                    {
                        continue;
                    }

                    accumulatedDurationMilliseconds += checkSMIL(item, linkedChildrenList, xpath);
                }
            }

            return accumulatedDurationMilliseconds;
        }

        uint32_t MediaOverlaysSmilModel::checkMetadata(const PackagePtr& package)
        {
            const string& narrator = package->MediaOverlays_Narrator();
            printf("Media Overlays NARRATOR: %s\n", narrator.c_str());

            const string& activeClass = package->MediaOverlays_ActiveClass();
            printf("Media Overlays ACTIVE CLASS: %s\n", activeClass.c_str());

            const string& playbackActiveClass = package->MediaOverlays_PlaybackActiveClass();
            printf("Media Overlays PLAYBACK ACTIVE CLASS: %s\n", playbackActiveClass.c_str());

            const string& durationStr = package->MediaOverlays_DurationTotal();
            printf("Media Overlays TOTAL DURATION (string): %s\n", durationStr.c_str());

            uint32_t totalDurationWholeMilliseconds = 0;
            if (!durationStr.empty())
            {
                try
                {
                    totalDurationWholeMilliseconds = ePub3::SmilClockValuesParser::ToWholeMilliseconds(durationStr);
                    printf("Media Overlays TOTAL DURATION (milliseconds): %ld\n", (long) totalDurationWholeMilliseconds);
                }
                catch (const std::invalid_argument& exc)
                {
                    HandleError(EPUBError::MediaOverlayInvalidSmilClockValue, _Str("OPF package -- media:duration=", durationStr, " => invalid SMIL Clock Value syntax"));
                }
                //catch (...)
                //{
                //}
            }

            uint32_t accumulatedDurationMilliseconds = 0;

            //std::map<string, shared_ptr<ManifestItem>>
            const ManifestTable& manifestTable = package->Manifest();

            for (ManifestTable::const_iterator iter = manifestTable.begin(); iter != manifestTable.end(); iter++)
            {
                ManifestItemPtr item = iter->second; //shared_ptr<ManifestItem>

                //string
                const ManifestItem::MimeType& mediaType = item->MediaType();

                if (mediaType != "application/smil+xml")
                {
                    continue;
                }

                printf("Media Overlays SMIL HREF: %s\n", item->Href().c_str());

                const string& itemDurationStr = package->MediaOverlays_DurationItem(item);
                printf("Media Overlays SMIL DURATION (string): %s\n", itemDurationStr.c_str());

                if (itemDurationStr.empty())
                {
                    HandleError(EPUBError::MediaOverlayMissingDurationMetadata, _Str(item->Href(), " => missing media:duration metadata"));
                }
                else
                {
                    uint32_t durationWholeMilliseconds = 0;
                    try
                    {
                        durationWholeMilliseconds = ePub3::SmilClockValuesParser::ToWholeMilliseconds(itemDurationStr);
                        printf("Media Overlays SMIL DURATION (milliseconds): %ld\n", (long) durationWholeMilliseconds);
                        accumulatedDurationMilliseconds += durationWholeMilliseconds;
                    }
                    catch (const std::invalid_argument& exc)
                    {
                        HandleError(EPUBError::MediaOverlayInvalidSmilClockValue, _Str(item->Href(), " -- media:duration=", itemDurationStr, " => invalid SMIL Clock Value syntax"));
                    }
                    //catch (...)
                    //{
                    //}
                }
            }

            if (accumulatedDurationMilliseconds != totalDurationWholeMilliseconds)
            {
                if (totalDurationWholeMilliseconds == 0)
                {
                    HandleError(EPUBError::MediaOverlayMissingDurationMetadata, _Str("OPF package", " => missing media:duration metadata"));
                }
                else
                {
                    std::stringstream s;
                    s << "Media Overlays duration mismatch (milliseconds): TOTAL " << (long) totalDurationWholeMilliseconds << " != ACCUMULATED " << (long) accumulatedDurationMilliseconds;
                    HandleError(EPUBError::MediaOverlayMismatchDurationMetadata, _Str(s.str()));
                }
            }
            else
            {
                printf("Media Overlays SMIL DURATION check okay.\n");
            }

            return totalDurationWholeMilliseconds;
        }

        EPUB3_END_NAMESPACE
