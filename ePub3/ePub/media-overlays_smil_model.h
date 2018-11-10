//
//  media-overlays_smil_model.h
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

#ifndef ePub3_mo_smil_model_h
#define ePub3_mo_smil_model_h

#include <ePub3/epub3.h>
#include <ePub3/utilities/owned_by.h>
#include <ePub3/manifest.h>
#include <ePub3/xml/node.h>
#include "media-overlays_smil_data.h"
#include "package.h"

//#include <ePub3/utilities/make_unique.h>
//std::unique_ptr<KLASS> obj = make_unique<KLASS>(constructor_params);

EPUB3_BEGIN_NAMESPACE

        class Package;

        class MediaOverlaysSmilModel;

        /**
Parser that reads SMIL XML files into an in-memory data model

See:
http://www.idpf.org/epub/30/spec/epub30-mediaoverlays.html

@see SMILData

@ingroup epub-model
*/
        class MediaOverlaysSmilModel : public std::enable_shared_from_this<MediaOverlaysSmilModel>, public OwnedBy<Package>
#if EPUB_PLATFORM(WINRT)
			, public NativeBridge
#endif
        {
        private:
            MediaOverlaysSmilModel() _DELETED_;

            MediaOverlaysSmilModel(const MediaOverlaysSmilModel &) _DELETED_;

            MediaOverlaysSmilModel(MediaOverlaysSmilModel &&) _DELETED_;

            uint32_t _totalDuration; //whole milliseconds (resolution = 1ms)

            shared_vector<SMILData> _smilDatas;
    
            template <class _Function>
            inline FORCE_INLINE
            _Function ForEachSmilData(_Function __f) const
            {
                // the _smilDatas vector iterator does not make a copy of the smart pointer (NO reference count++),
                // the std::shared_ptr<SMILData> object is not passed as value, but as reference &
                return std::for_each(_smilDatas.begin(), _smilDatas.end(), __f);
            }
    
        public:
            EPUB3_EXPORT

            MediaOverlaysSmilModel(const std::shared_ptr<Package> & package);

            virtual ~MediaOverlaysSmilModel();

            EPUB3_EXPORT

            void Initialize();

            EPUB3_EXPORT

            const string & Narrator() const;

            EPUB3_EXPORT

            const string & ActiveClass() const;

            EPUB3_EXPORT

            const string & PlaybackActiveClass() const;

            EPUB3_EXPORT

            uint32_t DurationMilliseconds_Metadata() const
            {
                return _totalDuration;
            }

            EPUB3_EXPORT

            const uint32_t DurationMilliseconds_Calculated() const;

            EPUB3_EXPORT

            std::vector<std::shared_ptr<SMILData>>::size_type GetSmilCount() const
            {
                return _smilDatas.size();
            }

            EPUB3_EXPORT

            const std::shared_ptr<SMILData> GetSmil(std::vector<std::shared_ptr<SMILData>>::size_type i) const
            {
                if (i >= _smilDatas.size())
                {
                    return nullptr;
                }

                const std::shared_ptr<SMILData> smilData = _smilDatas.at(i); // does not make a copy of the smart pointer (NO reference count++)
                return smilData;
            }

            EPUB3_EXPORT

            const double PositionToPercent(std::vector<std::shared_ptr<SMILData>>::size_type  smilIndex, uint32_t parIndex, uint32_t milliseconds) const;

            EPUB3_EXPORT

            const void PercentToPosition(double percent, SMILDataPtr & smilData, uint32_t & smilIndex, shared_ptr<const SMILData::Parallel>& par, uint32_t & parIndex, uint32_t & milliseconds) const;

            EPUB3_EXPORT

            static std::vector<string>::size_type GetSkippablesCount()
            {
                return _Skippables.size();
            }

            EPUB3_EXPORT

            static string GetSkippable(std::vector<string>::size_type i)
            {
                if (i >= _Skippables.size())
                {
                    return "";
                }

                string str = _Skippables.at(i);
                return str;
            }

            EPUB3_EXPORT

            static std::vector<string>::size_type GetEscapablesCount()
            {
                return _Escapables.size();
            }

            EPUB3_EXPORT

            static string GetEscapable(std::vector<string>::size_type i)
            {
                if (i >= _Escapables.size())
                {
                    return "";
                }

                string str = _Escapables.at(i);
                return str;
            }

        public:
            static const std::vector<string> _Skippables;
            static const std::vector<string> _Escapables;
        private:
            bool _excludeAudioDuration;

            void resetData();

            void populateData();

            void parseMetadata();

            uint32_t parseSMILs();

            uint32_t parseSMIL(SMILDataPtr smilData, shared_ptr<SMILData::Sequence> sequence, shared_ptr<SMILData::Parallel> parallel, const ManifestItemPtr item, shared_ptr<xml::Node> element, std::map<std::shared_ptr<ManifestItem>, string> & cache_manifestItemToAbsolutePath, std::map<string, std::shared_ptr<ManifestItem>> & cache_smilRelativePathToManifestItem); // recursive

        protected:
            shared_ptr<const SMILData::Parallel> ParallelAt(uint32_t timeMilliseconds) const;
        };

        EPUB3_END_NAMESPACE

#endif /* defined(ePub3_mo_smil_model_h) */
