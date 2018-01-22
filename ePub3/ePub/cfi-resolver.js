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

// Trying to namespace this, but I'm not a fluent JS coder, so I may be getting it wrong
// My theory is that within this anonymous function:
//    "someFn = blah" is inaccessible from the outside
//	  "this.someFn = blah" is accessible as window.CFIUtils.someFn

// Sample JS code to insert when opening a file which will then jump to a CFI,
// highlighting text if it's a ranged CFI:
//
//    function __ePub3_AutoLoadCFI() {
//        document.removeEventListener('DOMContentLoaded', __ePub3_AutoLoadCFI, false);
//        window.CFIUtils.locateCFI("<insert cfi here>");
//    }
//    document.addEventListener('DOMContentLoaded', __ePub3_AutoLoadCFI, false);
//
// The above would be inserted into HTML files as they're loaded, wrapped in <script>...</script>

window.CFIUtils = function() {
	Object.defineProperty(String.prototype, 'cfi_indexOf', {value: function(s,b){
	    b = typeof b !== 'undefined' ? b : 0;
	    return this.toLowerCase().indexOf(s.toLowerCase(), b);
	}});
	
	subnodeWithID = function(nodeList, nodeID) {
	    for (var i = 0; i < nodeList.length; i++)
	    {
	        if (nodeList[i].id === nodeID)
	            return nodeList[i];
	    }
    
	    return null;
	}
	
	iterateNodeList = function(node, list, idx)
	{
	    if (idx >= list.length) {
	        return node;
	    }
    
	    var nodeIdx = list[idx];
	    if (nodeIdx == '') {
	        // presumably the nonexistent string before the leading '/'
	        return iterateNodeList(node, list, idx+1);
	    }
    
	    var qualifier = null;
	    var qualifierOpen = nodeIdx.indexOf('[');
	    if (qualifierOpen != -1)
	    {
	        var qualifierClose = nodeIdx.indexOf(']', qualifierOpen+1);
	        if (qualifierClose != -1)
	            qualifier = nodeIdx.substring(qualifierOpen+1, qualifierClose);
	        nodeIdx = nodeIdx.substring(0, qualifierOpen);
	    }
    
	    // NB: node.childNodes will return all nodes of any type, while node.children will return only elements
    
	    if ((nodeIdx % 2) == 0) {
	        // look for the nodeIdx/2 element
	        nodeIdx = nodeIdx / 2;
	        var foundNode = null;
	        var j = 0;
	        var subElements = node.children;
	        if (nodeIdx <= subElements.length)
	            foundNode = subElements[nodeIdx-1];
        
	        if (foundNode == null)
	        {
	            // not found!
	            if (qualifier != null)
	                foundNode = subnodeWithID(subElements, qualifier);
            
	            if (foundNode == null)
	                return null;
	        }
	        else if (qualifier != null && foundNode.id != qualifier)
	        {
	            var idNode = subnodeWithID(subElements, qualifier);
	            if (idNode != null)
	                foundNode = idNode;
	        }
        
	        return iterateNodeList(foundNode, list, idx+1);
	    } else {
	        // referring to a text node -- this is actually a straightforward child index (1-based)
	        nodeIdx -= 1;
	        if (nodeIdx >= node.childNodes.length) {
	            return null;
	        }
        
	        return node.childNodes.item(nodeIdx);
	    }
	}
	
	splitRangedCFI = function(rangeCFI) {
	    var scanLocation = 0;
	    var componentStartLocation = 0;
	    var components = new Array();
    
	    while (scanLocation >= 0 && scanLocation <= rangeCFI.length)
	    {
	        var braceLocation = rangeCFI.indexOf('[', scanLocation);
	        var commaLocation = rangeCFI.indexOf(',', scanLocation);
        
	        if (braceLocation != -1 && braceLocation < commaLocation)
	        {
	            scanLocation = rangeCFI.indexOf(']', braceLocation+1);
	        }
	        else if (commaLocation != -1)
	        {
	            // break at this point
	            components.push(rangeCFI.substring(componentStartLocation, commaLocation));
	            componentStartLocation = commaLocation+1;
	            scanLocation = commaLocation+1;
	        }
	        else
	        {
	            // no more commas == ending the string
	            break;
	        }
	    }
    
	    if (componentStartLocation < rangeCFI.length)
	        components.push(rangeCFI.substring(componentStartLocation));
    
	    return components;
	}
	
	visitRange = function(range) {
	    var clientRect = range.getBoundingClientRect();
	    if (clientRect.top < 0)
	        window.scrollBy(0, clientRect.top - 100);
	    else if (clientRect.bottom > document.body.scrollTop + window.innerHeight)
	        window.scrollBy(0, clientRect.bottom - (document.body.scrollTop + window.innerHeight) + 100);
	}
	
	textLocationAssertion = function(terminatingStep) {
	    var qualifierIndex = terminatingStep.indexOf('[');
	    if ( qualifierIndex != -1 )
	    {
	        var qualifier = terminatingStep.substring(qualifierIndex+1, terminatingStep.length-1);
	        return qualifier.split(',');
	    }
    
	    return null;
	}
	
	adjustIndex = function(node, index, qualifiers) {
	    var adjusted = index;
    
	    // search for leading/ending text within node
	    // trailing qualifier has dominance, I'm assuming-- it's not explicitly stated in the specification
	    if (qualifiers[0].length != 0)
	    {
	        var idx = node.data.cfi_indexOf(qualifiers[0], adjusted-2-qualifiers[0].length);
	        if (idx != -1)
	        {
	            idx += qualifiers[0].length;
	            if (adjusted != idx)
	                adjusted = idx;
	        }
	    }
	    if (qualifiers.length > 1 && qualifiers[1].length != 0)
	    {
	        var idx = node.data.cfi_indexOf(qualifiers[1], adjusted-2);
	        if (idx != -1 && adjusted != idx)
	            adjusted = idx;
	    }
    
	    return adjusted;
	}
	
	makeRange = function(startNode, start, startQualifiers, endNode, end, endQualifiers) {
	    if (startQualifiers != null)
	        start = adjustIndex(startNode, start, startQualifiers);
	    if (endQualifiers != null)
	        end = adjustIndex(endNode, end, endQualifiers);
    
	    var range = document.createRange();
	    range.setStart(startNode, start);
	    range.setEnd(endNode, end);
	    return range;
	}
	
	////////////////////////////////////////////////////////////////////
	//  P U B L I C   A P I
	////////////////////////////////////////////////////////////////////
	
	this.locateCFI = function(cfi) {
	    // clean off the epubcfi(...) surround
	    if (cfi.cfi_indexOf('epubcfi(') == -1)
	        return;
    
	    cfi = cfi.substring(8,cfi.length-1);
    
	    // split into (parent,start,end) components
	    var rangeComponents = splitRangedCFI(cfi);
	    if (rangeComponents.length == 1)
	    {
	        // not a range -- so select one character, grab its client rect, and unselect
	        var nodeList = rangeComponents[0].split('/');
	        var textNode = iterateNodeList(document.body.parentElement, nodeList, 0);
	        if (textNode == null) {
	            return;
	        }
        
	        // location component
	        var last = rangeComponents[rangeComponents.length-1];
        
	        // qualifier?
	        var qualifiers = textLocationAssertion(last)
	        if (qualifiers != null)
	            last = last.substring(0, last.indexOf('['));
        
	        // character offset?
	        var start = 0, end = 1;
	        var lastElement = last.split(':');
	        if (lastElement.length == 2)
	        {
	            start = Number(lastElement[lastElement.length-1]);
	            end = start+1;
	        }
        
	        var range = makeRange(textNode, start, qualifiers, textNode, end, null);
	        visitRange(range);
	    }
	    else
	    {
	        // a range, so we have our start/end locations
	        var nodeList = rangeComponents[0].split('/');
	        var parent = iterateNodeList(document.body.parentElement, nodeList, 0);
        
	        var startComponents = rangeComponents[1].split('/');
	        var endComponents = rangeComponents[2].split('/');
        
	        var startNode = iterateNodeList(parent, startComponents, 0);
	        var endNode = iterateNodeList(parent, endComponents, 0);
        
	        var startTerminator = startComponents[startComponents.length-1];
	        var endTerminator = endComponents[endComponents.length-1];
        
	        var startQualifiers = textLocationAssertion(startTerminator);
	        if (startQualifiers != null)
	            startTerminator = startTerminator.substring(0, startTerminator.indexOf('['));
	        var endQualifiers = textLocationAssertion(endTerminator);
	        if (endQualifiers != null)
	            endTerminator = endTerminator.substring(0, endTerminator.indexOf('['));
        
	        // character offsets
	        var start = 0, end = 1;
	        var startOffsetElement = startTerminator.split(':');
	        if (startOffsetElement.length == 2)
	        {
	            start = Number(startOffsetElement[startOffsetElement.length-1]);
	            end = start+1;
	        }
        
	        var endOffsetElement = endTerminator.split(':');
	        if (endOffsetElement.length == 2)
	        {
	            end = Number(endOffsetElement[endOffsetElement.length-1]);
	        }
        
	        var range = makeRange(startNode, start, startQualifiers, endNode, end, endQualifiers);
	        visitRange(range);
        
	        // apply the selection
	        var sel = window.getSelection();
	        sel.removeAllRanges();
	        sel.addRange(range);
	    }
	}
};
