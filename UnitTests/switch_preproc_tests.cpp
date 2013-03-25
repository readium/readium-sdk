//
//  switch_preproc_tests.cpp
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

#include "../ePub3/ePub/switch_preprocessor.h"
#include "catch.hpp"

using namespace ePub3;

static const char gInput[] = R"raw(<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml" xmlns:epub="http://www.idpf.org/2007/ops" lang="en"
xml:lang="en">
  <head>
    <title>Testing epub:switch Preprocessing</title>
    <link href="css/epub.css" type="text/css" rel="stylesheet" />
  </head>
  <body>
    <epub:switch id="cmlSwitch">
      <epub:case required-namespace="http://www.xml-cml.org/schema">
        <cml xmlns="http://www.xml-cml.org/schema">
          <molecule id="sulfuric-acid">
            <formula id="f1" concise="H 2 S 1 O 4"/>
          </molecule>
        </cml>
      </epub:case>
      <epub:default>
        <p>H<sub>2</sub>SO<sub>4</sub></p>
      </epub:default>
    </epub:switch>
    <epub:switch id="mathmlSwitch">
      <epub:case required-namespace="http://www.w3.org/1998/Math/MathML">
        <math xmlns="http://www.w3.org/1998/Math/MathML">
          <mrow>
            <mn>2</mn>
            <mo> &#x2061;<!--INVISIBLE TIMES--></mo>
            <mi>x</mi>
          </mrow>
          <mrow>
            <mo>+</mo>
            <mi>y</mi>
            <mo>-</mo>
            <mi>z</mi>
          </mrow>
        </math>
      </epub:case>
      <epub:default>
        <p>2x + y - z</p>
      </epub:default>
    </epub:switch>
  </body>
</html>
)raw";

static const char gDefault[] = R"raw(<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml" xmlns:epub="http://www.idpf.org/2007/ops" lang="en"
xml:lang="en">
  <head>
    <title>Testing epub:switch Preprocessing</title>
    <link href="css/epub.css" type="text/css" rel="stylesheet" />
  </head>
  <body>
    
        <p>H<sub>2</sub>SO<sub>4</sub></p>
      
    
        <p>2x + y - z</p>
      
  </body>
</html>
)raw";

static const char gCMLOnly[] = R"raw(<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml" xmlns:epub="http://www.idpf.org/2007/ops" lang="en"
xml:lang="en">
  <head>
    <title>Testing epub:switch Preprocessing</title>
    <link href="css/epub.css" type="text/css" rel="stylesheet" />
  </head>
  <body>
    
        <cml xmlns="http://www.xml-cml.org/schema">
          <molecule id="sulfuric-acid">
            <formula id="f1" concise="H 2 S 1 O 4"/>
          </molecule>
        </cml>
      
    
        <p>2x + y - z</p>
      
  </body>
</html>
)raw";

static const char gMathMLOnly[] = R"raw(<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml" xmlns:epub="http://www.idpf.org/2007/ops" lang="en"
xml:lang="en">
  <head>
    <title>Testing epub:switch Preprocessing</title>
    <link href="css/epub.css" type="text/css" rel="stylesheet" />
  </head>
  <body>
    
        <p>H<sub>2</sub>SO<sub>4</sub></p>
      
    
        <math xmlns="http://www.w3.org/1998/Math/MathML">
          <mrow>
            <mn>2</mn>
            <mo> &#x2061;<!--INVISIBLE TIMES--></mo>
            <mi>x</mi>
          </mrow>
          <mrow>
            <mo>+</mo>
            <mi>y</mi>
            <mo>-</mo>
            <mi>z</mi>
          </mrow>
        </math>
      
  </body>
</html>
)raw";

static const char gCMLAndMathML[] = R"raw(<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml" xmlns:epub="http://www.idpf.org/2007/ops" lang="en"
xml:lang="en">
  <head>
    <title>Testing epub:switch Preprocessing</title>
    <link href="css/epub.css" type="text/css" rel="stylesheet" />
  </head>
  <body>
    
        <cml xmlns="http://www.xml-cml.org/schema">
          <molecule id="sulfuric-acid">
            <formula id="f1" concise="H 2 S 1 O 4"/>
          </molecule>
        </cml>
      
    
        <math xmlns="http://www.w3.org/1998/Math/MathML">
          <mrow>
            <mn>2</mn>
            <mo> &#x2061;<!--INVISIBLE TIMES--></mo>
            <mi>x</mi>
          </mrow>
          <mrow>
            <mo>+</mo>
            <mi>y</mi>
            <mo>-</mo>
            <mi>z</mi>
          </mrow>
        </math>
      
  </body>
