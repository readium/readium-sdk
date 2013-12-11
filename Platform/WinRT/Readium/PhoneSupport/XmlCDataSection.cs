//
//  XmlCDataSection.cs
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
    public class XmlCDataSection : IXmlCDataSection
    {
        internal XCData _base;

        internal XmlCDataSection(XCData linq)
        {
            _base = linq;
        }

        /// <summary>
        /// Splits this text node into two text nodes at the specified offset
        /// and inserts the new text node into the tree as a sibling that
        /// immediately follows this node.
        /// </summary>
        /// <param name="offset">The number of characters at which to split this
        /// text node into two nodes, starting from zero.</param>
        /// <returns>The new text node.</returns>
        public IXmlText SplitText(uint offset)
        {
            string str = _base.Value;
            if (offset >= str.Length)
                return null;

            string left = str.Substring(0, (int)offset);
            string right = str.Substring((int)offset);

            _base.Value = left;

            XCData newNode = new XCData(right);
            _base.AddAfterSelf(newNode);
            return NodeConversion.ConvertNode(newNode) as IXmlText;
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

        /// <summary>
        /// Inserts a string at the specified offset.
        /// </summary>
        /// <param name="offset">The offset, in characters, at which to insert the supplied string data.</param>
        /// <param name="data">The data to be inserted into the existing string.</param>
        public void InsertData(uint offset, string data)
        {
            string str = _base.Value;
            _base.Value = str.Insert((int)offset, data);
        }

        /// <summary>
        /// Replaces the specified number of characters with the supplied string.
        /// </summary>
        /// <param name="offset">The offset, in characters, at which to start replacing string data.</param>
        /// <param name="count">The number of characters to replace.</param>
        /// <param name="data">The new data that replaces the old string data.</param>
        public void ReplaceData(uint offset, uint count, string data)
        {
            string str = _base.Value;
            if (offset >= str.Length || offset+count > str.Length)
                throw new IndexOutOfRangeException();

            if (offset == 0 && count >= str.Length)
            {
                _base.Value = data;
            }
            else if (offset == 0)
            {
                _base.Value = data + str.Substring((int)count);
            }
            else
            {
                string left = str.Substring(0, (int)offset);
                string right = str.Substring((int)(offset + count));
                _base.Value = left + data + right;
            }
        }

        /// <summary>
        /// Retrieves a substring of the full string from the specified range.
        /// </summary>
        /// <param name="offset">Specifies the offset, in characters, from the beginning of the string.
        /// An offset of zero indicates copying from the start of the data.</param>
        /// <param name="count">Specifies the number of characters to retrieve from the specified offset.</param>
        /// <returns>The returned substring.</returns>
        public string SubstringData(uint offset, uint count)
        {
            return _base.Value.Substring((int)offset, (int)count);
        }

        /// <summary>
        /// Gets or sets the node data depending on the node type.
        /// </summary>
        public string Data
        {
            get { return _base.Value; }
            set { _base.Value = value; }
        }

        /// <summary>
        /// Gets the length of the data, in Unicode characters.
        /// </summary>
        public uint Length
        {
            get { return (uint)_base.Value.Length; }
        }

        /// <summary>
        /// This method is not applicable to this class. The CDataSection does not have children.
        /// This method will throw an exception.
        /// </summary>
        public IXmlNode AppendChild(IXmlNode newNode)
        {
            throw new InvalidOperationException();
        }

        /// <summary>
        /// Clones a new node.
        /// </summary>
        /// <param name="deep">A flag that indicates whether to recursively clone all nodes
        /// that are descendants of this node. If true, this method creates a clone of the
        /// complete tree below this node. If false, this method clones this node and its
        /// attributes only.</param>
        /// <returns>The cloned node.</returns>
        public IXmlNode CloneNode(bool deep)
        {
            if (deep)
                return NodeConversion.ConvertNode(new XCData(_base));
            return NodeConversion.ConvertNode(new XCData(_base.Value));
        }

        /// <summary>
        /// Determines whether a node has children. The CDataSection does not have children.
        /// This method always returns false.
        /// </summary>
        /// <returns>False</returns>
        public bool HasChildNodes()
        {
            return false;
        }

        /// <summary>
        /// Inserts a child node to the left of the specified node, or at the end of the child node list.
        /// This method is not applicable to this class and will throw an exception.
        /// </summary>
        public IXmlNode InsertBefore(IXmlNode newChild, IXmlNode referenceNode)
        {
            throw new InvalidOperationException();
        }

        /// <summary>
        /// Normalizes all descendant elements by combining two or more adjacent text nodes
        /// into one unified text node.
        /// </summary>
        public void Normalize()
        {
            throw new InvalidOperationException();
        }

        /// <summary>
        /// This method is not applicable to this class. The CDataSection does not have children.
        /// This method will throw an exception.
        /// </summary>
        public IXmlNode RemoveChild(IXmlNode childNode)
        {
            throw new InvalidOperationException();
        }

        /// <summary>
        /// This method is not applicable to this class. The CDataSection does not have children.
        /// This method will throw an exception.
        /// </summary>
        public IXmlNode ReplaceChild(IXmlNode newChild, IXmlNode referenceChild)
        {
            throw new InvalidOperationException();
        }

        /// <summary>
        /// Returns the root of the document that contains the node.
        /// </summary>
        public XmlDocument OwnerDocument
        {
            get { return NodeConversion.ConvertNode(_base.Document) as XmlDocument; }
        }

        /// <summary>
        /// Gets the parent node of the node instance.
        /// </summary>
        public IXmlNode ParentNode
        {
            get { return NodeConversion.ConvertNode(_base.Parent); }
        }

        /// <summary>
        /// Gets the list of attributes of this node.
        /// </summary>
        public XmlNamedNodeMap Attributes
        {
            get { return null; }
        }

        /// <summary>
        /// Gets a list of children in the current node. This property always returns NULL.
        /// </summary>
        public XmlNodeList ChildNodes
        {
            get { return null; }
        }

        /// <summary>
        /// Gets the first child node. This property always returns NULL.
        /// </summary>
        public IXmlNode FirstChild
        {
            get { return null; }
        }

        /// <summary>
        /// Gets the last child node. This property always returns NULL.
        /// </summary>
        public IXmlNode LastChild
        {
            get { return null; }
        }

        /// <summary>
        /// This method is not applicable to this class. The CDataSection does not have children.
        /// This method will throw an exception.
        /// </summary>
        public object LocalName
        {
            get { throw new InvalidOperationException(); }
        }

        /// <summary>
        /// This method is not applicable to this class. The CDataSection does not have children.
        /// This method will throw an exception.
        /// </summary>
        public string Prefix
        {
            get { throw new InvalidOperationException(); }
            set { throw new InvalidOperationException(); }
        }

        /// <summary>
        /// This method is not applicable to this class. The CDataSection does not have children.
        /// This method will throw an exception.
        /// </summary>
        public object NamespaceUri
        {
            get { throw new InvalidOperationException(); }
        }

        /// <summary>
        /// Returns the qualified name for attribute, document type, element, entity, or notation nodes.
        /// Returns a fixed string for all other node types.
        /// </summary>
        public string NodeName
        {
            get { return "CDATA_NODE"; }
        }

        /// <summary>
        /// Gets the XML Document Object Model (DOM) node type, which determines valid values and whether
        /// the node can have child nodes.
        /// </summary>
        public NodeType NodeType
        {
            get { return NodeType.CDATANode; }
        }

        /// <summary>
        /// Gets or sets the text associated with the node.
        /// </summary>
        public object NodeValue
        {
            get { return _base.Value; }
            set { _base.Value = value.ToString(); }
        }

        /// <summary>
        /// Gets the next sibling of the node in the parent's child list.
        /// </summary>
        public IXmlNode NextSibling
        {
            get { return NodeConversion.ConvertNode(_base.NextNode); }
        }

        /// <summary>
        /// Gets the previous sibling of the node in the parent's child list.
        /// </summary>
        public IXmlNode PreviousSibling
        {
            get { return NodeConversion.ConvertNode(_base.PreviousNode); }
        }

        /// <summary>
        /// Applies the specified pattern-matching operation to this node's context
        /// and returns the list of matching nodes as an XmlNodeList. This method is
        /// not applicable to this class and will throw an exception.
        /// </summary>
        public IXmlNodeList SelectNodes(string xpath)
        {
            throw new InvalidOperationException();
        }

        /// <summary>
        /// Applies the specified pattern-matching operation to this node's context
        /// and returns the list of matching nodes as an XmlNodeList. This method is
        /// not applicable to this class and will throw an exception.
        /// </summary>
        public IXmlNodeList SelectNodesNS(string xpath, object namespaces)
        {
            throw new InvalidOperationException();
        }

        /// <summary>
        /// Applies the specified pattern-matching operation to this node's context
        /// and returns the list of matching nodes as an XmlNodeList. This method is
        /// not applicable to this class and will throw an exception.
        /// </summary>
        public IXmlNode SelectSingleNode(string xpath)
        {
            throw new InvalidOperationException();
        }

        /// <summary>
        /// Applies the specified pattern-matching operation to this node's context
        /// and returns the list of matching nodes as an XmlNodeList. This method is
        /// not applicable to this class and will throw an exception.
        /// </summary>
        public IXmlNode SelectSingleNodeNS(string xpath, object namespaces)
        {
            throw new InvalidOperationException();
        }

        /// <summary>
        /// Returns the XML representation of the node and all its descendants.
        /// </summary>
        /// <returns>The XML representation of the node and all its descendants.</returns>
        public string GetXml()
        {
            return _base.ToString();
        }

        public string InnerText
        {
            get { return _base.Value; }
            set { _base.Value = value; }
        }
    }
}