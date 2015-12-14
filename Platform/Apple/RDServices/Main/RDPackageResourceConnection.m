//
//  RDPackageResourceConnection.m
//  RDServices
//
//  Created by Shane Meyer on 11/23/13.
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

#import "RDPackageResourceConnection.h"
#import "RDPackage.h"
#import "RDPackageResource.h"
#import "RDPackageResourceDataResponse.h"
#import "RDPackageResourceResponse.h"
#import "RDPackageResourceServer.h"


static NSMutableArray *m_coreResourcePaths = nil;
static NSMutableArray *m_coreResourcePrefixes = nil;
static long m_epubReadingSystem_Counter = 0;
static __weak RDPackageResourceServer *m_packageResourceServer = nil;
static BOOL m_provideCoreResources = NO;
static BOOL m_provideCoreResourcesIsKnown = NO;


@implementation RDPackageResourceConnection

//
// Converts the given HTML data to a string.  The character set and encoding are assumed to be
// UTF-8, UTF-16BE, or UTF-16LE.
//
- (NSString *)htmlFromData:(NSData *)data {
	if (data == nil || data.length == 0) {
		return nil;
	}
	
	NSString *html = nil;
	UInt8 *bytes = (UInt8 *)data.bytes;
	
	if (data.length >= 3) {
		if (bytes[0] == 0xFE && bytes[1] == 0xFF) {
			html = [[NSString alloc] initWithData:data
										 encoding:NSUTF16BigEndianStringEncoding];
		}
		else if (bytes[0] == 0xFF && bytes[1] == 0xFE) {
			html = [[NSString alloc] initWithData:data
										 encoding:NSUTF16LittleEndianStringEncoding];
		}
		else if (bytes[0] == 0xEF && bytes[1] == 0xBB && bytes[2] == 0xBF) {
			html = [[NSString alloc] initWithData:data
										 encoding:NSUTF8StringEncoding];
		}
		else if (bytes[0] == 0x00) {
			// There's a very high liklihood of this being UTF-16BE, just without the BOM.
			html = [[NSString alloc] initWithData:data
										 encoding:NSUTF16BigEndianStringEncoding];
		}
		else if (bytes[1] == 0x00) {
			// There's a very high liklihood of this being UTF-16LE, just without the BOM.
			html = [[NSString alloc] initWithData:data
										 encoding:NSUTF16LittleEndianStringEncoding];
		}
		else {
			html = [[NSString alloc] initWithData:data
										 encoding:NSUTF8StringEncoding];
			
			if (html == nil) {
				html = [[NSString alloc] initWithData:data
											 encoding:NSUTF16BigEndianStringEncoding];
				
				if (html == nil) {
					html = [[NSString alloc] initWithData:data
												 encoding:NSUTF16LittleEndianStringEncoding];
				}
			}
		}
	}
	
	return html;
}


