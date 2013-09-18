//
//  media-overlays_smil_data.h
//  ePub3
//
//  Created by Daniel Weck on 2013-09-18.
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

#ifndef ePub3_mo_smil_data_h
#define ePub3_mo_smil_data_h

#include <ePub3/epub3.h>
#include <ePub3/utilities/owned_by.h>
#import <ePub3/manifest.h>

EPUB3_BEGIN_NAMESPACE

        class MediaOverlaysSmilModel;

        class SMILData;

        typedef shared_ptr<SMILData> SMILDataPtr;

        class SMILData : public std::enable_shared_from_this<SMILData>, public OwnedBy<MediaOverlaysSmilModel>
        {
            friend class MediaOverlaysSmilModel;

        public:

            class Sequence;

            class Parallel;

            class TimeContainer;


            class TimeNode
            {
            private:
                TimeNode() _DELETED_;

                TimeNode(const TimeNode&) _DELETED_;

                TimeNode(TimeNode&&) _DELETED_;

            public:
                virtual const string& Name() const
                {
                    return string::EmptyString;
                }

                virtual ~TimeNode()
                {
                }

                TimeNode(TimeContainer *parent):_parent(parent)
                {
                }

                TimeContainer *_parent;
            };

            class TimeContainer : public TimeNode
            {
            private:
                TimeContainer() _DELETED_;

                TimeContainer(const TimeContainer&) _DELETED_;

                TimeContainer(TimeContainer&&) _DELETED_;

            public:
                virtual ~TimeContainer()
                {
                }

                TimeContainer(Sequence *parent, string textref_file, string textref_fragmentID, ManifestItemPtr textrefManifestItem, string type):TimeNode(parent), _textref_file(textref_file), _textref_fragmentID(textref_fragmentID), _textrefManifestItem(textrefManifestItem), _type(type)
                {
                    if (parent != nullptr)
                    {
                        parent->_children.push_back(this);
                    }
                }

                string _textref_file;
                string _textref_fragmentID;

                ManifestItemPtr _textrefManifestItem;

                string _type; // space-separated
            };

            class Sequence : public TimeContainer
            {
            private:
                Sequence() _DELETED_;

                Sequence(const Sequence&) _DELETED_;

                Sequence(Sequence&&) _DELETED_;

                static const string _Name;
            public:
                const string& Name() const
                {
                    return _Name;
                }

                virtual ~Sequence()
                {
                    for (int i = 0; i < _children.size(); i++)
                    {
                        TimeContainer *o = _children[i];
                        if (o != nullptr)
                        {
                            delete o;
                        }
                    }
                }

                Sequence(Sequence *parent, string textref_file, string textref_fragmentID, ManifestItemPtr textrefManifestItem, string type):TimeContainer(parent, textref_file, textref_fragmentID, textrefManifestItem, type)
                {
                    _children = std::vector<TimeContainer *>();
                }

                std::vector<TimeContainer *> _children;
            };

            class Media : public TimeNode
            {
            private:
                Media() _DELETED_;

                Media(const Media&) _DELETED_;

                Media(Media&&) _DELETED_;

            public:
                virtual ~Media()
                {
                }

                Media(Parallel *parent, string src_file, string src_fragmentID, ManifestItemPtr srcManifestItem):TimeNode(parent), _src_file(src_file), _src_fragmentID(src_fragmentID), _srcManifestItem(srcManifestItem)
                {
                }

                string _src_file;
                string _src_fragmentID;
                ManifestItemPtr _srcManifestItem;
            };

            class Audio : public Media
            {
            private:
                Audio() _DELETED_;

                Audio(const Audio&) _DELETED_;

                Audio(Audio&&) _DELETED_;

                static const string _Name;
            public:
                const string& Name() const
                {
                    return _Name;
                }

                virtual ~Audio()
                {
                }

                Audio(Parallel *parent, string src, ManifestItemPtr srcManifestItem, uint32_t clipBeginMilliseconds, uint32_t clipEndMilliseconds):Media(parent, src, "", srcManifestItem), _clipBeginMilliseconds(clipBeginMilliseconds), _clipEndMilliseconds(clipEndMilliseconds)
                {
                    parent->_audio = this;
                }

                uint32_t _clipBeginMilliseconds;
                uint32_t _clipEndMilliseconds;
            };

            class Text : public Media
            {
            private:
                Text() _DELETED_;

                Text(const Text&) _DELETED_;

                Text(Text&&) _DELETED_;

                static const string _Name;
            public:
                const string& Name() const
                {
                    return _Name;
                }

                virtual ~Text()
                {
                }

                Text(Parallel *parent, string src_file, string src_fragmentID, ManifestItemPtr srcManifestItem):Media(parent, src_file, src_fragmentID, srcManifestItem)
                {
                    parent->_text = this;
                }
            };

            class Parallel : public TimeContainer
            {
            private:
                Parallel() _DELETED_;

                Parallel(const Parallel&) _DELETED_;

                Parallel(Parallel&&) _DELETED_;

                static const string _Name;
            public:
                const string& Name() const
                {
                    return _Name;
                }

                virtual ~Parallel()
                {
                    if (_audio != nullptr)
                    {
                        delete _audio;
                    }
                    if (_text != nullptr)
                    {
                        delete _text;
                    }
                }

                Parallel(Sequence *parent, string textref_file, string textref_fragmentID, ManifestItemPtr textrefManifestItem, string type):TimeContainer(parent, textref_file, textref_fragmentID, textrefManifestItem, type)
                {
                }

                Audio *_audio;
                Text *_text;
            };

        private :
            SMILData() _DELETED_;

            SMILData(const SMILData&) _DELETED_;

            SMILData(SMILData&&) _DELETED_;

            uint32_t _duration; //whole milliseconds (resolution = 1ms)

            ManifestItemPtr _manifestItem;

            Sequence *_root;

        public:
            EPUB3_EXPORT

            SMILData(const shared_ptr<MediaOverlaysSmilModel> smilModel, ManifestItemPtr manifestItem, uint32_t duration);

            virtual ~SMILData()
            {
                if (_root != nullptr)
                {
                    delete _root;
                }
            }

            EPUB3_EXPORT

            const uint32_t DurationMilliseconds() const
            {
                return _duration;
            }

            EPUB3_EXPORT

            const ManifestItemPtr ManifestItem() const
            {
                return _manifestItem;
            }

            EPUB3_EXPORT

            const Sequence *Body() const
            {
                return _root;
            }
        };

        EPUB3_END_NAMESPACE

#endif /* defined(ePub3_mo_smil_data_h) */
