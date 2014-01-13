//
//  IXmlElement.cs
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
using Windows.Foundation;
using Windows.Foundation.Metadata;
using Windows.Storage;
using PhoneSupportInterfaces;

namespace ReadiumPhoneSupport
{
    [ComVisibleAttribute(true)]
    public class XmlProcessingInstruction : IXmlProcessingInstruction
    {
        internal XProcessingInstruction _base;

        internal XmlProcessingInstruction(XProcessingInstruction linq)
        {
            _base = linq;
        }

        public string Data
        {
            get
            {
                return _base.Data;
            }
            set
            {
                _base.Data = value;
            }
        }

        public string Target
        {
            get { return _base.Target; }
        }

        public IXmlNode AppendChild(IXmlNode newNode)
        {
            throw new InvalidOperationException();
        }

        public IXmlNode CloneNode(bool deep)
        {
            return new XmlProcessingInstruction(new XProcessingInstruction(_base));
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
            get { return "PROCESSING_INSTRUCTION_NODE"; }
        }

        public NodeType NodeType
        {
            get { return NodeType.ProcessingInstructionNode; }
        }

        public object NodeValue
        {
            get
            {
                return _base.Data;
            }
            set
            {
                _base.Data = value.ToString();
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
                return null;
            }
            set
            {
                throw new InvalidOperationException();
            }
        }
    }
}