- (NSObject <HTTPResponse> *)httpResponseForMethod:(NSString *)method URI:(NSString *)path {
	if (m_packageResourceServer == nil ||
		method == nil ||
		(![method isEqualToString:@"GET"] && ![method isEqualToString:@"HEAD"]) ||
		path == nil ||
		path.length == 0)
	{
		return nil;
	}
	
	if ([path hasPrefix:@"/"]) {
		path = [path substringFromIndex:1];
	}

	if (!m_provideCoreResourcesIsKnown) {
		m_provideCoreResourcesIsKnown = YES;

		// This is the first request since the web server started. The request should be for the
		// main reader HTML file. Check to see if its query string lists any core resources that
		// it would like for us to handle. UIWebView is able to serve core resources using file
		// URLs that point to the app bundle, but WKWebView is restricted from doing this, so the
		// web server must provide everything in that case.

		NSRange range = [path rangeOfString:@"?"];

		if (range.location != NSNotFound) {
			NSString *query = [path substringFromIndex:range.location + 1];

			// Add the main reader HTML file to the list of core resources.

			NSString *filename = [path substringToIndex:range.location];
			[m_coreResourcePaths addObject:filename];

			// Add other paths and prefixes.

			for (NSString *comp in [query componentsSeparatedByString:@"&"]) {
				NSArray *pair = [comp componentsSeparatedByString:@"="];

				if (pair.count == 2) {
					NSString *key = pair[0];
					NSString *val = pair[1];

					key = [key stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
					val = [val stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];

					for (NSString *path in [val componentsSeparatedByString:@","]) {
						NSString *s = [path stringByTrimmingCharactersInSet:
							[NSCharacterSet whitespaceAndNewlineCharacterSet]];

						if ([key isEqualToString:@"corePaths"]) {
							m_provideCoreResources = YES;
							[m_coreResourcePaths addObject:s];
						}
						else if ([key isEqualToString:@"corePrefixes"]) {
							m_provideCoreResources = YES;
							[m_coreResourcePrefixes addObject:s];
						}
					}
				}
			}
		}
	}

	// Remove trailing query string and hash from the path.

	NSRange rangeQ = [path rangeOfString:@"?"];
	if (rangeQ.location != NSNotFound) {
		path = [path substringToIndex:rangeQ.location];
	}
	NSRange rangeH = [path rangeOfString:@"#"];
	if (rangeH.location != NSNotFound) {
		path = [path substringToIndex:rangeH.location];
	}

	// Handle the special payload responses.

	NSData *specialPayloadAnnotationsCSS = m_packageResourceServer.specialPayloadAnnotationsCSS;
	NSData *specialPayloadMathJaxJS = m_packageResourceServer.specialPayloadMathJaxJS;
	
	if (specialPayloadMathJaxJS != nil && specialPayloadMathJaxJS.length > 0) {
		NSString * math = @"readium_MathJax.js";
		if ([path hasPrefix:math]) {
			
			RDPackageResourceDataResponse *dataResponse = [[RDPackageResourceDataResponse alloc]
														   initWithData:specialPayloadMathJaxJS];
			dataResponse.contentType = @"text/javascript";
			return dataResponse;
		}
	}
	
	if (specialPayloadAnnotationsCSS != nil && specialPayloadAnnotationsCSS.length > 0) {
		NSString * annotationsCSS = @"readium_Annotations.css";
		if ([path hasPrefix:annotationsCSS]) {
			
			RDPackageResourceDataResponse *dataResponse = [[RDPackageResourceDataResponse alloc]
														   initWithData:specialPayloadAnnotationsCSS];
			dataResponse.contentType = @"text/css";
			return dataResponse;
		}
	}

	// Fake script request, immediately invoked after epubReadingSystem hook is in place,
	// => push the global window.navigator.epubReadingSystem into the iframe(s)

	if ([path hasSuffix:@"readium_epubReadingSystem_inject.js"]) {

		NSString* cmd = @"var epubRSInject = function(win, isTopIframe) { if (win.frames) { for (var i = 0; i < win.frames.length; i++) { var iframe = win.frames[i]; if (iframe.readium_set_epubReadingSystem) { iframe.readium_set_epubReadingSystem(window.navigator.epubReadingSystem, isTopIframe); } epubRSInject(iframe, false); } } }; epubRSInject(window, true);";
		// Iterate top-level iframes, inject global window.navigator.epubReadingSystem if the expected hook function exists ( readium_set_epubReadingSystem() ).
		[m_packageResourceServer executeJavaScript:cmd];
		
		NSString* noop = @"(function(){})()"; // prevents 404 (WebConsole message)
		NSData *data = [noop dataUsingEncoding:NSUTF8StringEncoding];
		RDPackageResourceDataResponse *dataResponse = [[RDPackageResourceDataResponse alloc] initWithData:data];
		dataResponse.contentType = @"text/javascript";
		return dataResponse;
	}

	// Handle requests for core resources.

	BOOL provideCoreResource = m_provideCoreResources && [m_coreResourcePaths containsObject:path];

	if (!provideCoreResource && m_provideCoreResources) {
		for (NSString *prefix in m_coreResourcePrefixes) {
			if ([path hasPrefix:prefix]) {
				provideCoreResource = YES;
				break;
			}
		}
	}

	if (provideCoreResource) {
		path = [[NSBundle mainBundle].resourcePath stringByAppendingPathComponent:path];
		NSData *data = [[NSData alloc] initWithContentsOfFile:path];

		if (data == nil) {
			NSLog(@"The core resource is missing! (%@)", path);
			return nil;
		}

		RDPackageResourceDataResponse *dataResponse = [[RDPackageResourceDataResponse alloc]
			initWithData:data];

		NSString *ext = path.lowercaseString.pathExtension;

		if ([ext isEqualToString:@"html"]) {
			dataResponse.contentType = @"text/html";
		}
		else if ([ext isEqualToString:@"css"]) {
			dataResponse.contentType = @"text/css";
		}
		else if ([ext isEqualToString:@"js"]) {
			dataResponse.contentType = @"text/javascript";
		}
		else {
			NSLog(@"The core resource's extension is not recognized! (%@)", path);
		}

		return dataResponse;
	}

	// Handle requests for resources within the EPUB. Synchronize using a process-level lock to
	// guard against multiple threads accessing a resource byte stream, which may cause instability.

	@synchronized ([RDPackageResourceServer resourceLock]) {
		RDPackageResource *resource = [m_packageResourceServer.package resourceAtRelativePath:path];
		
		if (resource == nil) {
			NSLog(@"No resource found! (%@)", path);
			return nil;
		}

		NSString* ext = [[path pathExtension] lowercaseString];
		bool isHTML = [ext isEqualToString:@"xhtml"] || [ext isEqualToString:@"html"] || [resource.mimeType isEqualToString:@"application/xhtml+xml"] || [resource.mimeType isEqualToString:@"text/html"];

		if (isHTML) {
			NSString * FALLBACK_HTML = @"<?xml version=\"1.0\" encoding=\"UTF-8\"?><html xmlns=\"http://www.w3.org/1999/xhtml\"><head><title>HTML READ ERROR</title></head><body>ERROR READING HTML BYTES!</body></html>";

			NSData *data = [resource readDataFull];
			if (data == nil || data.length == 0)
			{
				data = [FALLBACK_HTML dataUsingEncoding:NSUTF8StringEncoding];
			}

			NSXMLParser *xmlparser = [[NSXMLParser alloc] initWithData:data];
			[xmlparser setShouldResolveExternalEntities:NO];
			BOOL isXhtmlWellFormed = [xmlparser parse];

			if (isXhtmlWellFormed == NO)
			{
				NSLog(@"XHTML PARSE ERROR: %@", xmlparser.parserError);

				// Can be used to check / debug encoding issues
				NSString * dataStr = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
				//NSLog(@"XHTML SOURCE: %@", dataStr);

				// FORCE HTML WebView parser
				//@"application/xhtml+xml"
				resource.mimeType = @"text/html";
			}

			NSString* source = [self htmlFromData:data];
			if (source == nil || source.length == 0)
			{
				source = FALLBACK_HTML;
			}

			NSString *pattern = @"(<head[^>]*>)";
			NSError *error = nil;
			NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:pattern options:NSRegularExpressionCaseInsensitive error:&error];
			if(error != nil) {
				NSLog(@"RegEx error: %@", error);
			} else {
				// Installs "hook" function so that top-level window (application) can later inject the window.navigator.epubReadingSystem into this HTML document's iframe
				NSString *inject_epubReadingSystem1 = [NSString stringWithFormat:@"<script id=\"readium_epubReadingSystem_inject1\" type=\"text/javascript\">\n//<![CDATA[\n%@\n//]]>\n</script>",
													   @"window.readium_set_epubReadingSystem = function (obj, isTopIframe) {\
													   \nwindow.navigator.epubReadingSystem = obj;\
													   \nwindow.readium_set_epubReadingSystem = undefined;\
													   \nvar el1 = document.getElementById(\"readium_epubReadingSystem_inject1\");\
													   \nif (el1 && el1.parentNode) { el1.parentNode.removeChild(el1); }\
													   \nvar el2 = document.getElementById(\"readium_epubReadingSystem_inject2\");\
													   \nif (el2 && el2.parentNode) { el2.parentNode.removeChild(el2); }\
													   \nif (isTopIframe) {\
													   \nwindow.parent = window;\
													   \nwindow.top = window;\
													   \n}\
													   \n};"];

				// Fake script, generates HTTP request => triggers the push of window.navigator.epubReadingSystem into this HTML document's iframe
				NSString *inject_epubReadingSystem2 = [NSString stringWithFormat:@"<script id=\"readium_epubReadingSystem_inject2\" type=\"text/javascript\" src=\"/%ld/readium_epubReadingSystem_inject.js\"> </script>", m_epubReadingSystem_Counter++];

				NSString *inject_mathJax = @"";
				if ([source rangeOfString:@"<math"].location != NSNotFound || [source rangeOfString:@"<m:math"].location != NSNotFound) {
					inject_mathJax = @"<script type=\"text/javascript\" src=\"/readium_MathJax.js\"> </script>";
				}

				NSString *newSource = [regex stringByReplacingMatchesInString:source options:0 range:NSMakeRange(0, [source length]) withTemplate:
									   [NSString stringWithFormat:@"%@\n%@\n%@\n%@", @"$1", inject_epubReadingSystem1, inject_epubReadingSystem2, inject_mathJax]];
				if (newSource != nil && newSource.length > 0) {
					NSData * newData = [newSource dataUsingEncoding:NSUTF8StringEncoding];
					if (newData != nil) {
						RDPackageResourceDataResponse *dataResponse = [[RDPackageResourceDataResponse alloc]
																	   initWithData:newData];

						if (resource.mimeType) {
							dataResponse.contentType = resource.mimeType;
						}

						return dataResponse;
					}
				}
			}
		}

		return [[RDPackageResourceResponse alloc] initWithResource:resource];
	}
}


+ (void)initialize {
	m_coreResourcePaths = [[NSMutableArray alloc] init];
	m_coreResourcePrefixes = [[NSMutableArray alloc] init];
}


+ (void)setPackageResourceServer:(RDPackageResourceServer *)packageResourceServer {
	m_packageResourceServer = packageResourceServer;

	[m_coreResourcePaths removeAllObjects];
	[m_coreResourcePrefixes removeAllObjects];

	m_provideCoreResources = NO;
	m_provideCoreResourcesIsKnown = NO;
}


@end
