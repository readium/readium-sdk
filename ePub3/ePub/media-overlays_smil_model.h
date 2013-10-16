//
//  media-overlays_smil_model.h
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

#ifndef ePub3_mo_smil_model_h
#define ePub3_mo_smil_model_h

#include <ePub3/epub3.h>
#include <ePub3/utilities/owned_by.h>
#include <ePub3/manifest.h>
#include <libxml/xpath.h> // xmlNodePtr
#include "media-overlays_smil_data.h"
#include "package.h"

EPUB3_BEGIN_NAMESPACE

        class Package;

        class MediaOverlaysSmilModel;

        typedef shared_ptr<MediaOverlaysSmilModel> MediaOverlaysSmilModelPtr;


        /**
Parser that reads SMIL XML files into an in-memory data model

See:
http://www.idpf.org/epub/30/spec/epub30-mediaoverlays.html

@see SMILData

@ingroup epub-model
*/
        class MediaOverlaysSmilModel : public std::enable_shared_from_this<MediaOverlaysSmilModel>, public OwnedBy<Package>
        {
        private:
            MediaOverlaysSmilModel() _DELETED_;

            MediaOverlaysSmilModel(const MediaOverlaysSmilModel&) _DELETED_;

            MediaOverlaysSmilModel(MediaOverlaysSmilModel&&) _DELETED_;

            uint32_t _totalDuration; //whole milliseconds (resolution = 1ms)

            std::vector<shared_ptr<SMILData>> _smilDatas;
        public:
            EPUB3_EXPORT

            MediaOverlaysSmilModel(const shared_ptr<Package> package); //PackagePtr

            virtual ~MediaOverlaysSmilModel();

            EPUB3_EXPORT

            void Initialize();

            EPUB3_EXPORT

            const string& Narrator() const;

            EPUB3_EXPORT

            const string& ActiveClass() const;

            EPUB3_EXPORT

            const string& PlaybackActiveClass() const;

            EPUB3_EXPORT

            uint32_t DurationMilliseconds_Metadata() const
            {
                return _totalDuration;
            }

            EPUB3_EXPORT

            const uint32_t DurationMilliseconds_Calculated() const;

            EPUB3_EXPORT

            std::vector<SMILDataPtr>::size_type GetSmilCount() const
            {
                return _smilDatas.size();
            }

            EPUB3_EXPORT

            SMILDataPtr GetSmil(std::vector<SMILDataPtr>::size_type i) const
            {
                if (i >= _smilDatas.size())
                {
                    return nullptr;
                }

                SMILDataPtr smilData = _smilDatas.at(i);
                return smilData;
            }

            EPUB3_EXPORT

            const double PositionToPercent(std::vector<SMILDataPtr>::size_type  smilIndex, uint32_t parIndex, uint32_t milliseconds) const;

            EPUB3_EXPORT

            const void PercentToPosition(double percent, SMILDataPtr & smilData, uint32_t & smilIndex, const SMILData::Parallel* & par, uint32_t & parIndex, uint32_t & milliseconds) const;

            //EPUB3_EXPORT

            static std::vector<string>::size_type GetSkippablesCount()
            {
                return _Skippables.size();
            }

            //EPUB3_EXPORT

            static string GetSkippable(std::vector<string>::size_type i)
            {
                if (i >= _Skippables.size())
                {
                    return "";
                }

                string str = _Skippables.at(i);
                return str;
            }

            //EPUB3_EXPORT

            static std::vector<string>::size_type GetEscapablesCount()
            {
                return _Escapables.size();
            }

            //EPUB3_EXPORT

            static string GetEscapable(std::vector<string>::size_type i)
            {
                if (i >= _Escapables.size())
                {
                    return "";
                }

                string str = _Escapables.at(i);
                return str;
            }

        private:
            static const std::vector<string> _Skippables;
            static const std::vector<string> _Escapables;

            bool _excludeAudioDuration;

            void resetData();

            void populateData();

            void parseMetadata();

            uint32_t parseSMILs();

            uint32_t parseSMIL(SMILDataPtr smilData, SMILData::Sequence *sequence, SMILData::Parallel *parallel, const ManifestItemPtr item, const xmlNodePtr element); // recursive

        protected:
            const SMILData::Parallel *ParallelAt(uint32_t timeMilliseconds) const;
        };

        EPUB3_END_NAMESPACE

#endif /* defined(ePub3_mo_smil_model_h) */
