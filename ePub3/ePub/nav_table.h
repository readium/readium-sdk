//
//  nav_table.h
//  ePub3
//
//  Created by Jim Dovey on 2012-12-11.
//  Copyright (c) 2012 Kobo Inc. All rights reserved.
//

#ifndef __ePub3__nav_table__
#define __ePub3__nav_table__

#include "epub3.h"
#include "nav_point.h"
#include <libxml/xpath.h> // for xmlNodeSetPtr

EPUB3_BEGIN_NAMESPACE

class NavigationTable : public NavigationElement
{
public:
    NavigationTable() = delete;
    NavigationTable(xmlNodePtr node);   // requires a HTML <nav> node
    NavigationTable(const std::string& type) : NavigationElement(), _type(type), _title() {}
    NavigationTable(std::string&& type) : NavigationElement(), _type(type), _title() {}
    NavigationTable(const NavigationTable&) = delete;
    NavigationTable(NavigationTable&& o) : NavigationElement(o), _type(std::move(o._type)), _title(std::move(o._title)) {}
    virtual ~NavigationTable() {}
    
    const std::string& Type() const { return _type; }
    void SetType(const std::string& str) { _type = str; }
    void SetType(std::string&& str) { _type = str; }
    
    virtual const std::string& Title() const { return _title; }
    virtual void SetTitle(const std::string& str) { _title = str; }
    virtual void SetTitle(std::string&& str) { _title = str; }
    
protected:
    std::string     _type;
    std::string     _title;     // optional
    
    bool Parse(xmlNodePtr node);
    static void BuildNavPoints(xmlNodeSetPtr nodes, NavigationList* navList);
};

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3__nav_table__) */