</html>
)raw";

static const char gCommentedInput[] = R"raw(<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml" xmlns:epub="http://www.idpf.org/2007/ops" lang="en"
xml:lang="en">
  <head>
    <title>Testing epub:switch Preprocessing</title>
    <link href="css/epub.css" type="text/css" rel="stylesheet" />
  </head>
  <body><!--
    <epub:switch id="cmlSwitch">
      <epub:case required-namespace="http://www.xml-cml.org/schema">
        <cml xmlns="http://www.xml-cml.org/schema">
          <molecule id="sulfuric-acid">
            <formula id="f1" concise="H 2 S 1 O 4"/>
          </molecule>
        </cml>
      </epub:case>
      <epub:default>-->
        <p>H<sub>2</sub>SO<sub>4</sub></p><!--
      </epub:default>
    </epub:switch>-->
    <!--<epub:switch id="mathmlSwitch">
      <epub:case required-namespace="http://www.w3.org/1998/Math/MathML">
        <math xmlns="http://www.w3.org/1998/Math/MathML">
          <mrow>
            <mn>2</mn>
            <mo> &#x2061;<!--INVISIBLE TIMES--></mo>
            <mi>x</mi>
          </mrow>
          <mrow>
            <mo>+</mo>
            <mi>y</mi>
            <mo>-</mo>
            <mi>z</mi>
          </mrow>
        </math>
      </epub:case>
      <epub:default>
        --><p>2x + y - z</p>
      <!--</epub:default>
    </epub:switch>
  --></body>
</html>
)raw";

static const char gCommentedDefaultOutput[] = R"raw(<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml" xmlns:epub="http://www.idpf.org/2007/ops" lang="en"
xml:lang="en">
  <head>
    <title>Testing epub:switch Preprocessing</title>
    <link href="css/epub.css" type="text/css" rel="stylesheet" />
  </head>
  <body>
    
        <p>H<sub>2</sub>SO<sub>4</sub></p>
      
    
        <p>2x + y - z</p>
      
  </body>
</html>
)raw";

static const char gTotallyCommentedInput[] = R"raw(<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml" xmlns:epub="http://www.idpf.org/2007/ops" lang="en"
xml:lang="en">
  <head>
    <title>Testing epub:switch Preprocessing</title>
    <link href="css/epub.css" type="text/css" rel="stylesheet" />
  </head>
  <body><!--
    <epub:switch id="cmlSwitch">
      <epub:case required-namespace="http://www.xml-cml.org/schema">
        <cml xmlns="http://www.xml-cml.org/schema">
          <molecule id="sulfuric-acid">
            <formula id="f1" concise="H 2 S 1 O 4"/>
          </molecule>
        </cml>
      </epub:case>
      <epub:default>
        <p>H<sub>2</sub>SO<sub>4</sub></p>
      </epub:default>
    </epub:switch>-->
    <p>H<sub>2</sub>SO<sub>4</sub></p>
    <!--<epub:switch id="mathmlSwitch">
      <epub:case required-namespace="http://www.w3.org/1998/Math/MathML">
        <math xmlns="http://www.w3.org/1998/Math/MathML">
          <mrow>
            <mn>2</mn>
            <mo> &#x2061;<!--INVISIBLE TIMES--></mo>
            <mi>x</mi>
          </mrow>
          <mrow>
            <mo>+</mo>
            <mi>y</mi>
            <mo>-</mo>
            <mi>z</mi>
          </mrow>
        </math>
      </epub:case>
      <epub:default>
        <p>2x + y - z</p>
      </epub:default>
    </epub:switch>-->
    <p>2x + y - z</p>
  </body>
</html>
)raw";

