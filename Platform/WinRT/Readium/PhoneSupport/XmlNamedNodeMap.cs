//
//  IXmlNamedNodeMap.cs
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
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Linq;
using System.Xml;
using System.Xml.Linq;
using PhoneSupportInterfaces;

namespace ReadiumPhoneSupport
{
    internal class OrderedXNameComparer : IComparer<XName>
    {
        internal List<XName> _order;

        public OrderedXNameComparer(List<XName> order)
        {
            _order = order;
        }

        public int Compare(XName x, XName y)
        {
            return _order.IndexOf(y) - _order.IndexOf(x);
        }
    }

    [ComVisibleAttribute(true)]
    public class XmlNamedNodeMap : IXmlNamedNodeMap
    {
        internal SortedDictionary<XName, XObject> _base;

        internal class Enumerator : IEnumerator<IXmlNode>
        {
            private SortedDictionary<XName, XObject> _dict;
            private SortedDictionary<XName, XObject>.Enumerator _raw;

            internal Enumerator(SortedDictionary<XName, XObject> dict)
            {
                _dict = dict;
                _raw = _dict.GetEnumerator();
            }

            public IXmlNode Current
            {
                get { return NodeConversion.ConvertNode(_raw.Current.Value); }
            }

            object IEnumerator.Current
            {
                get { return NodeConversion.ConvertNode(_raw.Current.Value); }
            }

            public bool MoveNext()
            {
                return _raw.MoveNext();
            }

            public void Reset()
            {
                _raw = _dict.GetEnumerator();
            }

            public void Dispose()
            { }
        }

        internal XmlNamedNodeMap(IEnumerable<XObject> linq)
        {
            List<XName> orderedTitles = new List<XName>(linq.Count());
            foreach (XObject item in linq)
            {
                var name = item.GetNodeName();
                if (name == null)
                    continue;
                orderedTitles.Add(name);
            }

            _base = new SortedDictionary<XName, XObject>(new OrderedXNameComparer(orderedTitles));
            foreach (XObject item in linq)
            {
                var name = item.GetNodeName();
                if (name == null)
                    continue;
                _base[name] = item;
            }
        }

        public IXmlNode GetNamedItem(string name)
        {
            return NodeConversion.ConvertNode(_base[name]);
        }

        public IXmlNode GetNamedItemNS(object namespaceUri, string name)
        {
            XName key = XNamespace.Get(namespaceUri.ToString()).GetName(name);
            return NodeConversion.ConvertNode(_base[key]);
        }

        public IXmlNode Item(uint index)
        {
            var key = (_base.Comparer as OrderedXNameComparer)._order[(int)index];
            return NodeConversion.ConvertNode(_base[key]);
        }

        public IXmlNode RemoveNamedItem(string name)
        {
            XObject item = _base[name];
            IXmlNode ret = NodeConversion.ConvertNode(item);
            _base.Remove(name);
            return ret;
        }

        public IXmlNode RemoveNamedItemNS(object namespaceUri, string name)
        {
            XName key = XNamespace.Get(namespaceUri.ToString()).GetName(name);
            IXmlNode ret = NodeConversion.ConvertNode(_base[key]);
            _base.Remove(key);
            return ret;
        }

        public IXmlNode SetNamedItem(IXmlNode node)
        {
            XName name = node.LocalName.ToString();
            var linq = node.GetLinqObject();
            if (linq == null || name == null)
                return null;
            _base[name] = linq;
            return node;
        }

        public IXmlNode SetNamedItemNS(IXmlNode node)
        {
            XName name = node.NodeName;
            var linq = node.GetLinqObject();
            if (linq == null || name == null)
                return null;
            _base[name] = linq;
            return node;
        }

        public uint Length
        {
            get { return (uint)_base.Count; }
        }
        
        [System.Runtime.CompilerServices.IndexerName("Items")]
        public IXmlNode this[int index]
        {
            get { return Item((uint)index); }
        }
        
        public int Count
        {
            get { return _base.Count; }
        }

        public IEnumerator<IXmlNode> GetEnumerator()
        {
            return new Enumerator(_base);
        }

        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
        {
            return new Enumerator(_base);
        }
    }
}