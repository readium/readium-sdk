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
#include <ePub3/media-overlays_smil_utils.h>
#include "xpath_wrangler.h"

EPUB3_BEGIN_NAMESPACE

// upside: nice syntax for checking
// downside: operator[] always creates a new item
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
static std::map<string, bool> AllowedRootNodeNames = {
    { "smil", true }
};
#else
        typedef std::pair<string, bool> __name_pair_t;
        static __name_pair_t __name_pairs[1] = {
                __name_pair_t("smil", true)
        };
        static std::map<string, bool> AllowedRootNodeNames(&__name_pairs[0], &__name_pairs[1]);
#endif

        MediaOverlaysSmilModel::~MediaOverlaysSmilModel()
        {
        }

        MediaOverlaysSmilModel::MediaOverlaysSmilModel(const PackagePtr& package) //shared_ptr<Package>
        : OwnedBy(package)
        {
            const string& narrator = package->MediaOverlays_Narrator();
            printf("Media Overlays NARRATOR: %s\n", narrator.c_str());

            const string& activeClass = package->MediaOverlays_ActiveClass();
            printf("Media Overlays ACTIVE CLASS: %s\n", activeClass.c_str());

            const string& playbackActiveClass = package->MediaOverlays_PlaybackActiveClass();
            printf("Media Overlays PLAYBACK ACTIVE CLASS: %s\n", playbackActiveClass.c_str());

            const string& durationStr = package->MediaOverlays_Duration();
            printf("Media Overlays TOTAL DURATION (string): %s\n", durationStr.c_str());

            uint32_t totalDurationWholeMilliseconds = -1;

            if (!durationStr.empty())
            {
                totalDurationWholeMilliseconds = ePub3::SmilClockValuesParser::ToWholeMilliseconds(durationStr);
                printf("Media Overlays TOTAL DURATION (milliseconds): %ld\n", (long) totalDurationWholeMilliseconds);
            }

            uint32_t accumulatedDurationMilliseconds = 0.0;

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

                const string& itemDurationStr = package->MediaOverlays_Duration(item);
                printf("Media Overlays SMIL DURATION (string): %s\n", itemDurationStr.c_str());

                if (!itemDurationStr.empty())
                {
                    uint32_t durationWholeMilliseconds = ePub3::SmilClockValuesParser::ToWholeMilliseconds(itemDurationStr);
                    printf("Media Overlays SMIL DURATION (milliseconds): %ld\n", (long) durationWholeMilliseconds);

                    accumulatedDurationMilliseconds += durationWholeMilliseconds;
                }
            }

            if (accumulatedDurationMilliseconds != totalDurationWholeMilliseconds)
            {
                printf("Media Overlays SMIL DURATION MISMATCH (milliseconds): TOTAL %ld != ACCUMULATED %ld\n", (long) totalDurationWholeMilliseconds, (long) accumulatedDurationMilliseconds);
            }
            else
            {
                printf("Media Overlays SMIL DURATION check okay.\n");
            }
        }

        bool MediaOverlaysSmilModel::ParseXML(xmlNodePtr node)
        {
            if (node == nullptr)
                return false;

            string name(node->name);
            if (AllowedRootNodeNames.find(name) == AllowedRootNodeNames.end())
                return false;

            string version = _getProp(node, "version", SMILNamespaceURI);
            if (!version.empty())
            {
                printf("SMIL VERSION: %s\n", version.c_str()); //"3.0"
            }
            else
            {
                printf("NO SMIL VERSION?\n");
            }

#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    XPathWrangler xpath(node->doc, {{"epub", ePub3NamespaceURI}, {"smil", SMILNamespaceURI}});
#else
            XPathWrangler::NamespaceList __ns;
            __ns["epub"] = ePub3NamespaceURI;
            __ns["smil"] = SMILNamespaceURI;
            XPathWrangler xpath(node->doc, __ns);
#endif
            xpath.NameDefaultNamespace("smil");

            xmlNodeSetPtr nodes = xpath.Nodes("./smil:body", node);
            if (nodes == nullptr)
                return false;
            if (nodes->nodeNr != 1)
            {
                xmlXPathFreeNodeSet(nodes);
                return false;
            }

            xmlNodePtr linkedChildrenList = nodes->nodeTab[0]->children;
            if (linkedChildrenList == nullptr)
            {
                return false;
            }

            for (; linkedChildrenList != nullptr; linkedChildrenList = linkedChildrenList->next)
            {
                if (linkedChildrenList->type != XML_ELEMENT_NODE)
                    continue;

                std::string elementName(reinterpret_cast<const char *>(linkedChildrenList->name));

                if (elementName == "p")
                {
                    const string& str = reinterpret_cast<const char *>(xmlNodeGetContent(linkedChildrenList));
                }
            }

            xmlXPathFreeNodeSet(nodes);

            return true;
        }

        EPUB3_END_NAMESPACE
