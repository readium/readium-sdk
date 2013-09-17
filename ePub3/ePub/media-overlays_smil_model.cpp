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
#include "xpath_wrangler.h"

EPUB3_BEGIN_NAMESPACE

        SMILData::SMILData(const MediaOverlaysSmilModelPtr smilModel, uint32_t duration)
        : OwnedBy(smilModel), _duration(duration)
        {

        }

        MediaOverlaysSmilModel::~MediaOverlaysSmilModel()
        {
        }

        MediaOverlaysSmilModel::MediaOverlaysSmilModel(const PackagePtr package) //shared_ptr<Package>
        : OwnedBy(package), _activeClass(""), _playbackActiveClass(""), _narrator(""), _totalDuration(0)
        {
        }

        void debugTreeAudio(const SMILData::Audio *audio)
        {
            printf("-- DEBUG TREE AUDIO\n");

            printf("CHECK SMIL DATA TREE MEDIA SRC %s\n", audio->_src.c_str());

            printf("CHECK SMIL DATA TREE AUDIO: %ld --> %ld\n", (long) audio->_clipBeginMilliseconds, (long) audio->_clipEndMilliseconds);
        }

        void debugTreeText(const SMILData::Text *text)
        {
            printf("-- DEBUG TREE TEXT\n");

            printf("CHECK SMIL DATA TREE MEDIA SRC %s\n", text->_src.c_str());

            printf("CHECK SMIL DATA TREE TEXT\n");
        }

        void debugTreePar(const SMILData::Parallel *par)
        {
            printf("-- DEBUG TREE PAR\n");

            printf("CHECK SMIL DATA TREE TEXTREF %s\n", par->_textref.c_str());
            printf("CHECK SMIL DATA TREE TYPE %s\n", par->_type.c_str());

            if (par->_text != nullptr)
            {
                debugTreeText(par->_text);
            }

            if (par->_audio != nullptr)
            {
                debugTreeAudio(par->_audio);
            }
        }

        void debugTreeSeq(const SMILData::Sequence *seqq)
        {
            printf("-- DEBUG TREE SEQ\n");

            printf("CHECK SMIL DATA TREE TEXTREF %s\n", seqq->_textref.c_str());
            printf("CHECK SMIL DATA TREE TYPE %s\n", seqq->_type.c_str());

            for (int i = 0; i < seqq->_children.size(); i++)
            {
                SMILData::TimeContainer* container = seqq->_children[i];

                const SMILData::Sequence *seq = dynamic_cast<const SMILData::Sequence *>(container);
                if (seq != nullptr)
                {
                    debugTreeSeq(seq);
                    continue;
                }

                const SMILData::Parallel *par = dynamic_cast<const SMILData::Parallel *>(container);
                if (par != nullptr)
                {
                    debugTreePar(par);
                    continue;
                }

                throw std::invalid_argument("WTF?");
            }
        }

        void debugSmilData(ManifestItemSMILMap _smilMap)
        {
            for (auto iterator = _smilMap.begin(); iterator != _smilMap.end(); iterator++)
            {
                string id = iterator->first;
                SMILDataPtr smilData = iterator->second;

                printf("}}}}}}} CHECK SMIL DATA TREE %s duration (milliseconds): %ld\n", id.c_str(), (long) smilData->GetDuration());

                debugTreeSeq(smilData->GetRoot());
            }
        }

        void MediaOverlaysSmilModel::InitData()
        {
            parseMetadata();

            uint32_t totalDurationFromSMILs = parseSMILs();

            if (_totalDuration != totalDurationFromSMILs)
            {
                std::stringstream s;
                s << "Media Overlays duration mismatch (milliseconds): METADATA " << (long) _totalDuration << " != SMILs " << (long) totalDurationFromSMILs;
                HandleError(EPUBError::MediaOverlayMismatchDurationMetadata, _Str(s.str()));
            }
            else
            {
                printf("Media Overlays SMILs parsed, total duration checked okay (milliseconds): %ld\n", (long) totalDurationFromSMILs);
            }

            debugSmilData(_smilMap);
        }

        uint32_t MediaOverlaysSmilModel::parseSMILs()
        {
            PackagePtr package = Owner();

            uint32_t accumulatedDurationMilliseconds = 0;

            // const ManifestTable& manifestTable = package->Manifest();
            // for (ManifestTable::const_iterator iter = manifestTable.begin(); iter != manifestTable.end(); iter++)
            //ManifestItemPtr item = iter->second; //shared_ptr<ManifestItem>

            shared_ptr<SpineItem> spineItem = package->FirstSpineItem();
            while (spineItem != nullptr)
            {
                ManifestItemPtr item = spineItem->ManifestItem();

                //string
                //const ManifestItem::MimeType& mediaType = item->MediaType();
                //if (mediaType != "application/smil+xml")

                //const string & mediaOverlayID = item->MediaOverlayID();
                //printf("--- Media Overlays ID: %s\n", mediaOverlayID.c_str());
                item = item->MediaOverlay();
                if (item == nullptr)
                {
                    spineItem = spineItem->Next();
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

                //SMILDataPtr smilData = getSMILDataForManifestItem(item->Identifier());
                SMILDataPtr smilData = _smilMap[item->Identifier()];

                printf("ITEM DUR: %ld\n", (long) smilData->GetDuration());

                const xmlNodePtr body = nodes->nodeTab[0];

                uint32_t smilDur = parseSMIL(smilData, nullptr, nullptr, item, body);


                printf("Media Overlays SMIL DURATION (milliseconds): %ld\n", (long) smilDur);

                accumulatedDurationMilliseconds += smilDur;

                xmlXPathFreeNodeSet(nodes);

                spineItem = spineItem->Next();
            }

            return accumulatedDurationMilliseconds;
        }

        uint32_t MediaOverlaysSmilModel::parseSMIL(SMILDataPtr smilData, SMILData::Sequence *sequence, SMILData::Parallel *parallel, const ManifestItemPtr item, const xmlNodePtr element)
        {
            if (element == nullptr || element->type != XML_ELEMENT_NODE)
            {
                return 0;
            }

            uint32_t accumulatedDurationMilliseconds = 0;

            std::string elementName(reinterpret_cast<const char *>(element->name));

            string textref = string(_getProp(element, "textref", ePub3NamespaceURI));
            string type = string(_getProp(element, "type", ePub3NamespaceURI));
            string src = string(_getProp(element, "src", SMILNamespaceURI));


            if (elementName == "body")
            {
                //const string& str = reinterpret_cast<const char *>(xmlNodeGetContent(linkedChildrenList));

                ManifestItemPtr textrefManifestItem = nullptr; //TODO
                smilData->_root = new SMILData::Sequence(nullptr, textref, textrefManifestItem, type);

                //parent = std::dynamic_pointer_cast<SMILData::TimeContainer *>(smilData->_root);
                //parent = std::reinterpret_cast<SMILData::TimeContainer *>(smilData->_root);
                //parent = smilData->_root;

                sequence = smilData->_root;
                parallel = nullptr;
            }
            else if (elementName == "seq")
            {
                ManifestItemPtr textrefManifestItem = nullptr; //TODO
                SMILData::Sequence *seq = new SMILData::Sequence(sequence, textref, textrefManifestItem, type);

                sequence = seq;
                parallel = nullptr;
            }
            else if (elementName == "par")
            {
                ManifestItemPtr textrefManifestItem = nullptr; //TODO
                SMILData::Parallel *par = new SMILData::Parallel(sequence, textref, textrefManifestItem, type);

                sequence = nullptr;
                parallel = par;
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

                ManifestItemPtr srcManifestItem = nullptr; //TODO
                SMILData::Audio *audio = new SMILData::Audio(parallel, src, srcManifestItem, clipBeginMilliseconds, clipEndMilliseconds);

                sequence = nullptr;
                parallel = nullptr;
            }
            else if (elementName == "text")
            {
                ManifestItemPtr srcManifestItem = nullptr; //TODO
                SMILData::Text *text = new SMILData::Text(parallel, src, srcManifestItem);

                sequence = nullptr;
                parallel = nullptr;
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

                    accumulatedDurationMilliseconds += parseSMIL(smilData, sequence, parallel, item, linkedChildrenList);
                }
            }

            return accumulatedDurationMilliseconds;
        }

        void MediaOverlaysSmilModel::parseMetadata()
        {
            PackagePtr package = Owner();

            MediaOverlaysSmilModelPtr sharedMe = std::enable_shared_from_this<MediaOverlaysSmilModel>::shared_from_this();

            //MediaOverlaysSmilModelPtr sharedMO = std::dynamic_pointer_cast<MediaOverlaysSmilModel>(sharedMe);

            _narrator = package->MediaOverlays_Narrator();
            printf("Media Overlays NARRATOR: %s\n", _narrator.c_str());

            _activeClass = package->MediaOverlays_ActiveClass();
            printf("Media Overlays ACTIVE CLASS: %s\n", _activeClass.c_str());

            _playbackActiveClass = package->MediaOverlays_PlaybackActiveClass();
            printf("Media Overlays PLAYBACK ACTIVE CLASS: %s\n", _playbackActiveClass.c_str());

            const string durationStr = package->MediaOverlays_DurationTotal();
            printf("Media Overlays TOTAL DURATION (string): %s\n", durationStr.c_str());

            _totalDuration = 0;
            if (!durationStr.empty())
            {
                try
                {
                    _totalDuration = ePub3::SmilClockValuesParser::ToWholeMilliseconds(durationStr);
                    printf("Media Overlays TOTAL DURATION (milliseconds): %ld\n", (long) _totalDuration);
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

            // const ManifestTable& manifestTable = package->Manifest();
            // for (ManifestTable::const_iterator iter = manifestTable.begin(); iter != manifestTable.end(); iter++)
            //ManifestItemPtr item = iter->second; //shared_ptr<ManifestItem>

            shared_ptr<SpineItem> spineItem = package->FirstSpineItem();
            while (spineItem != nullptr)
            {
                ManifestItemPtr item = spineItem->ManifestItem();

                //string
                //const ManifestItem::MimeType& mediaType = item->MediaType();
                //if (mediaType != "application/smil+xml")

                //const string & mediaOverlayID = item->MediaOverlayID();
                //printf("--- Media Overlays ID: %s\n", mediaOverlayID.c_str());
                item = item->MediaOverlay();
                if (item == nullptr)
                {
                    spineItem = spineItem->Next();
                    continue;
                }

                printf("Media Overlays SMIL HREF: %s\n", item->Href().c_str());

                const string itemDurationStr = package->MediaOverlays_DurationItem(item);
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

                        SMILDataPtr smilData = std::make_shared<class SMILData>(sharedMe, durationWholeMilliseconds);
                        _smilMap.insert(std::make_pair(item->Identifier(), smilData));

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

                spineItem = spineItem->Next();
            }

            if (accumulatedDurationMilliseconds != _totalDuration)
            {
                if (_totalDuration == 0)
                {
                    HandleError(EPUBError::MediaOverlayMissingDurationMetadata, _Str("OPF package", " => missing media:duration metadata"));
                }
                else
                {
                    std::stringstream s;
                    s << "Media Overlays duration mismatch (milliseconds): TOTAL " << (long) _totalDuration << " != ACCUMULATED " << (long) accumulatedDurationMilliseconds;
                    HandleError(EPUBError::MediaOverlayMismatchDurationMetadata, _Str(s.str()));
                }
            }
            else
            {
                printf("Media Overlays SMIL DURATION check okay.\n");
            }
        }

        EPUB3_END_NAMESPACE
