//
//  switch_preproc_tests.cpp
//  ePub3
//
//  Created by Jim Dovey on 2013-02-01.
//
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//  3. Neither the name of the organization nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
//

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


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
    std::unique_ptr<FilterContext> ctx(proc.MakeFilterContext(nullptr));
    
    size_t outLen = 0;
    char* input = strdup(gInput);
    char* output = reinterpret_cast<char*>(proc.FilterData(ctx.get(), input, sizeof(gInput), &outLen));
    output[outLen] = '\0';
    
    INFO("Output:\n" << output);
    REQUIRE(strncmp(output, gDefault, std::min(sizeof(gDefault), outLen)) == 0);
    
    if ( output != input )
        delete [] output;
    free(input);
}

TEST_CASE("Processors should be able to support a mix of supported and unsupported namespaces", "")
{
    SwitchPreprocessor proc;
    std::unique_ptr<FilterContext> ctx(proc.MakeFilterContext(nullptr));
    
    SwitchPreprocessor::SetSupportedNamespaces({"http://www.xml-cml.org/schema"});
    size_t outLen = 0;
    char* input = strdup(gInput);
    char* output = reinterpret_cast<char*>(proc.FilterData(ctx.get(), input, sizeof(gInput), &outLen));
    output[outLen] = '\0';
    
    INFO("CML output:\n" << output);
    REQUIRE(strncmp(output, gCMLOnly, std::min(sizeof(gCMLOnly), outLen)) == 0);
    
    if ( output != input )
        delete [] output;
    free(input);
    
    SwitchPreprocessor::SetSupportedNamespaces({MathMLNamespaceURI});
    ctx.reset(proc.MakeFilterContext(nullptr));
    input = strdup(gInput);
    output = reinterpret_cast<char*>(proc.FilterData(ctx.get(), input, sizeof(gInput), &outLen));
    output[outLen] = '\0';
    
    INFO("MathML output:\n" << output);
    REQUIRE(strncmp(output, gMathMLOnly, std::min(sizeof(gMathMLOnly), outLen)) == 0);
    
    if ( output != input )
        delete [] output;
    free(input);
    
    SwitchPreprocessor::SetSupportedNamespaces({});
}

TEST_CASE("Processors should be able to support multiple supported namespaces", "")
{
    SwitchPreprocessor proc;
    std::unique_ptr<FilterContext> ctx(proc.MakeFilterContext(nullptr));
    
    SwitchPreprocessor::SetSupportedNamespaces({"http://www.xml-cml.org/schema", MathMLNamespaceURI});
    
    size_t outLen = 0;
    char* input = strdup(gInput);
    char* output = reinterpret_cast<char*>(proc.FilterData(ctx.get(), input, sizeof(gInput), &outLen));
    output[outLen] = '\0';
    
    INFO("Output:\n" << output);
    REQUIRE(strncmp(output, gCMLAndMathML, std::min(sizeof(gCMLAndMathML), outLen)) == 0);
    
    if ( output != input )
        delete [] output;
    free(input);
    
    SwitchPreprocessor::SetSupportedNamespaces({});
}

TEST_CASE("Processors should gracefully handle comments around non-default switched content", "")
{
    SwitchPreprocessor proc;
    std::unique_ptr<FilterContext> ctx(proc.MakeFilterContext(nullptr));
    
    SwitchPreprocessor::SetSupportedNamespaces({});
    
    size_t outLen = 0;
    char* input = strdup(gCommentedInput);
    char* output = reinterpret_cast<char*>(proc.FilterData(ctx.get(), input, sizeof(gCommentedInput), &outLen));
    output[outLen] = '\0';
    
    INFO("Output:\n" << output);
    REQUIRE(strncmp(output, gCommentedDefaultOutput, std::min(sizeof(gCommentedDefaultOutput), outLen)) == 0);
    
    if ( output != input )
        delete [] output;
    free(input);
    
    SwitchPreprocessor::SetSupportedNamespaces({});
}

TEST_CASE("Processors should NOT uncomment switch constructs which have been commented out in their entirety", "Note that the switches themselves will still be processed.")
{
    SwitchPreprocessor proc;
    std::unique_ptr<FilterContext> ctx(proc.MakeFilterContext(nullptr));
    
    size_t outLen = 0;
    char* input = strdup(gTotallyCommentedInput);
    char* output = reinterpret_cast<char*>(proc.FilterData(ctx.get(), input, sizeof(gTotallyCommentedInput), &outLen));
    output[outLen] = '\0';
    
    INFO("Output:\n" << output);
    REQUIRE(strncmp(output, gTotallyCommentedOutput, std::min(sizeof(gTotallyCommentedOutput), outLen)) == 0);
    
    if ( output != input )
        delete [] output;
    free(input);
    
    SwitchPreprocessor::SetSupportedNamespaces({});
}
