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
#include <ePub3/media-overlays_smil_data.h>
#include "error_handler.h"
#include "xpath_wrangler.h"

EPUB3_BEGIN_NAMESPACE

/*
        void debugTreeAudio(const SMILData::Audio *audio)
        {
            printf("-- DEBUG TREE AUDIO\n");

            printf("CHECK SMIL DATA TREE MEDIA SRC %s\n", audio->_src_file.c_str());

            printf("CHECK SMIL DATA TREE AUDIO: %ld --> %ld\n", (long) audio->_clipBeginMilliseconds, (long) audio->_clipEndMilliseconds);
        }

        void debugTreeText(const SMILData::Text *text)
        {
            printf("-- DEBUG TREE TEXT\n");

            printf("CHECK SMIL DATA TREE MEDIA SRC FILE %s\n", text->_src_file.c_str());
            printf("CHECK SMIL DATA TREE MEDIA SRC FRAGID %s\n", text->_src_fragmentID.c_str());

            printf("CHECK SMIL DATA TREE TEXT\n");
        }

        void debugTreePar(const SMILData::Parallel *par)
        {
            printf("-- DEBUG TREE PAR\n");

            printf("CHECK SMIL DATA TREE TEXTREF FILE %s\n", par->_textref_file.c_str());
            printf("CHECK SMIL DATA TREE TEXTREF FRAGID %s\n", par->_textref_fragmentID.c_str());
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

            printf("CHECK SMIL DATA TREE TEXTREF FILE %s\n", seqq->_textref_file.c_str());
            printf("CHECK SMIL DATA TREE TEXTREF FRAGID %s\n", seqq->_textref_fragmentID.c_str());
            printf("CHECK SMIL DATA TREE TYPE %s\n", seqq->_type.c_str());

            for (int i = 0; i < seqq->_children.size(); i++)
            {
                SMILData::TimeContainer *container = seqq->_children[i];

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

        void debugSmilData(std::vector<shared_ptr<SMILData>> smilDatas)
        {
            for (int i = 0; i < smilDatas.size(); i++)
            {
                shared_ptr<SMILData> smilData = smilDatas[i];

                printf("}}}}}}} CHECK SMIL DATA TREE %s duration (milliseconds): %ld\n", smilData->ManifestItem()->Identifier().c_str(), (long) smilData->DurationMilliseconds());

                debugTreeSeq(smilData->Body());
            }
        }
*/

        MediaOverlaysSmilModel::~MediaOverlaysSmilModel()
        {
            //printf("~MediaOverlaysSmilModel()\n");
        }

        MediaOverlaysSmilModel::MediaOverlaysSmilModel(const PackagePtr package) //shared_ptr<Package>
        : OwnedBy(package), _totalDuration(0), _smilDatas(std::vector<shared_ptr<SMILData>>())
        {
        }

        void MediaOverlaysSmilModel::Initialize()
        {
            resetData();
            populateData();
        }

        void MediaOverlaysSmilModel::resetData()
        {
            _totalDuration = 0;

            for (int i = 0; i < _smilDatas.size(); i++)
            {
                shared_ptr<SMILData> o = _smilDatas[i];
                //TODO: o->releaseMemory() ?
            }

            //_smilDatas.erase(_smilDatas.begin(), _smilDatas.end());
            _smilDatas.clear();
        }

        void MediaOverlaysSmilModel::populateData()
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
                //printf("Media Overlays SMILs parsed, total duration checked okay (milliseconds): %ld\n", (long) totalDurationFromSMILs);
            }

            //debugSmilData(_smilDatas);
        }

        void MediaOverlaysSmilModel::parseMetadata()
        {
            const string& _narrator = Narrator();
            //printf("Media Overlays NARRATOR: %s\n", _narrator.c_str());

            const string& _activeClass = ActiveClass();
            //printf("Media Overlays ACTIVE CLASS: %s\n", _activeClass.c_str());

            const string& _playbackActiveClass = PlaybackActiveClass();
            //printf("Media Overlays PLAYBACK ACTIVE CLASS: %s\n", _playbackActiveClass.c_str());

            PackagePtr package = Owner();

            const string& durationStr = package->MediaOverlays_DurationTotal();
            //printf("Media Overlays TOTAL DURATION (string): %s\n", durationStr.c_str());

            _totalDuration = 0;
            if (!durationStr.empty())
            {
                try
                {
                    _totalDuration = ePub3::SmilClockValuesParser::ToWholeMilliseconds(durationStr);
                    //printf("Media Overlays TOTAL DURATION (milliseconds): %ld\n", (long) _totalDuration);
                }
                catch (const std::invalid_argument& exc)
                {
                    HandleError(EPUBError::MediaOverlayInvalidSmilClockValue, _Str("OPF package -- media:duration=", durationStr, " => invalid SMIL Clock Value syntax"));
                }
                //catch (...)
                //{
                //}
            }

            MediaOverlaysSmilModelPtr sharedMe = std::enable_shared_from_this<MediaOverlaysSmilModel>::shared_from_this();

            uint32_t accumulatedDurationMilliseconds = 0;

            // const ManifestTable& manifestTable = package->Manifest();
            // for (ManifestTable::const_iterator iter = manifestTable.begin(); iter != manifestTable.end(); iter++)
            // ManifestItemPtr item = iter->second; //shared_ptr<ManifestItem>

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

                //printf("Media Overlays SMIL HREF: %s\n", item->Href().c_str());

                const string& itemDurationStr = package->MediaOverlays_DurationItem(item);
                //printf("Media Overlays SMIL DURATION (string): %s\n", itemDurationStr.c_str());

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
                        //printf("Media Overlays SMIL DURATION (milliseconds): %ld\n", (long) durationWholeMilliseconds);

                        SMILDataPtr smilData = std::make_shared<class SMILData>(sharedMe, item, durationWholeMilliseconds);
                        _smilDatas.push_back(smilData);

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
                //printf("Media Overlays SMIL DURATION check okay.\n");
            }
        }

        uint32_t MediaOverlaysSmilModel::parseSMILs()
        {
            PackagePtr package = Owner();

            uint32_t accumulatedDurationMilliseconds = 0;

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

                //printf("Media Overlays SMIL PARSING: %s\n", item->Href().c_str());

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
                    xmlFreeDoc(doc);
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

                nodes = xpath.Nodes("./smil:head", smil);

                if (nodes->nodeNr == 0)
                {
                    // OKAY
                }
                else if (nodes->nodeNr == 1)
                {
                    //TODO: check head placement
                    //HandleError(EPUBError::MediaOverlayHeadIncorrectlyPlaced, _Str("'head' element incorrectly placed: ", item->Href().c_str()));
                }
                else if (nodes->nodeNr > 1)
                {
                    HandleError(EPUBError::MediaOverlayHeadIncorrectlyPlaced, _Str("multiple 'head' elements found: ", item->Href().c_str()));
                }

                xmlXPathFreeNodeSet(nodes);

                if (nodes->nodeNr > 1)
                {
                    xmlFreeDoc(doc);
                    return 0;
                }

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
                    xmlFreeDoc(doc);
                    return 0;
                }

                SMILDataPtr smilData = getDataForSMILManifestItem(item->Identifier());


                //printf("Media Overlays ITEM DUR: %ld\n", (long) smilData->DurationMilliseconds());

                const xmlNodePtr body = nodes->nodeTab[0];

                uint32_t smilDur = parseSMIL(smilData, nullptr, nullptr, item, body);


                //printf("Media Overlays SMIL DURATION (milliseconds): %ld\n", (long) smilDur);

                accumulatedDurationMilliseconds += smilDur;

                xmlXPathFreeNodeSet(nodes);
                xmlFreeDoc(doc);

                spineItem = spineItem->Next();
            }

            return accumulatedDurationMilliseconds;
        }

        std::vector<string> splitIriFileFragmentID(const string& iri)
        {
            //printf("=========== IRI: %s\n", iri.c_str());

            //auto split = std::vector<string>(2);
            std::vector<string> split;

            // FILE
            string::size_type i = iri.find_first_of('#');
            if (i != string::npos && i > 0)
            {
                split.push_back(iri.substr(0, i));
            }
            else
            {
                split.push_back(string(iri));
            }

            // FRAGMENT ID
            if (i != string::npos && i >= 0 && i < (iri.length() - 1))
            {
                string::size_type n = iri.length() - i - 1;
                split.push_back(iri.substr(i + 1, n));
            }
            else
            {
                split.push_back("");
            }

            //printf("=========== IRI FILE: %s\n", split.at(0).c_str());
            //printf("=========== IRI FRAGID: %s\n", split.at(1).c_str());

            return split;
        }

        ManifestItemPtr getReferencedManifestItem(PackagePtr package, string filepathInSmil, ManifestItemPtr smilItem)
        {
            if (filepathInSmil.empty())
            {
                return nullptr;
            }

            const string smilOPFPath = smilItem->AbsolutePath();
            //printf("=========== smilOPFPath: %s\n", smilOPFPath.c_str());

            string::size_type i = smilOPFPath.find_last_of('/');
            string smilParentFolder = "/";
            if (i != string::npos && i > 0)
            {
                smilParentFolder = _Str(smilOPFPath.substr(0, i), '/');
            }
            //printf("=========== smilParentFolder: %s\n", smilParentFolder.c_str());

            std::string refOPFPath = _Str(smilParentFolder, filepathInSmil);

            //std::string refOPFPath = _Str("/.././void1/../", smilParentFolder, "./void2/.././", filepathInSmil);
            //std::string refOPFPath = _Str(smilParentFolder, "void/../", filepathInSmil);
            //std::string refOPFPath = _Str("../", smilParentFolder, filepathInSmil);

            //printf("=========== refOPFPath BEFORE: %s\n", refOPFPath.c_str());

            std::string::size_type j = 0;
            while ((j = refOPFPath.find("../")) != string::npos)
            {
                //printf("=========== REPLACE 1: %d\n", j);

                std::string::size_type k = j < 2 ? 0 : refOPFPath.rfind('/', j - 2);
                //std::string::size_type k = refOPFPath.find_first_of('/', j);
                if (k == string::npos || k == 0)
                {
                    refOPFPath.replace(0, (j + 2) + 1, "");
                    //printf("=========== REPLACE K1: %d\n", k);
                }
                else
                {
                    refOPFPath.replace(k + 1, (j + 2) - k, "");
                    //printf("=========== REPLACE K2: %d\n", k);
                }
            }

            j = 0;
            while ((j = refOPFPath.find("./")) != string::npos)
            {
                refOPFPath.replace(j, 2, "");
                //printf("=========== REPLACE 2: %d\n", j);
            }

            //printf("=========== refOPFPath AFTER: %s\n", refOPFPath.c_str());

            const ManifestTable& manifestTable = package->Manifest();
            for (ManifestTable::const_iterator iter = manifestTable.begin(); iter != manifestTable.end(); iter++)
            {
                ManifestItemPtr manItem = iter->second; //shared_ptr<ManifestItem>

                const string abs = manItem->AbsolutePath();
                if (abs == refOPFPath)
                {
                    //printf("------> manifest item path: %s\n", abs.c_str());
                    return manItem;
                }
            }

            printf("------> manifest item path NOT FOUND??!: %s\n", filepathInSmil.c_str());
            return nullptr;
        }

        uint32_t MediaOverlaysSmilModel::parseSMIL(SMILDataPtr smilData, SMILData::Sequence *sequence, SMILData::Parallel *parallel, const ManifestItemPtr item, const xmlNodePtr element)
        {
            if (element == nullptr || element->type != XML_ELEMENT_NODE)
            {
                return 0;
            }

            uint32_t accumulatedDurationMilliseconds = 0;

            std::string elementName(reinterpret_cast<const char *>(element->name));

            string textref_ = string(_getProp(element, "textref", ePub3NamespaceURI));
            string textref_file;
            string textref_fragmentID;
            if (!textref_.empty())
            {
                //printf("=========== TEXTREF: %s\n", textref.c_str());

                std::vector<string> split = splitIriFileFragmentID(textref_);

                textref_file = split[0];
                textref_fragmentID = split[1];

                //printf("=========== TEXTREF FILE: %s\n", textref_file.c_str());
                //printf("=========== TEXTREF FRAGID: %s\n", textref_fragmentID.c_str());
            }

            string src_ = string(_getProp(element, "src", SMILNamespaceURI));
            string src_file;
            string src_fragmentID;
            if (!src_.empty())
            {
                //printf("=========== SRC: %s\n", src.c_str());

                std::vector<string> split = splitIriFileFragmentID(src_);

                src_file = split[0];
                src_fragmentID = split[1];

                //printf("=========== SRC FILE: %s\n", src_file.c_str());
                //printf("=========== SRC FRAGID: %s\n", src_fragmentID.c_str());
            }

            PackagePtr package = Owner();

            ManifestItemPtr srcManifestItem = getReferencedManifestItem(package, src_file, item);

            ManifestItemPtr textrefManifestItem = getReferencedManifestItem(package, textref_file, item);

            string type = string(_getProp(element, "type", ePub3NamespaceURI));

            if (elementName == "body")
            {
                if (!textref_file.empty() && textrefManifestItem == nullptr)
                {
                    HandleError(EPUBError::MediaOverlayInvalidTextRefSource, _Str(item->Href().c_str(), " [", textref_file.c_str(), "] => text ref manifest cannot be found"));
                }

                smilData->_root = new SMILData::Sequence(nullptr, textref_file, textref_fragmentID, textrefManifestItem, type);

                //sequence = smilData->Body();
                //sequence = smilData->_root; // because of const qualifier
                sequence = const_cast<ePub3::SMILData::Sequence *>(smilData->Body());

                parallel = nullptr;
            }
            else if (elementName == "seq")
            {
                if (!textref_file.empty() && textrefManifestItem == nullptr)
                {
                    HandleError(EPUBError::MediaOverlayInvalidTextRefSource, _Str(item->Href().c_str(), " [", textref_file.c_str(), "] => text ref manifest cannot be found"));
                }

                if (sequence == nullptr)
                {
                    HandleError(EPUBError::MediaOverlaySMILSequenceSequenceParent, _Str(item->Href().c_str(), " => parent of sequence time container must be sequence"));
                }

                SMILData::Sequence *seq = new SMILData::Sequence(sequence, textref_file, textref_fragmentID, textrefManifestItem, type);

                sequence = seq;
                parallel = nullptr;
            }
            else if (elementName == "par")
            {
                if (!textref_file.empty() && textrefManifestItem == nullptr)
                {
                    HandleError(EPUBError::MediaOverlayInvalidTextRefSource, _Str(item->Href().c_str(), " [", textref_file.c_str(), "] => text ref manifest cannot be found"));
                }

                if (sequence == nullptr)
                {
                    HandleError(EPUBError::MediaOverlaySMILParallelSequenceParent, _Str(item->Href().c_str(), " => parent of parallel time container must be sequence"));
                }

                SMILData::Parallel *par = new SMILData::Parallel(sequence, textref_file, textref_fragmentID, textrefManifestItem, type);

                sequence = nullptr;
                parallel = par;
            }
            else if (elementName == "audio")
            {
                if (parallel == nullptr)
                {
                    HandleError(EPUBError::MediaOverlaySMILAudioParallelParent, _Str(item->Href().c_str(), " => parent of audio must be parallel time container"));
                }

                if (src_file.empty())
                {
                    HandleError(EPUBError::MediaOverlayInvalidAudio, _Str(item->Href().c_str(), " => audio source is empty"));
                }
                else if (srcManifestItem == nullptr)
                {
                    HandleError(EPUBError::MediaOverlayInvalidAudioSource, _Str(item->Href().c_str(), " [", src_file.c_str(), "] => audio source manifest cannot be found"));
                }
                else if (srcManifestItem->MediaType() != "audio/mpeg" || srcManifestItem->MediaType() != "audio/mp4") //package->CoreMediaTypes.find(mediaType) == package->CoreMediaTypes.end()
                {
                    HandleError(EPUBError::MediaOverlayInvalidAudioType, _Str(item->Href().c_str(), " [", src_file.c_str(), "] => audio source type is invalid"));
                }

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

                SMILData::Audio *audio = new SMILData::Audio(parallel, src_file, srcManifestItem, clipBeginMilliseconds, clipEndMilliseconds);

                sequence = nullptr;
                parallel = nullptr;
            }
            else if (elementName == "text")
            {
                if (parallel == nullptr)
                {
                    HandleError(EPUBError::MediaOverlaySMILTextParallelParent, _Str(item->Href().c_str(), " => parent of text must be parallel time container"));
                }

                if (src_file.empty())
                {
                    HandleError(EPUBError::MediaOverlayInvalidText, _Str(item->Href().c_str(), " => text source is empty"));
                }
                else if (srcManifestItem == nullptr)
                {
                    HandleError(EPUBError::MediaOverlayInvalidTextSource, _Str(item->Href().c_str(), " [", src_file.c_str(), "] => text source manifest cannot be found"));
                }
                else if (src_fragmentID.empty())
                {
                    HandleError(EPUBError::MediaOverlayTextSrcFragmentMissing, _Str(item->Href().c_str(), " [", src_file.c_str(), "] => text source fragment identifier is empty"));
                }

                SMILData::Text *text = new SMILData::Text(parallel, src_file, src_fragmentID, srcManifestItem);

                sequence = nullptr;
                parallel = nullptr;
            }
            else
            {
                HandleError(EPUBError::MediaOverlayUnknownSMILElement, _Str(item->Href().c_str(), "[", elementName.c_str(), "] => unknown SMIL element"));
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

            if (elementName == "body")
            {
                if (sequence->GetChildrenCount() == 0)
                {
                    HandleError(EPUBError::MediaOverlayEmptyBody, _Str(item->Href().c_str(), " => SMIL body has no sequence or parallel time container children"));
                }
            }
            else if (elementName == "seq")
            {
                if (sequence->GetChildrenCount() == 0)
                {
                    HandleError(EPUBError::MediaOverlayEmptySeq, _Str(item->Href().c_str(), " => SMIL sequence time container has no sequence or parallel time container children"));
                }
            }
            else if (elementName == "par")
            {
                if (parallel->Text() == nullptr)
                {
                    HandleError(EPUBError::MediaOverlayEmptyPar, _Str(item->Href().c_str(), " => SMIL parallel time container has no text child"));
                }
            }

            return accumulatedDurationMilliseconds;
        }


        const string& MediaOverlaysSmilModel::Narrator() const
        {
            return Owner()->MediaOverlays_Narrator(false);
        }

        const string& MediaOverlaysSmilModel::ActiveClass() const
        {
            return Owner()->MediaOverlays_ActiveClass();
        }

        const string& MediaOverlaysSmilModel::PlaybackActiveClass() const
        {
            return Owner()->MediaOverlays_PlaybackActiveClass();
        }

        EPUB3_END_NAMESPACE
