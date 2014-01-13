//
//  IXmlNodeList.cs
//  ReadiumPhoneSupport
//
//  Created by Jim Dovey on 2013-12-03.
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

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Xml;
using System.Xml.Linq;
using PhoneSupportInterfaces;

namespace ReadiumPhoneSupport
{
    [ComVisibleAttribute(true)]
    public class XmlNodeList : IXmlNodeList
    {
        private List<XObject> _nodes;

        internal XmlNodeList(IEnumerable<XObject> nodes)
        {
            _nodes = new List<XObject>(nodes);
        }

        public IXmlNode Item(uint index)
        {
            return this[(int)index];
        }

        public uint Length
        {
            get { return (uint)this.Count; }
        }
        
        [System.Runtime.CompilerServices.IndexerName("Items")]
        public IXmlNode this[int index]
        {
            get { return NodeConversion.ConvertNode(_nodes[index]); }
        }
        
        public int Count
        {
            get { return _nodes.Count; }
        }

        public IEnumerator<IXmlNode> GetEnumerator()
        {
            return new XmlNodeConversionEnumerator(_nodes.GetEnumerator());
        }

        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }
    }
}