//
//  Extensions.cs
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
    internal class NodeConversion
    {
        static internal IXmlNode ConvertNode(XObject curNode)
        {
            if (curNode == null)
                return null;

            IXmlNode ret = curNode.Annotation<IXmlNode>();
            if (ret != null)
                return ret;

            switch (curNode.NodeType)
            {
                case XmlNodeType.Attribute:
                    ret = new XmlAttribute(curNode as XAttribute);
                    break;
                case XmlNodeType.CDATA:
                    ret = new XmlCDataSection(curNode as XCData);
                    break;
                case XmlNodeType.Comment:
                    ret = new XmlComment(curNode as XComment);
                    break;
                case XmlNodeType.Document:
                    ret = new XmlDocument(curNode as XDocument);
                    break;
                case XmlNodeType.DocumentType:
                    ret =new XmlDocumentType(curNode as XDocumentType);
                    break;
                case XmlNodeType.Element:
                    ret = new XmlElement(curNode as XElement);
                    break;
                case XmlNodeType.Entity:
                    ret = new DtdEntity(curNode as XNode);
                    break;
                case XmlNodeType.EntityReference:
                    ret = new XmlEntityReference(curNode.GetNodeName().ToString(), curNode as XText);
                    break;
                case XmlNodeType.Notation:
                    ret = new DtdNotation(curNode as XNode);
                    break;
                case XmlNodeType.ProcessingInstruction:
                    ret = new XmlProcessingInstruction(curNode as XProcessingInstruction);
                    break;
                case XmlNodeType.SignificantWhitespace:
                case XmlNodeType.Text:
                case XmlNodeType.Whitespace:
                    ret = new XmlText(curNode as XText);
                    break;

                default:
                    break;
            }

            if (ret != null)
            {
                curNode.AddAnnotation(ret);
            }

            return ret;
        }

        static internal XmlDocumentFragment CreateDocumentFragment()
        {
            XmlDocumentFragment ret = new XmlDocumentFragment(new XElement("DOCUMENT_FRAGMENT"));
            ret._base.AddAnnotation(ret);
            return ret;
        }

        static internal XmlEntityReference CreateEntityReference(string name, XText linq)
        {
            XmlEntityReference ret = new XmlEntityReference(name, linq);
            ret._base.AddAnnotation(ret);
            return ret;
        }
    }

    internal class XmlNodeConversionEnumerator : IEnumerator<IXmlNode>
    {
        private IEnumerator<XObject> _intern;

        internal XmlNodeConversionEnumerator(IEnumerator<XObject> intern)
        {
            _intern = intern;
        }

        public IXmlNode Current
        {
            get
            {
                return NodeConversion.ConvertNode(_intern.Current);
            }
        }

        object System.Collections.IEnumerator.Current
        {
            get { return Current; }
        }

        public bool MoveNext()
        {
            return _intern.MoveNext();
        }

        public void Reset()
        {
            _intern.Reset();
        }

        public void Dispose()
        { }
    }

    public static class ReadiumPhoneSupportExtensions
    {
        public static XmlNamespaceManager CreateNamespaceManager(this XDocument doc)
        {
            XmlNamespaceManager nsmgr = doc.Annotation<XmlNamespaceManager>();
            if (nsmgr != null)
                return nsmgr;

            nsmgr = new XmlNamespaceManager(doc.CreateReader().NameTable);
            var allNSAttrs = from attr in doc.Descendants().Attributes()
                             where attr.IsNamespaceDeclaration
                             select attr;

            foreach (XAttribute attr in allNSAttrs)
            {
                nsmgr.AddNamespace(attr.Name.LocalName, attr.Value);
            }

            doc.AddAnnotation(nsmgr);
            return nsmgr;
        }

        public static XName GetNodeName(this XObject node)
        {
            switch (node.NodeType)
            {
                case XmlNodeType.Attribute:
                    return (node as XAttribute).Name;
                case XmlNodeType.DocumentType:
                    return (node as XDocumentType).Name;
                case XmlNodeType.Element:
                    return (node as XElement).Name;
                default:
                    break;
            }
            return null;
        }

        internal static XObject GetLinqObject(this IXmlNode node)
        {
            switch (node.NodeType)
            {
                case NodeType.AttributeNode:
                    return (node as XmlAttribute)._base;
                case NodeType.CDATANode:
                    return (node as XmlCDataSection)._base;
                case NodeType.CommentNode:
                    return (node as XmlComment)._base;
                case NodeType.DocumentFragmentNode:
                    return (node as XmlDocumentFragment)._base;
                case NodeType.DocumentNode:
                    return (node as XmlDocument)._base;
                case NodeType.DocumentTypeNode:
                    return (node as XmlDocumentType)._base;
                case NodeType.ElementNode:
                    return (node as XmlElement)._base;
                case NodeType.EntityNode:
                    return (node as DtdEntity)._base;
                case NodeType.EntityReferenceNode:
                    return (node as XmlEntityReference)._base;
                case NodeType.NotationNode:
                    return (node as DtdNotation)._base;
                case NodeType.ProcessingInstructionNode:
                    return (node as XmlProcessingInstruction)._base;
                case NodeType.SignificantWhitespaceNode:
                case NodeType.TextNode:
                case NodeType.WhitespaceNode:
                    return (node as XmlText)._base;
                default:
                    break;
            }

            return null;
        }

        public static bool CompareChildValue(this XElement element, XName childName, bool childIsAttribute, string op, string value)
        {
            string myVal = (childIsAttribute ? element.Attribute(childName).Value : element.Element(childName).Value);
            if (myVal == null)
                return value == null && op == "==";

            switch (op)
            {
                case "==":
                    return myVal == value;
                case "!=":
                    return myVal != value;
                case ">":
                    return myVal.CompareTo(value) > 0;
                case "<":
                    return myVal.CompareTo(value) < 0;
                case ">=":
                    return myVal.CompareTo(value) >= 0;
                case "<=":
                    return myVal.CompareTo(value) <= 0;
                default:
                    throw new XPathException("Unrecognized operator: " + op);
            }
        }
    }
}