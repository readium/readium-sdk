//
//  RDPackageResourceDataResponse.m
//  SDKLauncher-iOS
//
//  Created by Oliver Eikemeier on 04.04.14.
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

#import "RDPackageResourceDataResponse.h"
#import "NSDate+RDDateAsString.h"

NSString * const kDataCacheControlHTTPHeader = @"no-transform,public,max-age=3000,s-maxage=9000";

@implementation RDPackageResourceDataResponse

- (NSDictionary *)httpHeaders {
    // Add cache-control, expires and last-modified HTTP headers so the webview caches shared assets
    NSDate *now = [NSDate date];
    NSString *nowStr = [now dateAsString];
    NSString *expStr = [[now dateByAddingTimeInterval:60*60*24*10] dateAsString];
	if (self.contentType) {
		return @{@"Content-Type": self.contentType, @"Cache-Control": kDataCacheControlHTTPHeader, @"Last-Modified": nowStr, @"Expires": expStr};
	}
	else {
		return @{@"Cache-Control": kDataCacheControlHTTPHeader, @"Last-Modified": nowStr, @"Expires": expStr};
	}
}
 
/**
 * Gets the current date and time, formatted properly (according to RFC) for insertion into an HTTP header.
 **/
- (NSString *)dateAsString:(NSDate *)date
{
    // From Apple's Documentation (Data Formatting Guide -> Date Formatters -> Cache Formatters for Efficiency):
    //
    // "Creating a date formatter is not a cheap operation. If you are likely to use a formatter frequently,
    // it is typically more efficient to cache a single instance than to create and dispose of multiple instances.
    // One approach is to use a static variable."
    //
    // This was discovered to be true in massive form via issue #46:
    //
    // "Was doing some performance benchmarking using instruments and httperf. Using this single optimization
    // I got a 26% speed improvement - from 1000req/sec to 3800req/sec. Not insignificant.
    // The culprit? Why, NSDateFormatter, of course!"
    //
    // Thus, we are using a static NSDateFormatter here.
    
    static NSDateFormatter *df;
    
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        
        // Example: Sun, 06 Nov 1994 08:49:37 GMT
        
        df = [[NSDateFormatter alloc] init];
        [df setFormatterBehavior:NSDateFormatterBehavior10_4];
        [df setTimeZone:[NSTimeZone timeZoneWithAbbreviation:@"GMT"]];
        [df setDateFormat:@"EEE, dd MMM y HH:mm:ss 'GMT'"];
        [df setLocale:[[NSLocale alloc] initWithLocaleIdentifier:@"en_US"]];
        
        // For some reason, using zzz in the format string produces GMT+00:00
    });
    
    return [df stringFromDate:date];
}

@end
