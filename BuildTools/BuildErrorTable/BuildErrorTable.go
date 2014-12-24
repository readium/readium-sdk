// 
// BuildErrorTable.go
// Created by Jim Dovey on 2013-05-21.
// 
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//  3. Neither the name of the organization nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
//

package main

import (
	"bufio"
	"flag"
	"fmt"
	"io"
	"os"
	pathutils "path/filepath"
	"regexp"
)

var (
	projectPath = flag.String("project-path", ".", "The path to the project root folder")
	showHelp    = flag.Bool("help", false, "Display usage information.")
	verbose     = flag.Bool("verbose", false, "Display verbose logging information.")
	dryRun      = flag.Bool("dry-run", false, "Don't actually write the output, just print to stdout")

	specName    = ""
	specSection = ""
	inErrorList = false

	matchStart *regexp.Regexp = regexp.MustCompile(`^enum class EPUBError`)
	getSpec    *regexp.Regexp = regexp.MustCompile(`^\s+// (EPUBSpec::[[:alnum:]]+)`)
	getSection *regexp.Regexp = regexp.MustCompile(`^\s+// § ([\d\.]+)`)
	getInfo    *regexp.Regexp = regexp.MustCompile(`^\s+([[:alnum:]]+),\s*///\<\s*(.*)\s+(Minor|Medium|Major|Critical).\n`)
	matchEnd   *regexp.Regexp = regexp.MustCompile(`^\};`)
)

type ErrorInfo struct {
	name     string
	spec     string
	section  string
	message  string
	severity string
}

func writeHeader(output *bufio.Writer) {
	if *verbose && !*dryRun {
		fmt.Println("Writing source header")
	}
	licensePath := pathutils.Join(*projectPath, "GPL3-header.txt")
	license, err := os.Open(licensePath)
	if err != nil {
		panic(err)
	}
	defer func() {
		if err := license.Close(); err != nil {
			panic(err)
		}
	}()

	reader := bufio.NewReader(license)
	for {
		if *verbose {
			fmt.Printf("Got line: '%s'\n")
		}
		line, err := reader.ReadString('\n')
		if err != nil {
			if err != io.EOF {
				panic(err)
			}
		}
		fmt.Fprintf(output, "// %s", line)
		if err == io.EOF {
			break
		}
	}

	output.WriteString("\n//\n")
	output.Flush()
}

