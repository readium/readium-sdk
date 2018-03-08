//
//  error_handler.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-05-14.
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

#include "error_handler.h"
#include <map>
#include <iostream>
#if EPUB_HAVE(STD_STRINGSTREAM)
# include <sstream>
#else
# include <strstream>
#endif

#if EPUB_COMPILER_SUPPORTS(CXX_UNICODE_LITERALS)
# define kSectionMarker u8"??"
#else
# define kSectionMarker "section"
#endif

EPUB3_BEGIN_NAMESPACE

EPUB3_EXPORT std::array<std::string, static_cast<std::size_t>(EPUBSpec::NUM_SPECS)> EPUBSpecNames = {
    "Open Container Format 3.0",
    "Open Publications Format 3.0",
    "Content Documents 3.0",
    "Media Overlays 3.0",
    "Canonical Fragment Identifiers 3.0"
};
EPUB3_EXPORT std::array<std::string, static_cast<std::size_t>(EPUBSpec::NUM_SPECS)> EPUBSpecURLs = {
    "http://idpf.org/epub/30/spec/epub30-ocf.html",
    "http://idpf.org/epub/30/spec/epub30-publications.html",
    "http://idpf.org/epub/30/spec/epub30-contentdocs.html",
    "http://idpf.org/epub/30/spec/epub30-mediaoverlays.html",
    "http://idpf.org/epub/linking/cfi/epub-cfi.html"
};

static std::array<std::string, 4> gSeverityStrings = {
    "Minor", "Medium", "Major", "Critical"
};

#include "error_lookup_table.cpp"

class _LIBCPP_HIDDEN __epub_spec_category : public std::error_category
{
public:
    FORCE_INLINE
    __epub_spec_category() {}
    virtual const char* name() const _NOEXCEPT;
    virtual std::string message(int __ev) const;
};

_LIBCPP_HIDDEN const __epub_spec_category& __get_epub_spec_category() _NOEXCEPT
{
    static const __epub_spec_category __epub_spec_category_;
    return __epub_spec_category_;
}

const char* __epub_spec_category::name() const _NOEXCEPT
{
    return "EPUB Specification";
}
std::string __epub_spec_category::message(int __ev) const
{
    EPUBError err = static_cast<EPUBError>(__ev);
    auto pos = gErrorLookupTable.find(err);
    if ( pos == gErrorLookupTable.end() )
        return std::string("Unspecified EPUB specification error");
    return pos->second.Message();
}

EPUB3_EXPORT
const std::string& SeverityString(ViolationSeverity __s)
{
    return gSeverityStrings[static_cast<std::size_t>(__s)];
}

EPUB3_EXPORT
const std::error_code ErrorCodeForEPUBError(EPUBError ev) _NOEXCEPT
{
    return std::error_code(static_cast<int>(ev), epub_spec_category());
}

EPUB3_EXPORT
const std::string DetailedErrorMessage(EPUBError err)
{
    auto pos = gErrorLookupTable.find(err);
    if ( pos == gErrorLookupTable.end() )
        return std::string("<unknown epub spec error>");
    const ErrorInfo& info = pos->second;
    std::stringstream ss;
    ss << SeverityString(info.Severity()) << " violation of " << EPUBSpecNames[static_cast<std::size_t>(info.Spec())] << " (" << EPUBSpecURLs[static_cast<std::size_t>(info.Spec())] << ") " << kSectionMarker << " " << info.Section() << ":" << std::endl;
    ss << "  " << info.Message();
    return ss.str();
}

EPUB3_EXPORT
const std::error_category& epub_spec_category() _NOEXCEPT
{
    return __get_epub_spec_category();
}

EPUB3_EXPORT
EPUBSpec SpecFromEPUBError(EPUBError err)
{
	auto pos = gErrorLookupTable.find(err);
	if (pos == gErrorLookupTable.end())
		return EPUBSpec::UnknownSpec;
	return pos->second.Spec();
}

std::string epub_spec_error::__init(const std::error_code& code, std::string what)
{
    if ( code )
    {
        if ( !what.empty() )
            what += ": ";
        what += code.message();
    }
    return std::move(what);
}
epub_spec_error::epub_spec_error(EPUBError __ev, const std::string& __what_arg)
  : std::runtime_error(__init(std::error_code(static_cast<int>(__ev), epub_spec_category()), __what_arg)),
    __ec(static_cast<int>(__ev), epub_spec_category())
{
}
epub_spec_error::epub_spec_error(EPUBError __ev, const char* __what_arg)
  : std::runtime_error(__init(std::error_code(static_cast<int>(__ev), epub_spec_category()), __what_arg)),
    __ec(static_cast<int>(__ev), epub_spec_category())
{
}
epub_spec_error::epub_spec_error(EPUBError __ev)
  : std::runtime_error(__init(std::error_code(static_cast<int>(__ev), epub_spec_category()), "")),
    __ec(static_cast<int>(__ev), epub_spec_category())
{
}
epub_spec_error::~epub_spec_error() _NOEXCEPT
{
}
ViolationSeverity epub_spec_error::Severity() const
{
    EPUBError err = static_cast<EPUBError>(__ec.value());
    auto pos = gErrorLookupTable.find(err);
    if ( pos == gErrorLookupTable.end() )
        return ViolationSeverity::Minor;
    return pos->second.Severity();
}
EPUBSpec epub_spec_error::Specification() const
{
	return SpecFromEPUBError(SpecErrorCode());
}

#if 0
#pragma mark -
#endif

EPUB3_EXPORT
bool DefaultErrorHandler(const error_details& err)
{
    if (err.is_spec_error())
    {
        switch ( err.severity() )
        {
            case ViolationSeverity::Critical:
            case ViolationSeverity::Major:
                return false;
                
            default:
                return true;
        }
    }
    
    return false;
}

static ErrorHandlerFn   gErrorHandler = ePub3::DefaultErrorHandler;

EPUB3_EXPORT
ErrorHandlerFn ErrorHandler()
{
    return gErrorHandler;
}

EPUB3_EXPORT
void SetErrorHandler(ErrorHandlerFn fn)
{
    gErrorHandler = fn;
}

EPUB3_END_NAMESPACE
