/*
 *  MakeHeaders.js
 *  ePub3
 *
 *  Created by Jim Dovey on 2013-04-29.
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

// paths are automatically relative to the location of this script file
var includeDir = "include";

var fso = new ActiveXObject("Scripting.FileSystemObject");
var srcRoot = fso.GetFolder("../..").Path;
var groups = {
    "utf8" : ["ePub3\\ThirdParty\\utf8-cpp\\include\\utf8.h", "ePub3\\ThirdParty\\utf8-cpp\\include\\utf8"],
    "google-url" : ["ePub3\\ThirdParty\\google-url\\base\\*.h", "ePub3\\ThirdParty\\google-url\\src\\*.h"],
    "libzip" : ["ePub3\\ThirdParty\\libzip\\*.h"],
    "ePub3" : ["ePub3\\*.h", "ePub3\\ePub\\*.h"],
    "ePub3\\xml" : ["ePub3\\xml\\utilities\\*.h", "ePub3\\xml\\tree\\*.h", "ePub3\\xml\\validation\\*.h"],
    "ePub3\\utilities" : ["ePub3\\utilities\\*.h"]
};

if (!fso.FolderExists(includeDir))
    fso.CreateFolder(includeDir);

for (var groupName in groups)
{
    WScript.Echo("Copying to '" + groupName + "'");
    
    var outFolder = fso.BuildPath(includeDir, groupName);
    if (!fso.FolderExists(outFolder))
        fso.CreateFolder(outFolder);
    outFolder += "\\";

    var inputs = groups[groupName];
    for (var i in inputs)
    {
        var path = fso.BuildPath(srcRoot, inputs[i]);
        WScript.Echo("Copying from '" + path + "'");
        try {
            fso.CopyFolder(path, outFolder, true);  // may fail if path points to files
        } catch (e) {
        }
        try {
            fso.CopyFile(path, outFolder, true);    // may fail if path points to folders
        } catch (e) {
        }
    }
}