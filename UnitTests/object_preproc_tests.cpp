//
//  object_preproc_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-02-01.
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

#include "../ePub3/ePub/object_preprocessor.h"
#include "../ePub3/ePub/container.h"
#include "../ePub3/ePub/package.h"
#include "catch.hpp"

#define EPUB_PATH "TestData/widget-figure-gallery-20121022.epub"

static const char gNormalObject[] = R"raw(<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml" xmlns:epub="http://www.idpf.org/2007/ops" lang="en"
xml:lang="en">
  <head>
    <title>Testing epub:switch Preprocessing</title>
    <link href="css/epub.css" type="text/css" rel="stylesheet" />
  </head>
  <body>
    <object data="bob.mp4" type="video/mpeg">
      <param autoplay="true" />
    </object>
  </body>
</html>)raw";

static const char gGalleryObject[] = R"raw(<?xml version="1.0" encoding="UTF-8"?>
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en"
	xmlns:epub="http://www.idpf.org/2007/ops">
	<head>
		<meta charset="utf-8"></meta>
		<title>Phases of the Moon</title>
        <link rel="stylesheet" type="text/css" href="epub.css"/>
	</head>
	<body>
		<section id="ch1">
			<h1>Phases of the Moon</h1>
			
    			<object data="moon-phases.xml" type="application/x-epub-figure-gallery" id="moon-figures">
    				<!-- object fallback is a series of figures -->
    				<figure class="fallback">
    					<img src="images/moon-images/1.new-moon.jpg"/>
    					<figcaption>
    						<h2>New Moon</h2>
    						<p>By the modern definition, New Moon occurs when the Moon and Sun are at the same geocentric ecliptic longitude. The part of the Moon facing us is completely in shadow then. Pictured here is the traditional New Moon, the earliest visible waxing crescent, which signals the start of a new month in many lunar and lunisolar calendars.</p>
    					</figcaption>
    				</figure>
				
    				<figure class="fallback">
    					<img src="images/moon-images/2.waxing-crescent.jpg"/>
    					<figcaption>
    						<h2>Waxing Crescent</h2>
    						<p>Visible toward the southwest in early evening.</p>
    					</figcaption>
    				</figure>
				
    				<figure class="fallback">
    					<img src="images/moon-images/3.first-quarter.jpg"/>
    					<figcaption>
    						<h2>First Quarter</h2>
    						<p>Visible high in the southern sky in early evening.</p>
    					</figcaption>
    				</figure>
				
    				<figure class="fallback">
    					<img src="images/moon-images/4.waxing-gibbous.jpg"/>
    					<figcaption>
    						<h2>Waxing Gibbous</h2>
    						<p>Visible to the southeast in early evening, up for most of the night.</p>
    					</figcaption>
    				</figure>
				
    				<figure class="fallback">
    					<img src="images/moon-images/5.full-moon.jpg"/>
    					<figcaption>
    						<h2>Full Moon</h2>
    						<p>Rises at sunset, high in the sky around midnight. Visible all night.</p>
    					</figcaption>
    				</figure>
				
    				<figure class="fallback">
    					<img src="images/moon-images/6.waning-gibbous.jpg"/>
    					<figcaption>
    						<h2>Waning Gibbous</h2>
    						<p>Rises after sunset, high in the sky after midnight, visible to the southwest after sunrise.</p>
    					</figcaption>
    				</figure>
				
    				<figure class="fallback">
    					<img src="images/moon-images/7.third-quarter.jpg"/>
    					<figcaption>
    						<h2>Third Quarter</h2>
    						<p>Rises around midnight, visible to the south after sunrise.</p>
    					</figcaption>
    				</figure>
				
    				<figure class="fallback">
    					<img src="images/moon-images/8.waning-crescent.jpg"/>
    					<figcaption>
    						<h2>Waning Crescent</h2>
    						<p>Low to the east before sunrise.</p>
    					</figcaption>
    				</figure>
    			</object>
        </section>
    </body>
</html>)raw";

static const char gShortGalleryObject[] = R"raw(<?xml version="1.0" encoding="UTF-8"?>
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en"
	xmlns:epub="http://www.idpf.org/2007/ops">
	<head>
		<meta charset="utf-8"></meta>
		<title>Phases of the Moon</title>
        <link rel="stylesheet" type="text/css" href="epub.css"/>
	</head>
	<body>
		<section id="ch1">
			<h1>Phases of the Moon</h1>
    			<object data="moon-phases.xml" type="application/x-epub-figure-gallery" id="moon-figures" />
        </section>
    </body>
</html>)raw";

