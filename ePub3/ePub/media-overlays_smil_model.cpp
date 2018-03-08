//
//  media-overlays_smil_model.cpp
//  ePub3
//
//  Created by Daniel Weck on 2013-09-15.
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

#include "media-overlays_smil_model.h"
#include "package.h"
#include <ePub3/media-overlays_smil_utils.h>
#include <ePub3/media-overlays_smil_data.h>
#include "error_handler.h"
#include "xpath_wrangler.h"


//#include <iostream>
#include <chrono>


//#include "make_unique.h"
//std::unique_ptr<KLASS> obj = make_unique<KLASS>(constructor_params);

EPUB3_BEGIN_NAMESPACE

class Timer
{

private:
    typedef std::chrono::high_resolution_clock CLOCK;
    typedef std::chrono::duration<double, std::ratio<1>> SECOND;
    std::chrono::time_point<CLOCK> BEGIN;

public:
    Timer() : BEGIN(CLOCK::now())
    {}

    void reset()
    {
        BEGIN = CLOCK::now();
    }

    double elapsed() const
    {
        return std::chrono::duration_cast<SECOND>(CLOCK::now() - BEGIN).count();
    }
};

//     static inline FORCE_INLINE
//     void HandleError_(EPUBError __code, const std::string & __msg)
//     {
// std::cerr << "HANDLE ERROR" << std::endl; 
// 
// //throw std::invalid_argument(std::string("parseSMILs TRACE HERE")); //std::runtime_error ///err.what()
// 
//         HandleError(__code, __msg);
//     }
    

