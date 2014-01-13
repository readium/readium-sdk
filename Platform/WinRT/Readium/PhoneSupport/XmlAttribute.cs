//
//  IXmlAttribute.cs
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
    /// <summary>
    /// Represents an attribute of an XmlElement. Valid and default values for
    /// the attribute are defined in a document type definition (DTD) or schema.
    /// </summary>
    [ComVisibleAttribute(true)]
    public class XmlAttribute : IXmlAttribute
    {
        internal XAttribute _base;

        internal XmlAttribute(XAttribute linq)
        {
            _base = linq;
        }

        /// <summary>
        /// Returns the attribute name.
        /// </summary>
        public string Name
        {
            get { return _base.Name.LocalName; }
        }

        /// <summary>
        /// Gets a value that indicates whether the attribute is explicitly specified
        /// or derived from a default value in the document type definition (DTD) or schema.
        /// 
        /// In our implementation, we are unable to determine this, since Xml.Linq eats all
        /// such information.
        /// </summary>
        /// <result>Always returns true.</result>
        public bool Specified
        {
            get { return true; }
        }

        /// <summary>
        /// Gets or sets the attribute value.
        /// </summary>
        public string Value
        {
            get { return _base.Value; }
            set { _base.Value = value; }
        }

        /// <summary>
        /// This method is not applicable to this class and will throw an exception.
        /// </summary>
        /// <param name="newNode">The new child node to be appended to the end of the list of children of this node.</param>
        /// <returns>The new child node successfully appended to the list. If null, no object is created.</returns>
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
        /// <returns>The newly created clone node.</returns>
        public IXmlNode CloneNode(bool deep)
        {
            if (deep)
                return new XmlAttribute(new XAttribute(_base));
            return new XmlAttribute(_base);
        }

        /// <summary>
        /// Determines whether a node has children.
        /// </summary>
        /// <returns>Attributes never have children; always returns false.</returns>
        public bool HasChildNodes()
        {
            // attributes never have children
            return false;
        }

        /// <summary>
        /// This method is not applicable to this class and will throw an exception.
        /// </summary>
        /// <param name="newChild">The address of the new node to be inserted. The node passed
        /// here must be a valid child of the current XML DOM document node. For example, if
        /// the current node is an attribute, you cannot pass another attribute in the newChild
        /// parameter, because an attribute cannot have an attribute as a child. If newChild
        /// is a DOCUMENT_FRAGMENT node type, all its children are inserted in order before 
        /// referenceChild. If newChild is already in the tree, it is first removed before it
        /// is reinserted before the referenceChild node. Read-only nodes, such as 
        /// NODE_DOCUMENT_TYPE and NODE_ENTITY nodes, cannot be passed in the newChild 
        /// parameter.</param>
        /// <param name="referenceNode">The reference node. The node specified is where the 
        /// newChild node is to be inserted to the left as the preceding sibling in the child
        /// list. The node passed here must be a either a child node of the current node or
        /// null. If the value is null, the newChild node is inserted at the end of the child
        /// list. If the referenceChild node is not a child of the current node, an error is
        /// returned.</param>
        /// <returns>On success, the child node that was inserted. If null, no object is created.</returns>
        public IXmlNode InsertBefore(IXmlNode newChild, IXmlNode referenceNode)
        {
            throw new InvalidOperationException();
        }

        /// <summary>
        /// This method is not applicable to this class and will throw an exception.
        /// </summary>
        public void Normalize()
        {
            throw new InvalidOperationException();
        }

        /// <summary>
        /// This method is not applicable to this class and will throw an exception.
        /// </summary>
        /// <param name="childNode">The node to remove.</param>
        /// <returns>The node that was removed. If null, no changes were made.</returns>
        public IXmlNode RemoveChild(IXmlNode childNode)
        {
            throw new InvalidOperationException();
        }

        /// <summary>
        /// This method is not applicable to this class and will throw an exception.
        /// </summary>
        /// <param name="newChild">The replacement node.</param>
        /// <param name="referenceChild">The child node to be replaced.</param>
        /// <returns>The node that was replaced. If null, no changes were made.</returns>
        public IXmlNode ReplaceChild(IXmlNode newChild, IXmlNode referenceChild)
        {
            throw new InvalidOperationException();
        }

        /// <summary>
        /// Returns the root of the document that contains the node.
        /// </summary>
        public IXmlDocument OwnerDocument
        {
            get { return NodeConversion.ConvertNode(_base.Document) as XmlDocument; }
        }

        /// <summary>
        /// The element to which this attribute belongs.
        /// </summary>
        public IXmlNode ParentNode
        {
            get { return NodeConversion.ConvertNode(_base.Parent); }
        }

        /// <summary>
        /// This method is not applicable to this class and will throw an exception.
        /// </summary>
        public IXmlNamedNodeMap Attributes
        {
            get { throw new InvalidOperationException(); }
        }

        /// <summary>
        /// This method is not applicable to this class and will throw an exception.
        /// </summary>
        public IXmlNodeList ChildNodes
        {
            get { throw new InvalidOperationException(); }
        }

        /// <summary>
        /// This method is not applicable to this class and will throw an exception.
        /// </summary>
        public IXmlNode FirstChild
        {
            get { throw new InvalidOperationException(); }
        }

        /// <summary>
        /// This method is not applicable to this class and will throw an exception.
        /// </summary>
        public IXmlNode LastChild
        {
            get { throw new InvalidOperationException(); }
        }

        /// <summary>
        /// Gets the local name, which is the local part of a qualified name.
        /// This is called the local part in Namespaces in XML.
        /// </summary>
        public object LocalName
        {
            get { return _base.Name.LocalName; }
        }

        /// <summary>
        /// Gets or sets the namespace prefix.
        /// </summary>
        /// <result>The namespace prefix specified on the attribute. For example,
        /// for the attribute xxx:yyy="zzz", this property returns xxx. It returns
        /// an empty string, "", if no prefix is specified.</result>
        public string Prefix
        {
            get
            {
                XElement parent = _base.Parent;
                if (parent != null)
                    return _base.Parent.GetPrefixOfNamespace(_base.Name.Namespace);

                char[] separators = {':'};
                var parts = _base.Name.ToString().Split(separators, 2);
                if (parts.Length == 2)
                    return parts[0];
                return "";
            }
            set
            {
                // have to create a new XAttribute instance, as XAttribute.Name is read-only
                XName name = (value.Length == 0? _base.Name.LocalName : value + ":" + _base.Name.LocalName);
                
                XElement parent = _base.Parent;
                if (parent != null)
                {
                    parent.SetAttributeValue(name, _base.Value);
                    parent.SetAttributeValue(_base.Name, null);
                    _base = parent.Attribute(name);
                }
                else
                {
                    _base = new XAttribute(name, _base.Value);
                }
            }
        }

        /// <summary>
        /// Returns the Uniform Resource Identifier (URI) for the namespace.
        /// </summary>
        public object NamespaceUri
        {
            get { return _base.Name.NamespaceName; }
        }

        /// <summary>
        /// Returns the qualified name for attribute, document type, element, entity,
        /// or notation nodes. Returns a fixed string for all other node types.
        /// </summary>
        public string NodeName
        {
            get { return _base.Name.ToString(); }
        }

        /// <summary>
        /// Gets the XML Document Object Model (DOM) node type, which determines valid
        /// values and whether the node can have child nodes.
        /// </summary>
        public NodeType NodeType
        {
            get { return NodeType.AttributeNode; }
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
        /// This property is not applicable to this class and will always return null.
        /// </summary>
        public IXmlNode NextSibling
        {
            get { return null; }
        }

        /// <summary>
        /// This property is not applicable to this class and will always return null.
        /// </summary>
        public IXmlNode PreviousSibling
        {
            get { return null; }
        }

        /// <summary>
        /// This property is not applicable to this class and will always return null.
        /// </summary>
        /// <returns>NULL.</returns>
        public IXmlNodeList SelectNodes(string xpath)
        {
            return null;
        }

        /// <summary>
        /// This property is not applicable to this class and will always return null.
        /// </summary>
        /// <returns>NULL.</returns>
        public IXmlNodeList SelectNodesNS(string xpath, object namespaces)
        {
            return null;
        }

        /// <summary>
        /// This property is not applicable to this class and will always return null.
        /// </summary>
        /// <returns>NULL</returns>
        public IXmlNode SelectSingleNode(string xpath)
        {
            return null;
        }

        /// <summary>
        /// This property is not applicable to this class and will always return null.
        /// </summary>
        /// <returns>NULL</returns>
        public IXmlNode SelectSingleNodeNS(string xpath, object namespaces)
        {
            return null;
        }

        /// <summary>
        /// Returns the XML representation of the node and all its descendants.
        /// </summary>
        /// <returns>The XML representation of the node and all its descendants.</returns>
        public string GetXml()
        {
            return _base.ToString();
        }

        /// <summary>
        /// Gets the text from inside the XML.
        /// </summary>
        public string InnerText
        {
            get { return _base.Value; }
            set { _base.Value = value; }
        }
    }
}