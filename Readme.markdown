# Readium SDK

#### Copyright (c) 2012–2013 The Readium Foundation.

The Readium SDK is an ePub reader SDK of similar scope and capability to
Adobe's Reader Mobile SDK (RMSDK).  As such, it is designed to implement an ePub3-
compliant Reading System, although its initial form will concentrate solely on the
core [ePub3 specification][ePub3] and [fixed-layout metadata][fxl], expanding later
to cover associated standards such as [Page Templates][pt].

[ePub3]: http://idpf.org/epub/30
[fxl]: http://idpf.org/epub/fxl/
[pt]: http://idpf.org/epub/pgt/csspgt-20120808.html

The project's aim is to develop a productized, high-performance, cross-platform
rendering engine for EPUB 3 content, optimized for use in native applications
(mobile/tablet and secondarily desktop systems).  Simplistic test applications for
Android, iOS, OS X and Windows are part of the SDK, along with unit tests.  The SDK
is designed to be extensible in various areas, including in support for pluggable
encryption and digital signature modules which might be used to implement a form
of DRM.

The Readium SDK is at present dual-licensed under both a copyleft license (version
3 of the *GNU General Public License*) and a commercial-use-enabling *Contributor's
License*.  All copyright in the official form of the SDK (that found at
<http://github.com/readium/readium-sdk/>) is assigned to the Readium Foundation. 
A copy of the GPLv3 can be found in the [LICENSE](LICENSE) file.  A Contributor's License is
available from the Readium Foundation directly; see the [CONTRIBUTING](CONTRIBUTING) file for
information on how to apply to become an official contributor or sponsor, and to
apply for a commercial license.

### Architecture

The SDK consists of three major components, corresponding roughly to *Model*, *View*,
and *Controller*.  The majority of the ePub3 data handling takes place inside the
*Model* component, which provides parsing and generation of ePub-related data
structures.  The *View* component is a browser engine; this engine may be a customized
form of WebKit developed as part of this project, but any CSS3-compliant browser
engine may be used.  The *Controller* component is a small interface between the data
and renderer components; this part is ultimately what Reading System vendors will
extend to create their own reading system.

The project is written in cross-platform C++ and JavaScript code, and is designed
to be combined with platform-specific glue code and a modern browser rendering
engine to instantiate an ePub3-compliant rendering engine within a platform-native
Reading System application.  The SDK constitutes a capable, high-performance ePub
engine, not an overall application (other than per platform test applications).

The general architecture is that the browser engine (headlessly) handles content
rendering with some features like pagination being implemented in Readium SDK
JavaScript code that has been “injected” into the browser context. The C++ code
handles tasks/features that can’t be efficiently performed in JS, such as
incrementally fetching (and, as necessary, decrypting/de-obfuscating) resources
from an .epub ZIP package.  Certain UX affordances are supported within the browser
context (e.g. text highlighting) but application-level UX affordances would
generally be done in the reading system application via platform-specific code
(e.g. Java on Android, Obj-C on iOS). Integration glue is provided in the Readium
SDK to facilitate this, and usage illustrated by the test applications (which
however are not product-level reading systems).   Handling of XML Encryption and
Digital Signature files is provided to simplify implementation of DRM or content
protection/validation that is compatible with the ePub3 specification.

The Readium SDK is designed to potentially work with multiple browser engines but
the #1 priority for initial development is WebKit, including platform-bundled
instantiations (e.g. via UIWebView on iOS). #2 priority browser engine will be
Trident/IE10 (needed for apps to be able to support screen readers on Windows).