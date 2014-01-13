//
//  IXmlText.cs
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
    public class XmlEntityReference : IXmlEntityReference
    {
        internal string _name;
        internal XText _base;

        internal XmlEntityReference(string name, XText linq)
        {
            _name = name;
            _base = linq;
        }

        public IXmlNode AppendChild(IXmlNode newNode)
        {
            throw new InvalidOperationException();
        }

        public IXmlNode CloneNode(bool deep)
        {
            XText newText = new XText(_base.Value);
            return new XmlEntityReference(_name, newText);
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

        public IXmlDocument OwnerDocument
        {
            get { return NodeConversion.ConvertNode(_base.Document) as XmlDocument; }
        }

        public IXmlNode ParentNode
        {
            get { return NodeConversion.ConvertNode(_base.Parent); }
        }

        public IXmlNamedNodeMap Attributes
        {
            get { throw new InvalidOperationException(); }
        }

        public IXmlNodeList ChildNodes
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
            get { return _name; }
        }

        public string Prefix
        {
            get
            {
                throw new InvalidOperationException();
            }
            set
            {
                throw new InvalidOperationException();
            }
        }

        public object NamespaceUri
        {
            get { throw new InvalidOperationException(); }
        }

        public string NodeName
        {
            get { return _name; }
        }

        public NodeType NodeType
        {
            get { return NodeType.EntityReferenceNode; }
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
            throw new InvalidOperationException();
        }

        public IXmlNodeList SelectNodesNS(string xpath, object namespaces)
        {
            throw new InvalidOperationException();
        }

        public IXmlNode SelectSingleNode(string xpath)
        {
            throw new InvalidOperationException();
        }

        public IXmlNode SelectSingleNodeNS(string xpath, object namespaces)
        {
            throw new InvalidOperationException();
        }

        public string GetXml()
        {
            return "&" + _name + ";";
        }

        public string InnerText
        {
            get
            {
                throw new InvalidOperationException();
            }
            set
            {
                throw new InvalidOperationException();
            }
        }
    }
}