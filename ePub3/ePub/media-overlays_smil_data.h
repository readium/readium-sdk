//
//  media-overlays_smil_data.h
//  ePub3
//
//  Created by Daniel Weck on 2013-09-18.
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

#ifndef ePub3_mo_smil_data_h
#define ePub3_mo_smil_data_h

#include <ePub3/epub3.h>
#include <ePub3/utilities/owned_by.h>
#include <ePub3/manifest.h>
#include <ePub3/spine.h>

EPUB3_BEGIN_NAMESPACE

        class MediaOverlaysSmilModel;

        class SMILData;
        typedef std::shared_ptr<SMILData> SMILDataPtr;

        class SMILData : public OwnedBy<MediaOverlaysSmilModel> //public std::enable_shared_from_this<SMILData>
#if EPUB_PLATFORM(WINRT)
			, public NativeBridge
#endif
        {
            friend class MediaOverlaysSmilModel; // _root

        public:

            class Sequence;

            class Parallel;

            class TimeContainer;

            class Audio;

            class Text;

            class TimeNode : public OwnedBy<SMILData>, public std::enable_shared_from_this<TimeNode> 
#if EPUB_PLATFORM(WINRT)
				, public NativeBridge
#endif
            {
            private:
                TimeNode() _DELETED_;

                TimeNode(const TimeNode &) _DELETED_;

                TimeNode(TimeNode &&) _DELETED_;

            protected:
                shared_ptr<TimeContainer> _parent;

            public:
                virtual const string & Name() const;

                virtual ~TimeNode();

                EPUB3_EXPORT

                TimeNode(shared_ptr<TimeContainer> parent, const std::shared_ptr<SMILData> & smilData) : _parent(parent), OwnedBy(smilData)
#if EPUB_PLATFORM(WINRT)
                    , NativeBridge()
#endif
                {
                }

                EPUB3_EXPORT

                shared_ptr<const TimeContainer> Parent() const
                {
                    return _parent;
                }
            };

            class TimeContainer : public TimeNode
            {
            private:
                TimeContainer() _DELETED_;

                TimeContainer(const TimeContainer &) _DELETED_;

                TimeContainer(TimeContainer &&) _DELETED_;

            protected:
                string _type; // space-separated

            public:
                virtual ~TimeContainer();

                EPUB3_EXPORT

                TimeContainer(shared_ptr<Sequence> parent, string type, const SMILDataPtr smilData):TimeNode(parent, smilData), _type(type)
                {
					// can't access shared_from_this() yet-- might work with MS' implementation, but not in libc++
					/*
                    if (parent != nullptr)
                    {
                        parent->_children.push_back(this);
                    }
					*/
                }

                EPUB3_EXPORT

                shared_ptr<const Sequence> ParentSequence() const
                {
                    return std::dynamic_pointer_cast<const Sequence>(_parent);
                }

                EPUB3_EXPORT

                const string Type() const
                {
                    return _type;
                }

                EPUB3_EXPORT

                virtual const bool IsParallel() const;

                EPUB3_EXPORT

                virtual const bool IsSequence() const;
            };

            class Sequence : public TimeContainer
            {
                friend class MediaOverlaysSmilModel; // _children

                friend class SMILData; // ParallelAt, NthParallel, ClipOffset

            private:
                Sequence() _DELETED_;

                Sequence(const Sequence &) _DELETED_;

                Sequence(Sequence &&) _DELETED_;

                static const string _Name;

            protected:
                string _textref_file;
                string _textref_fragmentID;

                const std::shared_ptr<ManifestItem> _textrefManifestItem;

                shared_vector<const TimeContainer> _children;

                const bool ClipOffset(uint32_t & offset, shared_ptr<const Parallel> par) const
                {
                    std::shared_ptr<SMILData> smilData = Owner(); // internally: std::weak_ptr<SMILData>.lock()
                    if (smilData == nullptr)
                    {
                        return false;
                    }
                    
                    for (shared_vector<const TimeContainer>::size_type i = 0; i < _children.size(); i++)
                    {
                        auto container = _children[i];
                        if (container->IsParallel())
                        {
                            auto para = std::dynamic_pointer_cast<const Parallel>(container);
                            if (para == par)
                            {
                                return true;
                            }

                            if (para->Audio() == nullptr)
                            {
                                continue;
                            }

                            if (para->Text() != nullptr && para->Text()->SrcManifestItem() != nullptr && para->Text()->SrcManifestItem() != smilData->XhtmlSpineItem()->ManifestItem())
                            {
                                continue;
                            }

                            uint32_t clipDur = para->Audio()->ClipDurationMilliseconds();
                            offset += clipDur;
                        }
                        else if (container->IsSequence())
                        {
                            auto sequence = std::dynamic_pointer_cast<const Sequence>(container);

                            bool found = sequence->ClipOffset(offset, par);
                            if (found)
                            {
                                return true;
                            }
                        }
                    }

                    return false;
                }

                shared_ptr<const Parallel> ParallelAt(uint32_t timeMilliseconds) const
                {
                    std::shared_ptr<SMILData> smilData = Owner(); // internally: std::weak_ptr<SMILData>.lock()
                    if (smilData == nullptr)
                    {
                        return nullptr;
                    }
                    
                    uint32_t offset = 0;

                    for (shared_vector<const TimeContainer>::size_type i = 0; i < _children.size(); i++)
                    {
                        uint32_t timeAdjusted = timeMilliseconds - offset;

                        auto container = _children[i];
                        if (container->IsParallel())
                        {
                            auto para = std::dynamic_pointer_cast<const Parallel>(container);

                            if (para->Audio() == nullptr)
                            {
                                continue;
                            }

                            if (para->Text() != nullptr && para->Text()->SrcManifestItem() != nullptr && para->Text()->SrcManifestItem() != smilData->XhtmlSpineItem()->ManifestItem())
                            {
                                continue;
                            }

                            uint32_t clipDur = para->Audio()->ClipDurationMilliseconds();

                            if (clipDur > 0 && timeAdjusted <= clipDur)
                            {
                                return para;
                            }

                            offset += clipDur;
                        }
                        else if (container->IsSequence())
                        {
                            auto sequence = std::dynamic_pointer_cast<const Sequence>(container);

                            auto para = sequence->ParallelAt(timeAdjusted);
                            if (para != nullptr)
                            {
                                return para;
                            }

                            offset += sequence->DurationMilliseconds();
                        }
                    }

                    return nullptr;
                }

                shared_ptr<const Parallel> NthParallel(uint32_t index, uint32_t & count) const
                {
                    for (shared_vector<const TimeContainer>::size_type i = 0; i < _children.size(); i++)
                    {
                        auto container = _children[i];
                        if (container->IsParallel())
                        {
                            count++;

                            if (count == index)
                            {
                                auto para = std::dynamic_pointer_cast<const Parallel>(container);
                                return para;
                            }
                        }
                        else if (container->IsSequence())
                        {
                            auto sequence = std::dynamic_pointer_cast<const Sequence>(container);

                            auto para = sequence->NthParallel(index, count);
                            if (para != nullptr)
                            {
                                return para;
                            }
                        }
                    }

                    return nullptr;
                }

            public:
                EPUB3_EXPORT

                const string & Name() const
                {
                    return _Name;
                }

                virtual ~Sequence();

                EPUB3_EXPORT

					Sequence(shared_ptr<Sequence> parent, string textref_file, string textref_fragmentID, const ManifestItemPtr textrefManifestItem, string type, const SMILDataPtr smilData) :TimeContainer(parent, type, smilData), _textref_file(textref_file), _textref_fragmentID(textref_fragmentID), _textrefManifestItem(textrefManifestItem), _children()
                {
                }

                EPUB3_EXPORT

                const string TextRefFile() const
                {
                    return _textref_file;
                }

                EPUB3_EXPORT

                const string TextRefFragmentId() const
                {
                    return _textref_fragmentID;
                }

                EPUB3_EXPORT

                const std::shared_ptr<ManifestItem> TextRefManifestItem() const
                {
                    return _textrefManifestItem;
                }

                EPUB3_EXPORT

                const shared_vector<const TimeContainer>::size_type GetChildrenCount() const
                {
                    return _children.size();
                }

                EPUB3_EXPORT

				shared_ptr<const TimeContainer> GetChild(shared_vector<const TimeContainer>::size_type i) const
                {
                    if (i >= _children.size())
                    {
                        return nullptr;
                    }

                    return _children[i];
                }

                EPUB3_EXPORT

                const bool IsParallel() const
                {
                    return false;
                }

                EPUB3_EXPORT

                const bool IsSequence() const
                {
                    return true;
                }

                EPUB3_EXPORT

                const uint32_t DurationMilliseconds() const
                {
                    std::shared_ptr<SMILData> smilData = Owner(); // internally: std::weak_ptr<SMILData>.lock()
                    if (smilData == nullptr)
                    {
                        return 0;
                    }
                    
                    uint32_t total = 0;

                    for (shared_vector<const TimeContainer>::size_type i = 0; i < _children.size(); i++)
                    {
                        auto container = _children[i];
                        if (container->IsParallel())
                        {
                            auto para = std::dynamic_pointer_cast<const Parallel>(container);

                            if (para->Audio() == nullptr)
                            {
                                continue;
                            }

                            if (para->Text() != nullptr && para->Text()->SrcManifestItem() != nullptr && para->Text()->SrcManifestItem() != smilData->XhtmlSpineItem()->ManifestItem())
                            {
                                continue;
                            }

                            uint32_t clipDur = para->Audio()->ClipDurationMilliseconds();
                            total += clipDur;
                        }
                        else if (container->IsSequence())
                        {
                            auto sequence = std::dynamic_pointer_cast<const Sequence>(container);

                            total += sequence->DurationMilliseconds();
                        }
                    }

                    return total;
                }
            };

            class Media : public TimeNode
            {
            private:
                Media() _DELETED_;

                Media(const Media &) _DELETED_;

                Media(Media &&) _DELETED_;

            protected:
                string _src_file;
                string _src_fragmentID;
                const std::shared_ptr<ManifestItem> _srcManifestItem;

            public:
                virtual ~Media();

                EPUB3_EXPORT

                Media(shared_ptr<Parallel> parent, string src_file, string src_fragmentID, const ManifestItemPtr srcManifestItem, const SMILDataPtr smilData):TimeNode(parent, smilData), _src_file(src_file), _src_fragmentID(src_fragmentID), _srcManifestItem(srcManifestItem)
                {
                }

                EPUB3_EXPORT

                shared_ptr<const Parallel> ParentParallel() const
                {
                    return std::dynamic_pointer_cast<const Parallel>(_parent);
                }

                EPUB3_EXPORT

                const string SrcFile() const
                {
                    return _src_file;
                }

                EPUB3_EXPORT

                const string SrcFragmentId() const
                {
                    return _src_fragmentID;
                }

                EPUB3_EXPORT

                const std::shared_ptr<ManifestItem> SrcManifestItem() const
                {
                    return _srcManifestItem;
                }

                EPUB3_EXPORT

                virtual const bool IsAudio() const;

                EPUB3_EXPORT

                virtual const bool IsText() const;
            };

            class Audio : public Media
            {
            private:
                Audio() _DELETED_;

                Audio(const Audio &) _DELETED_;

                Audio(Audio &&) _DELETED_;

                static const string _Name;

            protected:
                uint32_t _clipBeginMilliseconds;
                uint32_t _clipEndMilliseconds;

            public:
                EPUB3_EXPORT

                const string & Name() const
                {
                    return _Name;
                }

                virtual ~Audio();

                EPUB3_EXPORT

                Audio(shared_ptr<Parallel> parent, string src, const ManifestItemPtr srcManifestItem, uint32_t clipBeginMilliseconds, uint32_t clipEndMilliseconds, const SMILDataPtr smilData):Media(parent, src, "", srcManifestItem, smilData), _clipBeginMilliseconds(clipBeginMilliseconds), _clipEndMilliseconds(clipEndMilliseconds)
                {
					// can't use shared_from_this() yet, is hasn't been set up
                    //parent->_audio = this;
                }

                EPUB3_EXPORT

                const uint32_t ClipBeginMilliseconds() const
                {
                    return _clipBeginMilliseconds;
                }

                EPUB3_EXPORT

                const uint32_t ClipEndMilliseconds() const
                {
                    return _clipEndMilliseconds;
                }

                EPUB3_EXPORT

                const uint32_t ClipDurationMilliseconds() const
                {
                    if (_clipEndMilliseconds <= 0 || _clipEndMilliseconds <= _clipBeginMilliseconds)
                    {
                        return 0;
                    }

                    return _clipEndMilliseconds - _clipBeginMilliseconds;
                }

                EPUB3_EXPORT

                const bool IsAudio() const
                {
                    return true;
                }

                EPUB3_EXPORT

                const bool IsText() const
                {
                    return false;
                }
            };

            class Text : public Media
            {
            private:
                Text() _DELETED_;

                Text(const Text &) _DELETED_;

                Text(Text &&) _DELETED_;

                static const string _Name;

            public:
                EPUB3_EXPORT

                const string & Name() const
                {
                    return _Name;
                }

                virtual ~Text();

                EPUB3_EXPORT

                Text(shared_ptr<Parallel> parent, string src_file, string src_fragmentID, const ManifestItemPtr srcManifestItem, const SMILDataPtr smilData):Media(parent, src_file, src_fragmentID, srcManifestItem, smilData)
                {
					// can't use shared_from_this() yet, it hasn't been assigned
                    //parent->_text = this;
                }

                EPUB3_EXPORT

                const bool IsAudio() const
                {
                    return false;
                }

                EPUB3_EXPORT

                const bool IsText() const
                {
                    return true;
                }
            };

            class Parallel : public TimeContainer
            {
				friend class MediaOverlaysSmilModel;	// _audio, _text

            private:
                Parallel() _DELETED_;

                Parallel(const Parallel &) _DELETED_;

                Parallel(Parallel &&) _DELETED_;

                static const string _Name;

            protected:
                shared_ptr<Audio> _audio;
                shared_ptr<Text> _text;

            public:

                EPUB3_EXPORT

                const string & Name() const
                {
                    return _Name;
                }

                virtual ~Parallel();

                EPUB3_EXPORT

                Parallel(shared_ptr<Sequence> parent, string type, const SMILDataPtr smilData):TimeContainer(parent, type, smilData), _audio(nullptr), _text(nullptr)
                {
                }

                EPUB3_EXPORT

                shared_ptr<const Audio> Audio() const
                {
                    return _audio;
                }

                EPUB3_EXPORT

                shared_ptr<const Text> Text() const
                {
                    return _text;
                }

                EPUB3_EXPORT

                const bool IsParallel() const
                {
                    return true;
                }

                EPUB3_EXPORT

                const bool IsSequence() const
                {
                    return false;
                }
            };

        private :
            SMILData() _DELETED_;

            SMILData(const SMILData &) _DELETED_;

            SMILData(SMILData &&) _DELETED_;

        protected:
            uint32_t _duration; //whole milliseconds (resolution = 1ms)

            const std::shared_ptr<ManifestItem> _manifestItem;

            const std::shared_ptr<SpineItem> _spineItem;

            shared_ptr<Sequence> _root;

            shared_ptr<const Parallel> ParallelAt(uint32_t timeMilliseconds) const
            {
                if (_root == nullptr)
                {
                    return nullptr;
                }

                return _root->ParallelAt(timeMilliseconds);
            }

            shared_ptr<const Parallel> NthParallel(uint32_t index) const
            {
                if (_root == nullptr)
                {
                    return nullptr;
                }

                uint32_t count = -1;
                return _root->NthParallel(index, count);
            }

            const uint32_t ClipOffset(shared_ptr<const Parallel> par) const
            {
                if (_root == nullptr)
                {
                    return 0;
                }

                uint32_t offset = 0;
                if (_root->ClipOffset(offset, par))
                {
                    return offset;
                }

                return 0;
            }

        public:
            EPUB3_EXPORT

            SMILData(const shared_ptr<MediaOverlaysSmilModel> & smilModel, const ManifestItemPtr manifestItem, const SpineItemPtr spineItem, uint32_t duration): OwnedBy(smilModel),
#if EPUB_PLATFORM(WINRT)
                NativeBridge(),
#endif
                _manifestItem(manifestItem), _spineItem(spineItem), _duration(duration), _root(nullptr)
            {
                //printf("SMILData(%s)\n", manifestItem->Href().c_str());
            }

            virtual ~SMILData();

            EPUB3_EXPORT

            const SpineItemPtr XhtmlSpineItem() const
            {
                return _spineItem;
            }

            EPUB3_EXPORT

            const ManifestItemPtr SmilManifestItem() const
            {
                return _manifestItem;
            }

            EPUB3_EXPORT

            shared_ptr<const Sequence> Body() const
            {
                if (_root == nullptr)
                {
                    return nullptr;
                }
                
                //return _root.get();
                return _root;
            }

            EPUB3_EXPORT

            const uint32_t DurationMilliseconds_Metadata() const
            {
                return _duration;
            }

            EPUB3_EXPORT

            const uint32_t DurationMilliseconds_Calculated() const
            {
                if (_root == nullptr)
                {
                    return 0;
                }

                return _root->DurationMilliseconds();
            }
        };

        EPUB3_END_NAMESPACE

#endif /* defined(ePub3_mo_smil_data_h) */
