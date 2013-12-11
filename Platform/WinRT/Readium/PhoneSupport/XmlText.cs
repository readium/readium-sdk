//
//  XmlText.cs
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

namespace ReadiumPhoneSupport
{
    [ComVisibleAttribute(true)]
    public class XmlText : IXmlText
    {
        internal XText _base;

        internal XmlText(XText linq)
        {
            _base = linq;
        }

        /// <summary>
        /// Splits this text node into two text nodes at the specified offset and inserts
        /// the new text node into the tree as a sibling that immediately follows this node.
        /// </summary>
        /// <param name="offset">The number of characters at which to split this text node
        /// into two nodes, starting from zero.</param>
        /// <returns>The new text node.</returns>
        public IXmlText SplitText(uint offset)
        {
            string newText = _base.Value.Substring((int)offset);
            _base.Value = _base.Value.Substring(0, (int)offset);
            XText newNode = new XText(newText);
            _base.AddAfterSelf(newNode);
            return new XmlText(newNode);
        }

        /// <summary>
        /// Appends the supplied string to the existing string data.
        /// </summary>
        /// <param name="data">The data to be appended to the existing string.</param>
        public void AppendData(string data)
        {
            _base.Value = _base.Value + data;
        }

        /// <summary>
        /// Deletes specified data.
        /// </summary>
        /// <param name="offset">The offset, in characters, at which to start deleting the string data.</param>
        /// <param name="count">The number of characters to delete.</param>
        public void DeleteData(uint offset, uint count)
        {
            string str = _base.Value;
            _base.Value = str.Remove((int)offset, (int)count);
        }

        public void InsertData(uint offset, string data)
        {
            string str = _base.Value;
            _base.Value = str.Insert((int)offset, data);
        }

        public void ReplaceData(uint offset, uint count, string data)
        {
            string str = _base.Value;
            str = str.Remove((int)offset, (int)count);
            _base.Value = str.Insert((int)offset, data);
        }

        public string SubstringData(uint offset, uint count)
        {
            return _base.Value.Substring((int)offset, (int)count);
        }

        public string Data
        {
            get
            {
                return _base.Value;
            }
            set
            {
                _base.Value = value;
            }
        }

        public uint Length
        {
            get { return (uint)_base.Value.Length; }
        }

        public IXmlNode AppendChild(IXmlNode newNode)
        {
            throw new InvalidOperationException();
        }

        public IXmlNode CloneNode(bool deep)
        {
            return NodeConversion.ConvertNode(new XText(_base.Value));
        }

        public bool HasChildNodes()
        {
            return false;
        }

        public IXmlNode InsertBefore(IXmlNode newChild, IXmlNode referenceNode)
        {
            throw new InvalidOperationException();
        }

        public void Normalize()
        {
            // no-op
        }

        public IXmlNode RemoveChild(IXmlNode childNode)
        {
            throw new InvalidOperationException();
        }

        public IXmlNode ReplaceChild(IXmlNode newChild, IXmlNode referenceChild)
        {
            throw new InvalidOperationException();
        }

        public XmlDocument OwnerDocument
        {
            get { return NodeConversion.ConvertNode(_base.Document) as XmlDocument; }
        }

        public IXmlNode ParentNode
        {
            get { return NodeConversion.ConvertNode(_base.Parent); }
        }

        public XmlNamedNodeMap Attributes
        {
            get { throw new InvalidOperationException(); }
        }

        public XmlNodeList ChildNodes
        {
            get { throw new InvalidOperationException(); }
        }

        public IXmlNode FirstChild
        {
            get { throw new InvalidOperationException(); }
        }

        public IXmlNode LastChild
        {
            get { throw new InvalidOperationException(); }
        }

        public object LocalName
        {
            get { return null; }
        }

        public string Prefix
        {
            get
            {
                return null;
            }
            set
            {
                throw new InvalidOperationException();
            }
        }

        public object NamespaceUri
        {
            get { return null; }
        }

        public string NodeName
        {
            get { return "TEXT_NODE"; }
        }

        public NodeType NodeType
        {
            get { return NodeType.TextNode; }
        }

        public object NodeValue
        {
            get
            {
                return _base.Value;
            }
            set
            {
                _base.Value = value.ToString();
            }
        }

        public IXmlNode NextSibling
        {
            get { return NodeConversion.ConvertNode(_base.NextNode); }
        }

        public IXmlNode PreviousSibling
        {
            get { return NodeConversion.ConvertNode(_base.PreviousNode); }
        }

        public IXmlNodeList SelectNodes(string xpath)
        {
            return SelectNodesNS(xpath, null);
        }

        public IXmlNodeList SelectNodesNS(string xpath, object namespaces)
        {
            if (xpath != ".")
                return null;

            var list = new List<XObject>(1);
            list.Add(_base);
            return new XmlNodeList(list);
        }

        public IXmlNode SelectSingleNode(string xpath)
        {
            return SelectSingleNodeNS(xpath, null);
        }

        public IXmlNode SelectSingleNodeNS(string xpath, object namespaces)
        {
            if (xpath != ".")
                return null;
            return this;
        }

        public string GetXml()
        {
            return _base.ToString();
        }

        public string InnerText
        {
            get
            {
                return _base.Value;
            }
            set
            {
                _base.Value = value;
            }
        }
    }
}