/*
        void debugTreeAudio(const SMILData::Audio * audio)
        {
            printf("-- DEBUG TREE AUDIO\n");

            printf("CHECK SMIL DATA TREE MEDIA SRC %s\n", audio->_src_file.c_str());

            printf("CHECK SMIL DATA TREE AUDIO: %ld --> %ld\n", (long) audio->_clipBeginMilliseconds, (long) audio->_clipEndMilliseconds);
        }

        void debugTreeText(const SMILData::Text * text)
        {
            printf("-- DEBUG TREE TEXT\n");

            printf("CHECK SMIL DATA TREE MEDIA SRC FILE %s\n", text->_src_file.c_str());
            printf("CHECK SMIL DATA TREE MEDIA SRC FRAGID %s\n", text->_src_fragmentID.c_str());

            printf("CHECK SMIL DATA TREE TEXT\n");
        }

        void debugTreePar(const SMILData::Parallel * par)
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

        void debugTreeSeq(const SMILData::Sequence * seqq)
        {
            printf("-- DEBUG TREE SEQ\n");

            printf("CHECK SMIL DATA TREE TEXTREF FILE %s\n", seqq->_textref_file.c_str());
            printf("CHECK SMIL DATA TREE TEXTREF FRAGID %s\n", seqq->_textref_fragmentID.c_str());
            printf("CHECK SMIL DATA TREE TYPE %s\n", seqq->_type.c_str());

            for (int i = 0; i < seqq->_children.size(); i++)
            {
                SMILData::TimeContainer * container = seqq->_children[i];

                const SMILData::Sequence * seq = dynamic_cast<const SMILData::Sequence *>(container);
                if (seq != nullptr)
                {
                    debugTreeSeq(seq);
                    continue;
                }

                const SMILData::Parallel * par = dynamic_cast<const SMILData::Parallel *>(container);
                if (par != nullptr)
                {
                    debugTreePar(par);
                    continue;
                }

                throw std::invalid_argument("WTF?");
            }
        }

        void debugSmilData(std::vector<std::shared_ptr<SMILData>> & smilDatas)
        {
            for (int i = 0; i < smilDatas.size(); i++)
            {
                const std::shared_ptr<SMILData> smilData = smilDatas.at(i);

                printf("}}}}}}} CHECK SMIL DATA TREE %s duration (milliseconds): %ld\n", smilData->ManifestItem()->Identifier().c_str(), (long) smilData->DurationMilliseconds());

                debugTreeSeq(smilData->Body());
            }
        }
*/

        MediaOverlaysSmilModel::~MediaOverlaysSmilModel()
        {
            //printf("~MediaOverlaysSmilModel()\n");
        }

        MediaOverlaysSmilModel::MediaOverlaysSmilModel(const std::shared_ptr<Package> & package) : OwnedBy(package), _totalDuration(0), _smilDatas(std::vector<std::shared_ptr<SMILData>>())
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

            // std::vector automatically releases the contained smart shared pointers (reference count--)

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
                s << "Media Overlays total duration mismatch (milliseconds): METADATA " << (long) _totalDuration << " != SMILs " << (long) totalDurationFromSMILs;
                const std::string & str = _Str(s.str());
                //printf("%s\n", str.c_str());

                //_totalDuration = totalDurationFromSMILs;

                HandleError(EPUBError::MediaOverlayMismatchDurationMetadata, str);
            }
            else
            {
                //printf("Media Overlays SMILs parsed, total duration checked okay (milliseconds): %ld\n", (long) totalDurationFromSMILs);
            }

            //debugSmilData(_smilDatas);
        }

        void MediaOverlaysSmilModel::parseMetadata()
        {
            //const string & _narrator = Narrator();
            //printf("Media Overlays NARRATOR: %s\n", _narrator.c_str());

            //const string & _activeClass = ActiveClass();
            //printf("Media Overlays ACTIVE CLASS: %s\n", _activeClass.c_str());

            //const string & _playbackActiveClass = PlaybackActiveClass();
            //printf("Media Overlays PLAYBACK ACTIVE CLASS: %s\n", _playbackActiveClass.c_str());

            std::shared_ptr<Package> package = Owner(); // internally: std::weak_ptr<Package>.lock()
            if (package == nullptr)
            {
                return;
            }

            const string & durationStr = package->MediaOverlays_DurationTotal();
            //printf("Media Overlays TOTAL DURATION (string): %s\n", durationStr.c_str());

            _totalDuration = 0;
            if (!durationStr.empty())
            {
                try
                {
                    _totalDuration = SmilClockValuesParser::ToWholeMilliseconds(durationStr);
                    //printf("Media Overlays TOTAL DURATION (milliseconds): %ld\n", (long) _totalDuration);
                }
                catch (const std::invalid_argument & error)
                {
                    HandleError(EPUBError::MediaOverlayInvalidSmilClockValue, _Str("OPF package -- media:duration=", durationStr, " => invalid SMIL Clock Value syntax (", error.what(), ")"));
                }
                //catch (...)
                //{
                //}
            }

            std::shared_ptr<MediaOverlaysSmilModel> sharedMe = std::enable_shared_from_this<MediaOverlaysSmilModel>::shared_from_this();
            //std::weak_ptr<MediaOverlaysSmilModel> weakSharedMe = sharedMe; // Not needed: smart shared pointer passed as reference, then onto OwnedBy()
            
            uint32_t accumulatedDurationMilliseconds = 0;

            // const ManifestTable & manifestTable = package->Manifest();
            // for (ManifestTable::const_iterator iter = manifestTable.begin(); iter != manifestTable.end(); iter++)
            // std::shared_ptr<ManifestItem> item = iter->second;

            bool allFake = true;

            std::shared_ptr<SpineItem> spineItem = package->FirstSpineItem();
            while (spineItem != nullptr)
            {
                ManifestItemPtr item = spineItem->ManifestItem();

                //string
                //const ManifestItem::MimeType & mediaType = item->MediaType();
                //if (mediaType != "application/smil+xml")

                //const string & mediaOverlayID = item->MediaOverlayID();
                //printf("--- Media Overlays ID: %s\n", mediaOverlayID.c_str());
                item = item->MediaOverlay();
                if (item == nullptr)
                {
                    const std::shared_ptr<SMILData> smilData = std::make_shared<class SMILData>(sharedMe, nullptr, spineItem, 0);
                    _smilDatas.push_back(smilData); // creates a *copy* of the shared smart pointer (reference count++)

                    spineItem = spineItem->Next();
                    continue;
                }

                //printf("Media Overlays SMIL HREF: %s\n", item->Href().c_str());

                const string & itemDurationStr = package->MediaOverlays_DurationItem(item);
                //printf("Media Overlays SMIL DURATION (string): %s\n", itemDurationStr.c_str());

                if (itemDurationStr.empty())
                {
                    allFake = false;

                    const std::shared_ptr<SMILData> smilData = std::make_shared<class SMILData>(sharedMe, item, spineItem, 0);
                    _smilDatas.push_back(smilData); // creates a *copy* of the shared smart pointer (reference count++)

                    HandleError(EPUBError::MediaOverlayMissingDurationMetadata, _Str(item->Href(), " => missing media:duration metadata"));
                }
                else
                {
                    uint32_t durationWholeMilliseconds = 0;
                    try
                    {
                        durationWholeMilliseconds = SmilClockValuesParser::ToWholeMilliseconds(itemDurationStr);
                        //printf("Media Overlays SMIL DURATION (milliseconds): %ld\n", (long) durationWholeMilliseconds);

                        allFake = false;

                        const std::shared_ptr<SMILData> smilData = std::make_shared<class SMILData>(sharedMe, item, spineItem, durationWholeMilliseconds);
                        _smilDatas.push_back(smilData); // creates a *copy* of the shared smart pointer (reference count++)

                        accumulatedDurationMilliseconds += durationWholeMilliseconds;
                    }
                    catch (const std::invalid_argument & error)
                    {
                        allFake = false;

                        const std::shared_ptr<SMILData> smilData = std::make_shared<class SMILData>(sharedMe, item, spineItem, 0);
                        _smilDatas.push_back(smilData); // creates a *copy* of the shared smart pointer (reference count++)

                        HandleError(EPUBError::MediaOverlayInvalidSmilClockValue, _Str(item->Href(), " -- media:duration=", itemDurationStr, " => invalid SMIL Clock Value syntax (", error.what(), ")"));
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
                    s << "Media Overlays metadata duration mismatch (milliseconds): TOTAL " << (long) _totalDuration << " != ACCUMULATED " << (long) accumulatedDurationMilliseconds;
                    const std::string & str = _Str(s.str());
                    //printf("%s\n", str.c_str());
                    HandleError(EPUBError::MediaOverlayMismatchDurationMetadata, str);
                }
            }
            else
            {
                //printf("Media Overlays SMIL DURATION check okay.\n");
            }

            if (allFake)
            {
                // std::vector automatically releases the contained smart shared pointers (reference count--)
                
                //_smilDatas.erase(_smilDatas.begin(), _smilDatas.end());
                _smilDatas.clear();
            }
            else
            {
                ForEachSmilData([](const std::shared_ptr<SMILData> & data)
                {
                    if (data->SmilManifestItem() != nullptr)
                    {
                        return;
                    }
                    
                    printf("SMIL placeholder for 'blank' MO page: %s\n", data->XhtmlSpineItem()->ManifestItem()->Href().c_str());

                    data->_root = std::make_shared<SMILData::Sequence>(nullptr, "", "", nullptr, "", data);

                    shared_ptr<SMILData::Sequence> sequence = std::const_pointer_cast<SMILData::Sequence>(data->Body());

                    shared_ptr<SMILData::Parallel> par = std::make_shared<SMILData::Parallel>(sequence, "", data);
					sequence->_children.push_back(par);

                    par->_text = std::make_shared<SMILData::Text>(par, data->XhtmlSpineItem()->ManifestItem()->Href(), "", nullptr, data);
                });
            }
        }

        uint32_t MediaOverlaysSmilModel::parseSMILs()
        {       
            std::shared_ptr<Package> package = Owner(); // internally: std::weak_ptr<Package>.lock()
            if (package == nullptr)
            {
                return 0;
            }

//Timer timer;
            //uint counter = 0;

            std::map<std::shared_ptr<ManifestItem>, string> cache_manifestItemToAbsolutePath;

            uint32_t accumulatedDurationMilliseconds = 0;


            shared_ptr<SpineItem> spineItem = package->FirstSpineItem();
            while (spineItem != nullptr)
            {
                ManifestItemPtr item = spineItem->ManifestItem();

                //string
                //const ManifestItem::MimeType & mediaType = item->MediaType();
                //if (mediaType != "application/smil+xml")

                //const string & mediaOverlayID = item->MediaOverlayID();
                //printf("--- Media Overlays ID: %s\n", mediaOverlayID.c_str());
                item = item->MediaOverlay();
                if (item == nullptr)
                {
                    spineItem = spineItem->Next();
                    continue;
                }

//                counter++;
//                if (counter > 10)
//                {
//                    break;
//                }

                //printf("Media Overlays SMIL PARSING: %s\n", item->Href().c_str());

                //unique_ptr<ArchiveXmlReader> xmlReader = package->XmlReaderForRelativePath(item->Href());
                shared_ptr<xml::Document> doc = item->ReferencedDocument();
                if (!bool(doc))
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

                xml::NodeSet nodes = xpath.Nodes("/smil:smil");

                if (nodes.empty())
                {
                    HandleError(EPUBError::MediaOverlayInvalidRootElement, _Str("'smil' root element not found: ", item->Href().c_str()));
                }
                else if (nodes.size() > 1)
                {
                    HandleError(EPUBError::MediaOverlayInvalidRootElement, _Str("Multiple 'smil' root elements found: ", item->Href().c_str()));
                }

                if (nodes.size() != 1)
                    return 0;

                shared_ptr<xml::Node> smil = nodes[0];

                string version = _getProp(smil, "version", SMILNamespaceURI);
                if (version.empty())
                {
                    HandleError(EPUBError::MediaOverlayVersionMissing, _Str("SMIL version not found: ", item->Href().c_str()));
                }
                else if (version != "3.0")
                {
                    HandleError(EPUBError::MediaOverlayInvalidVersion, _Str("Invalid SMIL version (", version, "): ", item->Href().c_str()));
                }

                nodes = xpath.Nodes("./smil:head", smil);

                if (nodes.empty())
                {
                    // OKAY
                }
                else if (nodes.size() == 1)
                {
                    //TODO: check head placement
                    //HandleError(EPUBError::MediaOverlayHeadIncorrectlyPlaced, _Str("'head' element incorrectly placed: ", item->Href().c_str()));
                }
                else if (nodes.size() > 1)
                {
                    HandleError(EPUBError::MediaOverlayHeadIncorrectlyPlaced, _Str("multiple 'head' elements found: ", item->Href().c_str()));
                    return 0;
                }

                nodes = xpath.Nodes("./smil:body", smil);

                if (nodes.empty())
                {
                    HandleError(EPUBError::MediaOverlayNoBody, _Str("'body' element not found: ", item->Href().c_str()));
                }
                else if (nodes.size() > 1)
                {
                    HandleError(EPUBError::MediaOverlayMultipleBodies, _Str("multiple 'body' elements found: ", item->Href().c_str()));
                }

                if (nodes.size() != 1)
                {
                    return 0;
                }

                shared_ptr<SMILData> smilData = nullptr;
                string id = item->Identifier();
                for (shared_vector<SMILData>::size_type i = 0; i < _smilDatas.size(); i++)
                {
                    const std::shared_ptr<SMILData> data = _smilDatas.at(i); // does not make a copy of the smart pointer (NO reference count++)

                    if (data->SmilManifestItem() == nullptr)
                    {
                        continue;
                    }

                    if (data->SmilManifestItem()->Identifier() == id)
                    {
                        auto seq = data->Body();
                        if (seq != nullptr)
                        {
                            continue;
                        }

                        if (data->XhtmlSpineItem()->ManifestItem()->Identifier() != spineItem->ManifestItem()->Identifier())
                        {
                            printf("XHTML spine item's manifest item ID mismatch?!\n");
                        }
                        smilData = data;
                        break;
                    }
                }

                shared_ptr<xml::Node> body = nodes[0];

                std::map<string, std::shared_ptr<ManifestItem>> cache_smilRelativePathToManifestItem;

//// TIMER START
//timer.reset();

                uint32_t smilDur = parseSMIL(smilData, nullptr, nullptr, item, body, cache_manifestItemToAbsolutePath, cache_smilRelativePathToManifestItem);
                //printf("Media Overlays SMIL DURATION (milliseconds): %ld\n", (long) smilDur);

//// TIMER END
//double seconds = timer.elapsed();
//std::stringstream s;
//s << "TIMER (" << counter << "): " << seconds;
//const std::string & str = _Str(s.str());
//printf("%s\n", str.c_str());
////std::cout << t << std::endl;

                uint32_t metaDur = smilData->DurationMilliseconds_Metadata();
                if (metaDur != smilDur)
                {
                    std::stringstream s;
                    s << "Media Overlays SMIL duration mismatch (milliseconds): METADATA " << (long) metaDur << " != SMIL " << (long) smilDur << " (" << item->Href().c_str() << ")";
                    const std::string & str = _Str(s.str());
                    //printf("%s\n", str.c_str());

                    //smilData->_duration = smilDur;

                    HandleError(EPUBError::MediaOverlayMismatchDurationMetadata, str);
                }

                accumulatedDurationMilliseconds += smilDur;

                spineItem = spineItem->Next();
            }

            return accumulatedDurationMilliseconds;
        }

        static std::vector<string> splitFileFragmentId; // = std::vector<string>(2);

        void splitIriFileFragmentID(const string & iri, std::vector<string> &splitFileFragmentId)
        {
            //printf("=========== IRI: %s\n", iri.c_str());

            splitFileFragmentId.clear();

            const char * str = iri.c_str();
            size_t size = strlen(str);
            for (size_t j = 0; j < size; j++)
            {
                char c = str[j];
                if (c == '#')
                {
                    splitFileFragmentId.push_back(string(str, j));

                    j++;
                    if (size > j)
                    {
                        splitFileFragmentId.push_back(string(str + j));
                    }
                    else
                    {
                        splitFileFragmentId.push_back("");
                    }

                    return;
                }
            }
            splitFileFragmentId.push_back(iri);
            splitFileFragmentId.push_back("");
//
//            // FILE
//            string::size_type i = iri.find_first_of('#');
//            if (i != string::npos && i > 0)
//            {
//                splitFileFragmentId.push_back(iri.substr(0, i));
//            }
//            else
//            {
//                splitFileFragmentId.push_back(iri); //string(iri)
//            }
//
//            // FRAGMENT ID
//            if (i != string::npos && i < (iri.length() - 1))
//            {
//                string::size_type n = iri.length() - i - 1;
//                splitFileFragmentId.push_back(iri.substr(i + 1, n));
//            }
//            else
//            {
//                splitFileFragmentId.push_back("");
//            }
//
//            //printf("=========== IRI FILE: %s\n", split.at(0).c_str());
//            //printf("=========== IRI FRAGID: %s\n", split.at(1).c_str());
//
//            // RVO Return Value Optimisation should take care of moving the object reference from the local stack that of the callee's context?
//            //return split;
        }

        std::shared_ptr<ManifestItem> getReferencedManifestItem(const std::shared_ptr<Package> & package, string filepathInSmil, const std::shared_ptr<ManifestItem> & smilItem, std::map<std::shared_ptr<ManifestItem>, string> & cache_manifestItemToAbsolutePath)
        {
            if (filepathInSmil.empty())
            {
                return nullptr;
            }

            std::map<std::shared_ptr<ManifestItem>, string>::iterator iterator = cache_manifestItemToAbsolutePath.find(smilItem);
            string smilOPFPath;
            if (iterator != cache_manifestItemToAbsolutePath.end()
                //cache_manifestItemToAbsolutePath.count(smilItem)
            )
            {
                //smilOPFPath = cache_manifestItemToAbsolutePath[smilItem];
                smilOPFPath = iterator->second;

                //printf("=========== smilOPFPath FROM CACHE: %s\n", smilOPFPath.c_str());
            }
            else
            {
                smilOPFPath = smilItem->AbsolutePath();
                cache_manifestItemToAbsolutePath[smilItem] = smilOPFPath;

                //printf("=========== smilOPFPath CACHING: %s\n", smilOPFPath.c_str());
            }


            //printf("=========== smilOPFPath: %s\n", smilOPFPath.c_str());

            string::size_type i = smilOPFPath.find_last_of('/');
            string smilParentFolder = "/";
            if (i != string::npos && i > 0)
            {
                smilParentFolder = _Str(smilOPFPath.substr(0, i), '/');
            }
            //printf("=========== smilParentFolder: %s\n", smilParentFolder.c_str());

            std::string refOPFPath = _Str(smilParentFolder, filepathInSmil);

            //std::string & refOPFPath = _Str("/.././void1/../", smilParentFolder, "./void2/.././", filepathInSmil);
            //std::string & refOPFPath = _Str(smilParentFolder, "void/../", filepathInSmil);
            //std::string & refOPFPath = _Str("../", smilParentFolder, filepathInSmil);

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


            // When content does not have a root folder such as "OEBPS" or "EPUB", manifest absolute paths have a '/' prefix!!
// //std::shared_ptr<Package> package = Owner(); // internally: std::weak_ptr<Package>.lock()
//            if (package != nullptr)
//            {
//                string basePath = package->BasePath();
//                printf("=========== basePath: %s\n", basePath.c_str());
//            }
            if (smilOPFPath.at(0) == '/' && refOPFPath.at(0) != '/')
            {
                //refOPFPath = _Str(smilOPFPath.substr(0, 1), refOPFPath);
                refOPFPath.insert(0, "/");
            }

            const ManifestTable & manifestTable = package->Manifest();
            for (ManifestTable::const_iterator iter = manifestTable.begin(); iter != manifestTable.end(); iter++)
            {
                std::shared_ptr<ManifestItem> manItem = iter->second;

                string abs;
                iterator = cache_manifestItemToAbsolutePath.find(manItem);
                if (iterator != cache_manifestItemToAbsolutePath.end()
                        //cache_manifestItemToAbsolutePath.count(manItem)
                        )
                {
                    //abs = cache_manifestItemToAbsolutePath[manItem];
                    abs = iterator->second;

                    //printf("=========== abs FROM CACHE: %s\n", abs.c_str());
                }
                else
                {
                    abs = manItem->AbsolutePath();
                    cache_manifestItemToAbsolutePath[manItem] = abs;

                    //printf("=========== abs CACHING: %s\n", abs.c_str());
                }

                if (abs.compare(refOPFPath) == 0)
                {
                    //printf("------> manifest item path: %s\n", abs.c_str());
                    
                    return manItem; //unless RVO (Return Value Optimization), creates a copy of the smart pointer (reference count++), and deallocates the local variable (reference count--)
                }
            }

            //printf("------> manifest item path NOT FOUND??!: [%s] %s\n", smilOPFPath.c_str(), filepathInSmil.c_str());
            return nullptr;
        }

        uint32_t MediaOverlaysSmilModel::parseSMIL(SMILDataPtr smilData, shared_ptr<SMILData::Sequence> sequence, shared_ptr<SMILData::Parallel> parallel, const ManifestItemPtr item, shared_ptr<xml::Node> element, std::map<std::shared_ptr<ManifestItem>, string> & cache_manifestItemToAbsolutePath, std::map<string, std::shared_ptr<ManifestItem>> & cache_smilRelativePathToManifestItem)
        {
            if (!bool(element) || !element->IsElementNode())
            {
                return 0;
            }

            uint32_t accumulatedDurationMilliseconds = 0;

            string elementName = element->Name();

            string textref_ = string(_getProp(element, "textref", ePub3NamespaceURI));
            string textref_file;
            string textref_fragmentID;
            if (!textref_.empty())
            {
                //printf("=========== TEXTREF: %s\n", textref.c_str());

                splitIriFileFragmentID(textref_, splitFileFragmentId);

                textref_file = splitFileFragmentId[0];
                textref_fragmentID = splitFileFragmentId[1];
//
//                printf("=========== TEXTREF FILE: %s\n", textref_file.c_str());
//                printf("=========== TEXTREF FRAGID: %s\n", textref_fragmentID.c_str());
            }

            string src_ = string(_getProp(element, "src", SMILNamespaceURI));
            string src_file;
            string src_fragmentID;
            if (!src_.empty())
            {
                //printf("=========== SRC: %s\n", src.c_str());

                splitIriFileFragmentID(src_, splitFileFragmentId);

                src_file = splitFileFragmentId[0];
                src_fragmentID = splitFileFragmentId[1];
//
//                printf("=========== SRC FILE: %s\n", src_file.c_str());
//                printf("=========== SRC FRAGID: %s\n", src_fragmentID.c_str());
            }

            std::shared_ptr<Package> package = Owner(); // internally: std::weak_ptr<Package>.lock()
            if (package == nullptr)
            {
                return 0;
            }

            std::map<string, std::shared_ptr<ManifestItem>>::iterator iterator = cache_smilRelativePathToManifestItem.find(src_file);
            std::shared_ptr<ManifestItem> srcManifestItem;
            if (iterator != cache_smilRelativePathToManifestItem.end()
                    //cache_smilRelativePathToManifestItem.count(src_file)
                    )
            {
                //srcManifestItem = cache_smilRelativePathToManifestItem[src_file];
                srcManifestItem = iterator->second;

                //printf("=========== srcManifestItem FROM CACHE: %s\n", src_file.c_str());
            }
            else
            {
                srcManifestItem = getReferencedManifestItem(package, src_file, item, cache_manifestItemToAbsolutePath);
                cache_smilRelativePathToManifestItem[src_file] = srcManifestItem;

                //printf("=========== srcManifestItem CACHING: %s\n", src_file.c_str());
            }

            iterator = cache_smilRelativePathToManifestItem.find(textref_file);
            std::shared_ptr<ManifestItem> textrefManifestItem;
            if (iterator != cache_smilRelativePathToManifestItem.end()
                    //cache_smilRelativePathToManifestItem.count(textref_file)
                    )
            {
                //textrefManifestItem = cache_smilRelativePathToManifestItem[textref_file];
                textrefManifestItem = iterator->second;

                //printf("=========== textrefManifestItem FROM CACHE: %s\n", textref_file.c_str());
            }
            else
            {
                textrefManifestItem = getReferencedManifestItem(package, textref_file, item, cache_manifestItemToAbsolutePath);
                cache_smilRelativePathToManifestItem[textref_file] = textrefManifestItem;

                //printf("=========== textrefManifestItem CACHING: %s\n", textref_file.c_str());
            }

            string type = string(_getProp(element, "type", ePub3NamespaceURI));

            if (elementName == "body")
            {
                if (!textref_file.empty() && textrefManifestItem == nullptr)
                {
                    //printf("Media Overlays TEXT REF error: %s [%s]\n", textref_file.c_str(), item->Href().c_str());
                    HandleError(EPUBError::MediaOverlayInvalidTextRefSource, _Str(item->Href().c_str(), " [", textref_file.c_str(), "] => text ref manifest cannot be found"));
                }

                smilData->_root = std::make_shared<SMILData::Sequence>(nullptr, textref_file, textref_fragmentID, textrefManifestItem, type, smilData);

                sequence = std::const_pointer_cast<SMILData::Sequence>(smilData->Body());

                parallel = nullptr;
            }
            else if (elementName == "seq")
            {
                if (!textref_file.empty() && textrefManifestItem == nullptr)
                {
                    //printf("Media Overlays TEXT REF error: %s\n", textref_file.c_str());
                    HandleError(EPUBError::MediaOverlayInvalidTextRefSource, _Str(item->Href().c_str(), " [", textref_file.c_str(), "] => text ref manifest cannot be found"));
                }

                if (sequence == nullptr)
                {
                    HandleError(EPUBError::MediaOverlaySMILSequenceSequenceParent, _Str(item->Href().c_str(), " => parent of sequence time container must be sequence"));
                }

                shared_ptr<SMILData::Sequence> seq = std::make_shared<SMILData::Sequence>(sequence, textref_file, textref_fragmentID, textrefManifestItem, type, smilData);
				sequence->_children.push_back(seq);

                sequence = seq;
                parallel = nullptr;
            }
            else if (elementName == "par")
            {
                if (!textref_file.empty() && textrefManifestItem == nullptr)
                {
                    //printf("Media Overlays TEXT REF error: %s\n", textref_file.c_str());
                    HandleError(EPUBError::MediaOverlayInvalidTextRefSource, _Str(item->Href().c_str(), " [", textref_file.c_str(), "] => text ref manifest cannot be found"));
                }

                if (sequence == nullptr)
                {
                    HandleError(EPUBError::MediaOverlaySMILParallelSequenceParent, _Str(item->Href().c_str(), " => parent of parallel time container must be sequence"));
                }

                shared_ptr<SMILData::Parallel> par = std::make_shared<SMILData::Parallel>(sequence, type, smilData);
				sequence->_children.push_back(par);

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
                else if (srcManifestItem->MediaType() != "audio/mpeg" && srcManifestItem->MediaType() != "audio/mp4") //package->CoreMediaTypes.find(mediaType) == package->CoreMediaTypes.end()
                {
                    HandleError(EPUBError::MediaOverlayInvalidAudioType, _Str(item->Href().c_str(), " [", src_file.c_str(), " (", srcManifestItem->MediaType().c_str(), ")] => audio source type is invalid"));
                }

                uint32_t clipBeginMilliseconds = 0;
                uint32_t clipEndMilliseconds = 0;

                string clipBeginStr = _getProp(element, "clipBegin", SMILNamespaceURI);
                //printf("Media Overlays CLIP BEGIN: %s\n", clipBeginStr.c_str());
                if (!clipBeginStr.empty())
                {
                    try
                    {
                        clipBeginMilliseconds = SmilClockValuesParser::ToWholeMilliseconds(clipBeginStr);
                    }
                    catch (const std::invalid_argument & error)
                    {
                        HandleError(EPUBError::MediaOverlayInvalidSmilClockValue, _Str(item->Href().c_str(), " -- clipBegin=", clipBeginStr, " => invalid SMIL Clock Value syntax (", error.what(), ")"));
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
                        clipEndMilliseconds = SmilClockValuesParser::ToWholeMilliseconds(clipEndStr);
                    }
                    catch (const std::invalid_argument & error)
                    {
                        HandleError(EPUBError::MediaOverlayInvalidSmilClockValue, _Str(item->Href().c_str(), " -- clipEnd=", clipEndStr, " => invalid SMIL Clock Value syntax (", error.what(), ")"));
                    }
                    //catch (...)
                    //{
                    //}
                }

                if (clipEndStr.empty() || clipEndMilliseconds <= clipBeginMilliseconds)
                {
                    //TODO: get intrinsic audio file duration
                    clipEndMilliseconds = 0; // means NULL
                }

                int32_t clipDuration = clipEndMilliseconds - clipBeginMilliseconds;

                if (clipDuration <= 0)
                {
                    //HandleError(EPUBError::MediaOverlayInvalidSmilClockValue, _Str(item->Href().c_str(), " -- clipBegin=", clipBeginStr, ", clipEnd=", clipEndStr, " => invalid time values"));
                }
                else
                {
                    accumulatedDurationMilliseconds += clipDuration;
                }

                parallel->_audio = std::make_shared<SMILData::Audio>(parallel, src_file, srcManifestItem, clipBeginMilliseconds, clipEndMilliseconds, smilData);

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

                std::shared_ptr<ManifestItem> spineManifestItem = smilData->_spineItem->ManifestItem();
                if (srcManifestItem != spineManifestItem)
                {
                    printf("Media Overlays TEXT SRC mismatch (SMIL[1] with XHTML[1+]): %s (%s) [%s]\n", srcManifestItem->Href().c_str(), spineManifestItem->Href().c_str(), item->Href().c_str());
                    _excludeAudioDuration = true;
                }

                parallel->_text = std::make_shared<SMILData::Text>(parallel, src_file, src_fragmentID, srcManifestItem, smilData);

                sequence = nullptr;
                parallel = nullptr;
            }
            else
            {
                HandleError(EPUBError::MediaOverlayUnknownSMILElement, _Str(item->Href().c_str(), "[", elementName.c_str(), "] => unknown SMIL element"));
            }

            shared_ptr<xml::Node> childNode = element->FirstElementChild();
            if (bool(childNode))
            {
                _excludeAudioDuration = false;

                for (; bool(childNode); childNode = childNode->NextElementSibling())
                {
                    uint32_t time = parseSMIL(smilData, sequence, parallel, item, childNode, cache_manifestItemToAbsolutePath, cache_smilRelativePathToManifestItem);

                    if (elementName != "par" || !_excludeAudioDuration)
                    {
                        accumulatedDurationMilliseconds += time;
                    }
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


        const string & MediaOverlaysSmilModel::Narrator() const
        {
            std::shared_ptr<Package> pack = Owner(); // internally: std::weak_ptr<Package>.lock()
            if (pack == nullptr)
            {
                return string::EmptyString;
            }
            return pack->MediaOverlays_Narrator(false);
        }

        const string & MediaOverlaysSmilModel::ActiveClass() const
        {
            std::shared_ptr<Package> pack = Owner(); // internally: std::weak_ptr<Package>.lock()
            if (pack == nullptr)
            {
                return string::EmptyString;
            }
            return pack->MediaOverlays_ActiveClass();
        }

        const string & MediaOverlaysSmilModel::PlaybackActiveClass() const
        {
            std::shared_ptr<Package> pack = Owner(); // internally: std::weak_ptr<Package>.lock()
            if (pack == nullptr)
            {
                return string::EmptyString;
            }
            return pack->MediaOverlays_PlaybackActiveClass();
        }

        const uint32_t MediaOverlaysSmilModel::DurationMilliseconds_Calculated() const
        {
            uint32_t total = 0;

            ForEachSmilData([&total](const std::shared_ptr<SMILData> & data)
            {
                total += data->DurationMilliseconds_Calculated();
            });

            return total;
        }

        shared_ptr<const SMILData::Parallel> MediaOverlaysSmilModel::ParallelAt(uint32_t timeMilliseconds) const
        {
            uint32_t offset = 0;

            for (shared_vector<SMILData>::size_type i = 0; i < _smilDatas.size(); i++)
            {
                const std::shared_ptr<SMILData> data = _smilDatas.at(i); // does not make a copy of the smart pointer (NO reference count++)
                uint32_t timeAdjusted = timeMilliseconds - offset;

                shared_ptr<const SMILData::Parallel> para = data->ParallelAt(timeAdjusted);
                if (para != nullptr)
                {
                    return para;
                }

                offset += data->DurationMilliseconds_Calculated();
            }

            return nullptr;
        }

        const void MediaOverlaysSmilModel::PercentToPosition(double percent, SMILDataPtr & smilData, uint32_t & smilIndex, shared_ptr<const SMILData::Parallel>& par, uint32_t & parIndex, uint32_t & milliseconds) const
        {
            if (percent < 0.0 || percent > 100.0)
            {
                percent = 0.0;
            }

            uint32_t total = DurationMilliseconds_Calculated();

            uint32_t timeMs = (uint32_t) (total * (percent / 100.0));

            //printf("=== TIME SCRUB: %ldms / %ldms (==%ldms)", (long) timeMs, (long) total, (long) mo->DurationMillisecondsTotal());

            par = ParallelAt(timeMs);
            if (par == nullptr)
            {
                return;
            }

            std::shared_ptr<SMILData> smilDataPar = par->Owner(); // internally: std::weak_ptr<SMILData>.lock()
            if (smilDataPar == nullptr)
            {
                return;
            }
            
            uint32_t smilDataOffset = 0;
            for (std::vector<std::shared_ptr<SMILData>>::size_type i = 0; i < GetSmilCount(); i++)
            {
                smilData = GetSmil(i);
                if (smilData == smilDataPar) // smart shared pointer comparison operator overloaded
                {
                    break;
                }
                smilDataOffset += smilData->DurationMilliseconds_Calculated();
            }

            milliseconds = timeMs - (smilDataOffset + smilData->ClipOffset(par));
        }

        const double MediaOverlaysSmilModel::PositionToPercent(std::vector<std::shared_ptr<SMILData>>::size_type smilIndex, uint32_t parIndex, uint32_t milliseconds) const
        {
            if (smilIndex >= GetSmilCount())
            {
                return -1.0;
            }

            uint32_t smilDataOffset = 0;
            for (std::vector<std::shared_ptr<SMILData>>::size_type i = 0; i < smilIndex; i++)
            {
                const std::shared_ptr<SMILData> sd = GetSmil(i);
                smilDataOffset += sd->DurationMilliseconds_Calculated();
            }

            const std::shared_ptr<SMILData> smilData = GetSmil(smilIndex);

            shared_ptr<const SMILData::Parallel> par = smilData->NthParallel(parIndex);
            if (par == nullptr)
            {
                return -1.0;
            }

            uint32_t offset = smilDataOffset + smilData->ClipOffset(par) + milliseconds;

            uint32_t total = DurationMilliseconds_Calculated();

            double percent = ((double) offset / (double) total) * 100.0;

            return percent;

            //printf("=== TIME SCRUB [%f%] %ldms / %ldms (==%ldms)", percent, (long) offset, (long) total, (long) mo->DurationMillisecondsTotal());
        }

        // http://www.idpf.org/epub/vocab/structure
        // http://www.idpf.org/epub/30/spec/epub30-mediaoverlays.html#sec-skippability
        const std::vector<string> MediaOverlaysSmilModel::_Skippables = {
                "sidebar",
                "practice",
                "marginalia",
                "annotation",
                "help",
                "note",
                "footnote",
                "rearnote",
                "table",
                "table-row",
                "table-cell",
                "list",
                "list-item",
                "pagebreak"
        };

        // http://www.idpf.org/epub/vocab/structure
        // http://www.idpf.org/epub/30/spec/epub30-mediaoverlays.html#sec-escabaility
        const std::vector<string> MediaOverlaysSmilModel::_Escapables = {
                "sidebar",
                "bibliography",
                "toc",
                "loi",
                "appendix",
                "landmarks",
                "lot",
                "index",
                "colophon",
                "epigraph",
                "conclusion",
                "afterword",
                "warning",
                "epilogue",
                "foreword",
                "introduction",
                "prologue",
                "preface",
                "preamble",
                "notice",
                "errata",
                "copyright-page",
                "acknowledgments",
                "other-credits",
                "titlepage",
                "imprimatur",
                "contributors",
                "halftitlepage",
                "dedication",
                "help",
                "annotation",
                "marginalia",
                "practice",
                "note",
                "footnote",
                "rearnote",
                "footnotes",
                "rearnotes",
                "bridgehead",
                "page-list",
                "table",
                "table-row",
                "table-cell",
                "list",
                "list-item",
                "glossary"
        };


        EPUB3_END_NAMESPACE
