//
//  DtdNotation.cs
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
    public class DtdNotation : IDtdNotation
    {
        internal XNode _base;

        internal DtdNotation(XNode linq)
        {
            _base = linq;
        }

        public object PublicId
        {
            get { throw new NotImplementedException(); }
        }

        public object SystemId
        {
            get { throw new NotImplementedException(); }
        }

        public IXmlNode AppendChild(IXmlNode newNode)
        {
            throw new NotImplementedException();
        }

        public IXmlNode CloneNode(bool deep)
        {
            throw new NotImplementedException();
        }

        public bool HasChildNodes()
        {
            throw new NotImplementedException();
        }

        public IXmlNode InsertBefore(IXmlNode newChild, IXmlNode referenceNode)
        {
            throw new NotImplementedException();
        }

        public void Normalize()
        {
            throw new NotImplementedException();
        }

        public IXmlNode RemoveChild(IXmlNode childNode)
        {
            throw new NotImplementedException();
        }

        public IXmlNode ReplaceChild(IXmlNode newChild, IXmlNode referenceChild)
        {
            throw new NotImplementedException();
        }

        public XmlDocument OwnerDocument
        {
            get { throw new NotImplementedException(); }
        }

        public IXmlNode ParentNode
        {
            get { throw new NotImplementedException(); }
        }

        public XmlNamedNodeMap Attributes
        {
            get { throw new NotImplementedException(); }
        }

        public XmlNodeList ChildNodes
        {
            get { throw new NotImplementedException(); }
        }

        public IXmlNode FirstChild
        {
            get { throw new NotImplementedException(); }
        }

        public IXmlNode LastChild
        {
            get { throw new NotImplementedException(); }
        }

        public object LocalName
        {
            get { throw new NotImplementedException(); }
        }

        public string Prefix
        {
            get
            {
                throw new NotImplementedException();
            }
            set
            {
                throw new NotImplementedException();
            }
        }

        public object NamespaceUri
        {
            get { throw new NotImplementedException(); }
        }

        public string NodeName
        {
            get { throw new NotImplementedException(); }
        }

        public NodeType NodeType
        {
            get { throw new NotImplementedException(); }
        }

        public object NodeValue
        {
            get
            {
                throw new NotImplementedException();
            }
            set
            {
                throw new NotImplementedException();
            }
        }

        public IXmlNode NextSibling
        {
            get { throw new NotImplementedException(); }
        }

        public IXmlNode PreviousSibling
        {
            get { throw new NotImplementedException(); }
        }

        public IXmlNodeList SelectNodes(string xpath)
        {
            throw new NotImplementedException();
        }

        public IXmlNodeList SelectNodesNS(string xpath, object namespaces)
        {
            throw new NotImplementedException();
        }

        public IXmlNode SelectSingleNode(string xpath)
        {
            throw new NotImplementedException();
        }

        public IXmlNode SelectSingleNodeNS(string xpath, object namespaces)
        {
            throw new NotImplementedException();
        }

        public string GetXml()
        {
            throw new NotImplementedException();
        }

        public string InnerText
        {
            get
            {
                throw new NotImplementedException();
            }
            set
            {
                throw new NotImplementedException();
            }
        }
    }
}