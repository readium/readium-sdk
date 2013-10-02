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
#import <ePub3/manifest.h>
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

            //typedef std::map<string, shared_ptr<SMILData>> ManifestItemSMILMap;
            //ManifestItemSMILMap _smilMap; // indexed by SMIL *manifest item* ID (_not_ spine item)
            //for (auto iterator = _smilData.begin(); iterator != _smilData.end(); iterator++)
            //{
            //    string id = iterator->first;
            //    SMILDataPtr smilData = iterator->second;
            //}
            //_smilMap.insert(std::make_pair(key, value));
            //SMILDataPtr smilData = _smilMap[item->Identifier()];

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

            uint32_t DurationMillisecondsTotal() const
            {
                return _totalDuration;
            }

            EPUB3_EXPORT

            std::vector<SMILDataPtr>::size_type GetSmilCount() const
            {
                return _smilDatas.size();
            }

            EPUB3_EXPORT

            SMILDataPtr GetSmil(std::vector<SMILDataPtr>::size_type i) const
            {
                if (i < 0 || i >= _smilDatas.size())
                {
                    return nullptr;
                }

                SMILDataPtr smilData = _smilDatas.at(i);
                return smilData;
            }

            //EPUB3_EXPORT

            static std::vector<string>::size_type GetSkippablesCount()
            {
                return _Skippables.size();
            }

            //EPUB3_EXPORT

            static string GetSkippable(std::vector<string>::size_type i)
            {
                if (i < 0 || i >= _Skippables.size())
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
                if (i < 0 || i >= _Escapables.size())
                {
                    return "";
                }

                string str = _Escapables.at(i);
                return str;
            }

            /*
            EPUB3_EXPORT

            uint32_t DurationMilliseconds(const string manifestItemID)
            {
                SMILDataPtr smilData = getDataForSMILManifestItem(manifestItemID);
                if (smilData == nullptr)
                {
                    return 0;
                }

                return smilData->Duration();
            }
            */

        private:
            static const std::vector<string> _Skippables;
            static const std::vector<string> _Escapables;

            SMILDataPtr getDataForSMILManifestItem(const string id) const
            {
                for (int i = 0; i < _smilDatas.size(); i++)
                {
                    shared_ptr<SMILData> smilData = _smilDatas.at(i);
                    if (smilData->ManifestItem()->Identifier() == id)
                    {
                        return smilData;
                    }
                }

                return nullptr;
            }


            void resetData();

            void populateData();

            void parseMetadata();

            uint32_t parseSMILs();

            uint32_t parseSMIL(SMILDataPtr smilData, SMILData::Sequence *sequence, SMILData::Parallel *parallel, const ManifestItemPtr item, const xmlNodePtr element); // recursive
        };

        EPUB3_END_NAMESPACE

#endif /* defined(ePub3_mo_smil_model_h) */