static const char gGalleryIFrame[] = R"raw(<?xml version="1.0" encoding="UTF-8"?>
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en"
	xmlns:epub="http://www.idpf.org/2007/ops">
	<head>
		<meta charset="utf-8"></meta>
		<title>Phases of the Moon</title>
        <link rel="stylesheet" type="text/css" href="epub.css"/>
	</head>
	<body>
		<section id="ch1">
			<h1>Phases of the Moon</h1>
			
    			<iframe src="epub3://code.google.com.epub-samples.widget-figure-gallery/EPUB/figure-gallery-widget/figure-gallery-impl.xhtml?src=moon-phases.xml&type=application%2Fx-epub-figure-gallery" srcdoc="epub3://code.google.com.epub-samples.widget-figure-gallery/EPUB/figure-gallery-widget/figure-gallery-impl.xhtml?src=moon-phases.xml&type=application%2Fx-epub-figure-gallery" id="moon-figures" sandbox="allow-forms allow-scripts allow-same-origin" seamless="seamless"></iframe><form action="epub3://code.google.com.epub-samples.widget-figure-gallery/EPUB/figure-gallery-widget/figure-gallery-impl.xhtml?src=moon-phases.xml&type=application%2Fx-epub-figure-gallery" method="get" id="moon-figures-form"><button type="submit" id="moon-figures-button">Open Fullscreen</button></form>
        </section>
    </body>
</html>)raw";

static const char gGalleryIFrameFrench[] = R"raw(<?xml version="1.0" encoding="UTF-8"?>
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en"
	xmlns:epub="http://www.idpf.org/2007/ops">
	<head>
		<meta charset="utf-8"></meta>
		<title>Phases of the Moon</title>
        <link rel="stylesheet" type="text/css" href="epub.css"/>
	</head>
	<body>
		<section id="ch1">
			<h1>Phases of the Moon</h1>
			
    			<iframe src="epub3://code.google.com.epub-samples.widget-figure-gallery/EPUB/figure-gallery-widget/figure-gallery-impl.xhtml?src=moon-phases.xml&type=application%2Fx-epub-figure-gallery" srcdoc="epub3://code.google.com.epub-samples.widget-figure-gallery/EPUB/figure-gallery-widget/figure-gallery-impl.xhtml?src=moon-phases.xml&type=application%2Fx-epub-figure-gallery" id="moon-figures" sandbox="allow-forms allow-scripts allow-same-origin" seamless="seamless"></iframe><form action="epub3://code.google.com.epub-samples.widget-figure-gallery/EPUB/figure-gallery-widget/figure-gallery-impl.xhtml?src=moon-phases.xml&type=application%2Fx-epub-figure-gallery" method="get" id="moon-figures-form"><button type="submit" id="moon-figures-button">Ouvrir</button></form>
        </section>
    </body>
</html>)raw";

using namespace ePub3;

TEST_CASE("Normal object tags are left unchanged", "")
{
    Container c(EPUB_PATH);
    const Package* pkg = c.Packages()[0];
    
    ObjectPreprocessor proc(pkg);
    
    size_t outLen = 0;
    char* input = strdup(gNormalObject);
    char* output = reinterpret_cast<char*>(proc.FilterData(input, sizeof(gNormalObject), &outLen));
    
    INFO("Unaltered output:\n" << string(output, outLen));
    REQUIRE(outLen == sizeof(gNormalObject));
    REQUIRE(strncmp(input, output, outLen) == 0);
    
    if ( output != input )
        delete [] output;
    free(input);
}

TEST_CASE("Object tags for bound media should be replaced by iframes and buttons", "")
{
    Container c(EPUB_PATH);
    const Package* pkg = c.Packages()[0];
    
    ObjectPreprocessor proc(pkg);
    
    size_t outLen = 0;
    char* input = strdup(gGalleryObject);
    char* output = reinterpret_cast<char*>(proc.FilterData(input, sizeof(gGalleryObject), &outLen));
    
    INFO("IFrame output:\n" << string(output, outLen));
    REQUIRE(outLen == sizeof(gGalleryIFrame));
    REQUIRE(strncmp(gGalleryIFrame, output, outLen) == 0);
}

TEST_CASE("The title of the 'Open Fullscreen' button may be replaced", "")
{
    Container c(EPUB_PATH);
    const Package* pkg = c.Packages()[0];
    
    ObjectPreprocessor proc(pkg, "Ouvrir");
    
    size_t outLen = 0;
    char* input = strdup(gGalleryObject);
    char* output = reinterpret_cast<char*>(proc.FilterData(input, sizeof(gGalleryObject), &outLen));
    
    INFO("IFrame output:\n" << string(output, outLen));
    REQUIRE(outLen == sizeof(gGalleryIFrameFrench));
    REQUIRE(strncmp(gGalleryIFrameFrench, output, outLen) == 0);
}
