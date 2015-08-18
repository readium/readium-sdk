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


static long m_epubReadingSystem_Counter = 0;
static __weak RDPackageResourceServer *m_packageResourceServer = nil;


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
		![method isEqualToString:@"GET"] ||
		path == nil ||
		path.length == 0)
	{
		return nil;
	}
	
	if (path != nil) {
		if ([path hasPrefix:@"/"]) {
			path = [path substringFromIndex:1];
		}

		NSRange rangeQ = [path rangeOfString:@"?"];
		if (rangeQ.location != NSNotFound) {
			path = [path substringToIndex:rangeQ.location];
		}
		NSRange rangeH = [path rangeOfString:@"#"];
		if (rangeH.location != NSNotFound) {
			path = [path substringToIndex:rangeH.location];
		}
	}
	
	NSObject <HTTPResponse> *response = nil;
	NSData *specialPayloadAnnotationsCSS = m_packageResourceServer.specialPayloadAnnotationsCSS;
	NSData *specialPayloadMathJaxJS = m_packageResourceServer.specialPayloadMathJaxJS;
	
	if (specialPayloadMathJaxJS != nil && specialPayloadMathJaxJS.length > 0) {
		NSString * math = @"readium_MathJax.js";
		if ([path hasPrefix:math]) {
			
			RDPackageResourceDataResponse *dataResponse = [[RDPackageResourceDataResponse alloc]
														   initWithData:specialPayloadMathJaxJS];
			dataResponse.contentType = @"text/javascript";
			
			response = dataResponse;
			return response;
		}
	}
	
	if (specialPayloadAnnotationsCSS != nil && specialPayloadAnnotationsCSS.length > 0) {
		NSString * annotationsCSS = @"readium_Annotations.css";
		if ([path hasPrefix:annotationsCSS]) {
			
			RDPackageResourceDataResponse *dataResponse = [[RDPackageResourceDataResponse alloc]
														   initWithData:specialPayloadAnnotationsCSS];
			dataResponse.contentType = @"text/css";
			
			response = dataResponse;
			return response;
		}
	}
	
	// Fake script request, immediately invoked after epubReadingSystem hook is in place,
	// => push the global window.navigator.epubReadingSystem into the iframe(s)
	NSString * eprs = @"readium_epubReadingSystem_inject.js";
	if ([path hasSuffix:eprs]) {

		NSString* cmd = @"var epubRSInject = function(win) { if (win.frames) { for (var i = 0; i < win.frames.length; i++) { var iframe = win.frames[i]; if (iframe.readium_set_epubReadingSystem) { iframe.readium_set_epubReadingSystem(window.navigator.epubReadingSystem); } epubRSInject(iframe); } } }; epubRSInject(window);";
		// Iterate top-level iframes, inject global window.navigator.epubReadingSystem if the expected hook function exists ( readium_set_epubReadingSystem() ).
		[m_packageResourceServer executeJavaScript:cmd];
		
		NSString* noop = @"(function(){})()"; // prevents 404 (WebConsole message)
		NSData *data = [noop dataUsingEncoding:NSUTF8StringEncoding];
		RDPackageResourceDataResponse *dataResponse = [[RDPackageResourceDataResponse alloc] initWithData:data];
		dataResponse.contentType = @"text/javascript";
		return dataResponse;
	}
	
	// Synchronize using a process-level lock to guard against multiple threads accessing a
	// resource byte stream, which may lead to instability.
	
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

			BOOL isXhtmlWellFormed = YES;
			@try
			{
				NSXMLParser *xmlparser = [[NSXMLParser alloc] initWithData:data];
				//[xmlparser setDelegate:self];
				[xmlparser setShouldResolveExternalEntities:NO];
				isXhtmlWellFormed = [xmlparser parse];

				if (isXhtmlWellFormed == NO)
				{
					NSError * error = [xmlparser parserError];
					NSLog(@"XHTML PARSE ERROR: %@", error);
				}
			}
			@catch (NSException *ex)
			{
				NSLog(@"XHTML parse exception: %@", ex);
				isXhtmlWellFormed = NO;
			}

			if (isXhtmlWellFormed == NO)
			{
				// Can be used to check / debug encoding issues
				NSString * dataStr = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];

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
													   @"window.readium_set_epubReadingSystem = function (obj) {\
													   \nwindow.navigator.epubReadingSystem = obj;\
													   \nwindow.readium_set_epubReadingSystem = undefined;\
													   \nvar el1 = document.getElementById(\"readium_epubReadingSystem_inject1\");\
													   \nif (el1 && el1.parentNode) { el1.parentNode.removeChild(el1); }\
													   \nvar el2 = document.getElementById(\"readium_epubReadingSystem_inject2\");\
													   \nif (el2 && el2.parentNode) { el2.parentNode.removeChild(el2); }\
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

						response = dataResponse;
						return response;
					}
				}
			}
		}

		RDPackageResourceResponse *resourceResponse = [[RDPackageResourceResponse alloc] initWithResource:resource];
		response = resourceResponse;
	}
	
	return response;
}


+ (void)setPackageResourceServer:(RDPackageResourceServer *)packageResourceServer {
	m_packageResourceServer = packageResourceServer;
}


@end
