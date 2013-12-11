//
//  XmlDocumentType.cs
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
    public class XmlDocumentType : IXmlDocumentType
    {
        internal XDocumentType _base;

        internal XmlDocumentType(XDocumentType linq)
        {
            _base = linq;
        }

        public XmlNamedNodeMap Entities
        {
            get { throw new NotImplementedException(); }
        }

        public string Name
        {
            get { return _base.Name; }
        }

        public XmlNamedNodeMap Notations
        {
            get { throw new NotImplementedException(); }
        }

        public IXmlNode AppendChild(IXmlNode newNode)
        {
            throw new InvalidOperationException();
        }

        public IXmlNode CloneNode(bool deep)
        {
            throw new InvalidOperationException();
        }

        public bool HasChildNodes()
        {
            throw new InvalidOperationException();
        }

        public IXmlNode InsertBefore(IXmlNode newChild, IXmlNode referenceNode)
        {
            throw new InvalidOperationException();
        }

        public void Normalize()
        {
            throw new InvalidOperationException();
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
            get { throw new InvalidOperationException(); }
        }

        public IXmlNode ParentNode
        {
            get { throw new InvalidOperationException(); }
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
            get { return Name; }
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
            get { return Name; }
        }

        public NodeType NodeType
        {
            get { return NodeType.DocumentTypeNode; }
        }

        public object NodeValue
        {
            get
            {
                return _base.InternalSubset;
            }
            set
            {
                throw new InvalidOperationException();
            }
        }

        public IXmlNode NextSibling
        {
            get { throw new InvalidOperationException(); }
        }

        public IXmlNode PreviousSibling
        {
            get { throw new InvalidOperationException(); }
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
            return _base.ToString(SaveOptions.OmitDuplicateNamespaces);
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