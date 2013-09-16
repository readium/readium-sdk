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
#include <libxml/xpath.h> // for xmlNodeSetPtr

EPUB3_BEGIN_NAMESPACE

class Package;

class MediaOverlaysSmilModel;

typedef shared_ptr<MediaOverlaysSmilModel> MediaOverlaysSmilModelPtr;

/**
Parser that reads SMIL XML files into an in-memory data model

See:
http://www.idpf.org/epub/30/spec/epub30-mediaoverlays.html

@ingroup epub-model
*/
class MediaOverlaysSmilModel : public std::enable_shared_from_this<MediaOverlaysSmilModel>, public OwnedBy<Package>
{
private:
    MediaOverlaysSmilModel() _DELETED_;
    MediaOverlaysSmilModel(const MediaOverlaysSmilModel&) _DELETED_;
    MediaOverlaysSmilModel(MediaOverlaysSmilModel&&) _DELETED_;

public:
    EPUB3_EXPORT MediaOverlaysSmilModel(const shared_ptr<Package>& owner); //PackagePtr
    virtual ~MediaOverlaysSmilModel();

protected:
    bool ParseXML(xmlNodePtr node);
};

EPUB3_END_NAMESPACE

#endif /* defined(ePub3_mo_smil_model_h) */
