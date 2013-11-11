/*
 *  CopyAssets.js
 *
 *  Created by Diego Sandin on 2013-11-08.
 *  Copyright (c) 2012-2013 The Readium Foundation and contributors.
 *  
 *  The Readium SDK is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

var fso = new ActiveXObject("Scripting.FileSystemObject");
var srcRoot = fso.GetFolder("..\\Readium").Path;
var deployRoot = fso.GetFolder("..\\").Path;

var configurations =
    [
        ["Debug", "Debug"],
        ["Release", "Retail"]
    ]

var architectures = 
    [
        ["ARM", "arm"],
        ["Win32", "x86"],
        ["x64", "x64"]
    ]


var assets = 
[ 
    ["\deploy\\ARCHITECTURE\\CONFIGURATION\\Readium\\Readium.pdb", "DesignTime\\CONFIGURATION\\ARCHITECTURE\\"],
    ["\deploy\\ARCHITECTURE\\CONFIGURATION\\Readium\\Readium.exp", "DesignTime\\CONFIGURATION\\ARCHITECTURE\\"],
    ["\deploy\\ARCHITECTURE\\CONFIGURATION\\Readium\\Readium.lib", "DesignTime\\CONFIGURATION\\ARCHITECTURE\\"],
    ["\\deploy\\ARCHITECTURE\\CONFIGURATION\\Readium\\Readium.dll", "Redist\\CONFIGURATION\\ARCHITECTURE\\"],
    ["\\deploy\\ARCHITECTURE\\CONFIGURATION\\Readium\\Readium.winmd", "References\\CONFIGURATION\\ARCHITECTURE\\"]
]


/* Copy  Include files */
var includeDir = "DesignTime\\CommonConfiguration\\Neutral\\Include";
WScript.Echo("Copying 'Public Interface' to '" + includeDir + "'");

if (!fso.FolderExists(includeDir))
    fso.CreateFolder(includeDir);

var path = fso.BuildPath(srcRoot, "\\*.h");
try {
    fso.CopyFile(path, includeDir, true);
} catch (e) {
    WScript.Echo("Error: '" + e.description + "'  '" + e.message + "'");
}



for (var i in configurations)
{
    for (var j in architectures)
    {
        WScript.Echo("Copying '" + configurations[i] + "' files of '" + architectures[j][0] + "' architeture ");
        for (var k in assets)
        {

            var inPath = fso.BuildPath(deployRoot, assets[k][0].replace("ARCHITECTURE", architectures[j][0]).replace("CONFIGURATION", configurations[i][0]));
            var outPath = assets[k][1].replace("ARCHITECTURE", architectures[j][1]).replace("CONFIGURATION", configurations[i][1]);

            WScript.Echo("Copying '" + inPath + "' to '" + outPath + "'");

            if (!fso.FolderExists(outPath))
                fso.CreateFolder(outPath);

            try {
                fso.CopyFile(inPath, outPath, true);
            } catch (e) {
                WScript.Echo("Error: '" + e.description + "'");
            }
        }
    }
}