func main() {
	flag.Parse()
	if *projectPath == "" {
		fmt.Println("No project root specified.")
		flag.PrintDefaults()
		return
	}
	if *showHelp {
		flag.PrintDefaults()
		return
	}

	var infoList []*ErrorInfo = nil
	inputPath := pathutils.Join(*projectPath, "ePub3", "utilities", "error_handler.h")
	outputPath := pathutils.Join(*projectPath, "ePub3", "utilities", "error_lookup_table.cpp")

	if *verbose {
		fmt.Printf("Input: %s\nOutput: %s\n", inputPath, outputPath)
	}

	inFile, err := os.Open(inputPath)
	if err != nil {
		panic(err)
	}
	defer func() {
		if err := inFile.Close(); err != nil {
			panic(err)
		}
	}()

	input := bufio.NewReader(inFile)
	for {
		line, err := input.ReadString('\n')
		if err != nil && err != io.EOF {
			panic(err)
		}

		if !inErrorList {
			if matchStart.MatchString(line) {
				if *verbose {
					fmt.Println("Found start of error enum:")
					fmt.Println(line)
				}
				inErrorList = true
			}
			continue
		} else if matchEnd.MatchString(line) {
			if *verbose {
				fmt.Println("Found end of error enum:")
				fmt.Println(line)
			}
			// all done!
			break
		}

		if matches := getSpec.FindStringSubmatch(line); matches != nil {
			// line contains a spec variable name
			specName = matches[1]
			if *verbose {
				fmt.Printf("Found spec variable name '%s':\n", specName)
				fmt.Println(line)
			}
			continue
		}
		if matches := getSection.FindStringSubmatch(line); matches != nil {
			// line containes a section number
			specSection = matches[1]
			if *verbose {
				fmt.Printf("Found spec section number '%s':\n", specSection)
				fmt.Println(line)
			}
			continue
		}
		if matches := getInfo.FindStringSubmatch(line); matches != nil {
            severity := "ViolationSeverity::" + matches[3]
			infoList = append(infoList, &ErrorInfo{matches[1], specName, specSection, matches[2], severity})
			if *verbose {
				fmt.Printf("Found error info: %s / '%s' (%s)\n", matches[1], matches[2], severity)
				fmt.Println(line)
			}
		}
	}

	if len(infoList) == 0 {
		fmt.Println("No error information found ???")
		return
	}

	var output *bufio.Writer

	if *dryRun {
		output = bufio.NewWriter(os.Stdout)
	} else {
		outFile, err := os.Create(outputPath)
		if err != nil {
			panic(err)
		}
		defer func() {
			if err := outFile.Close(); err != nil {
				panic(err)
			}
		}()

		output = bufio.NewWriter(outFile)
	}

	// write the header (will be modified/licensed later)
	writeHeader(output)
    
    output.WriteString(`
// Automatically generated by BuildErrorTable.
// DO NOT MODIFY

`)
    
    output.WriteString(`
struct _LIBCPP_HIDDEN ErrorInfo
{
    FORCE_INLINE
    ErrorInfo(ViolationSeverity __v, EPUBSpec __s, std::string __sect, std::string __msg) _NOEXCEPT : _severity(__v), _spec(__s), _section(__sect), _message(__msg) {}
    FORCE_INLINE
    ErrorInfo(const ErrorInfo& __o) : _severity(__o._severity), _spec(__o._spec), _section(__o._section), _message(__o._message) {}
    FORCE_INLINE
    ErrorInfo(ErrorInfo&& __o) : _severity(__o._severity), _spec(__o._spec), _section(std::move(__o._section)), _message(std::move(__o._message)) {}
    FORCE_INLINE
    ~ErrorInfo() {}
    
    ErrorInfo&          operator=(const ErrorInfo& __o)
    {
        _severity = __o._severity;
        _spec = __o._spec;
        _section = __o._section;
        _message = __o._message;
        return *this;
    }
    ErrorInfo&          operator=(ErrorInfo&& __o)
    {
        _severity = __o._severity;
        _spec = __o._spec;
        _section = std::move(__o._section);
        _message = std::move(__o._message);
        return *this;
    }

private:
    ErrorInfo()                             _DELETED_;

public:
    FORCE_INLINE
    ViolationSeverity   Severity() const    _NOEXCEPT   { return _severity; }
    FORCE_INLINE
    EPUBSpec            Spec() const        _NOEXCEPT   { return _spec; }
    FORCE_INLINE
    const std::string&  Section() const     _NOEXCEPT   { return _section; }
    FORCE_INLINE
    const std::string&  Message() const     _NOEXCEPT   { return _message; }

private:
    ViolationSeverity   _severity;
    EPUBSpec            _spec;
    std::string         _section;
    std::string         _message;

};

typedef std::map<EPUBError, ErrorInfo>                                     ErrorLookup;

`)

	output.WriteString("#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)\n\n")
	output.WriteString("static const ErrorLookup gErrorLookupTable = {\n")
	output.Flush()

	for i, info := range infoList {
		if _, err := fmt.Fprintf(output, "    {EPUBError::%s, {%s, %s, \"%s\", \"%s\"}}", info.name, info.severity, info.spec, info.section, info.message); err != nil {
			panic(err)
		}
		if i == len(infoList)-1 {
			output.WriteString("\n")
		} else {
			output.WriteString(",\n")
		}
		if err := output.Flush(); err != nil {
			panic(err)
		}
	}

	output.WriteString("};\n")
	output.WriteString("\n#else   // !EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)\n\n")
	output.WriteString("static ErrorLookup gErrorLookupTable;\n\n")
	output.WriteString("INITIALIZER(__initErrorTables)\n{\n")
	output.Flush()

	for _, info := range infoList {
		if _, err := fmt.Fprintf(output, "    gErrorLookupTable[EPUBError::%s] = ErrorInfo(%s, %s, \"%s\", \"%s\");\n", info.name, info.severity, info.spec, info.section, info.message); err != nil {
			panic(err)
		}
		output.Flush()
	}

	output.WriteString("}\n")
	output.WriteString("\n#endif  // !EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)\n")
	output.Flush()
}