static const char gTotallyCommentedOutput[] = R"raw(<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml" xmlns:epub="http://www.idpf.org/2007/ops" lang="en"
xml:lang="en">
  <head>
    <title>Testing epub:switch Preprocessing</title>
    <link href="css/epub.css" type="text/css" rel="stylesheet" />
  </head>
  <body><!--
    
        <p>H<sub>2</sub>SO<sub>4</sub></p>
      -->
    <p>H<sub>2</sub>SO<sub>4</sub></p>
    <!--
        <p>2x + y - z</p>
      -->
    <p>2x + y - z</p>
  </body>
</html>
)raw";

TEST_CASE("Default processor should render epub:default only", "")
{
    SwitchPreprocessor proc;
    
    size_t outLen = 0;
    char* input = strdup(gInput);
    char* output = reinterpret_cast<char*>(proc.FilterData(input, sizeof(gInput), &outLen));
    output[outLen] = '\0';
    
    INFO("Output:\n" << output);
    REQUIRE(strncmp(output, gDefault, std::min(sizeof(gDefault), outLen)) == 0);
    
    if ( output != input )
        delete [] output;
    free(input);
}

TEST_CASE("Processors should be able to support a mix of supported and unsupported namespaces", "")
{
    SwitchPreprocessor cmlProc({"http://www.xml-cml.org/schema"}), mathmlProc({MathMLNamespaceURI});
    
    size_t outLen = 0;
    char* input = strdup(gInput);
    char* output = reinterpret_cast<char*>(cmlProc.FilterData(input, sizeof(gInput), &outLen));
    output[outLen] = '\0';
    
    INFO("CML output:\n" << output);
    REQUIRE(strncmp(output, gCMLOnly, std::min(sizeof(gCMLOnly), outLen)) == 0);
    
    if ( output != input )
        delete [] output;
    free(input);
    
    input = strdup(gInput);
    output = reinterpret_cast<char*>(mathmlProc.FilterData(input, sizeof(gInput), &outLen));
    output[outLen] = '\0';
    
    INFO("MathML output:\n" << output);
    REQUIRE(strncmp(output, gMathMLOnly, std::min(sizeof(gMathMLOnly), outLen)) == 0);
    
    if ( output != input )
        delete [] output;
    free(input);
}

TEST_CASE("Processors should be able to support multiple supported namespaces", "")
{
    SwitchPreprocessor proc({"http://www.xml-cml.org/schema", MathMLNamespaceURI});
    
    size_t outLen = 0;
    char* input = strdup(gInput);
    char* output = reinterpret_cast<char*>(proc.FilterData(input, sizeof(gInput), &outLen));
    output[outLen] = '\0';
    
    INFO("Output:\n" << output);
    REQUIRE(strncmp(output, gCMLAndMathML, std::min(sizeof(gCMLAndMathML), outLen)) == 0);
    
    if ( output != input )
        delete [] output;
    free(input);
}

TEST_CASE("Processors should gracefully handle comments around non-default switched content", "")
{
    SwitchPreprocessor proc;
    
    size_t outLen = 0;
    char* input = strdup(gCommentedInput);
    char* output = reinterpret_cast<char*>(proc.FilterData(input, sizeof(gCommentedInput), &outLen));
    output[outLen] = '\0';
    
    INFO("Output:\n" << output);
    REQUIRE(strncmp(output, gCommentedDefaultOutput, std::min(sizeof(gCommentedDefaultOutput), outLen)) == 0);
    
    if ( output != input )
        delete [] output;
    free(input);
}

TEST_CASE("Processors should NOT uncomment switch constructs which have been commented out in their entirety", "Note that the switches themselves will still be processed.")
{
    SwitchPreprocessor proc;
    
    size_t outLen = 0;
    char* input = strdup(gTotallyCommentedInput);
    char* output = reinterpret_cast<char*>(proc.FilterData(input, sizeof(gTotallyCommentedInput), &outLen));
    output[outLen] = '\0';
    
    INFO("Output:\n" << output);
    REQUIRE(strncmp(output, gTotallyCommentedOutput, std::min(sizeof(gTotallyCommentedOutput), outLen)) == 0);
    
    if ( output != input )
        delete [] output;
    free(input);
}
