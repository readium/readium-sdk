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
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Linq;
using System.Xml;
using System.Xml.Linq;
using Windows.Foundation;
using Windows.Foundation.Metadata;
using Windows.Storage;
using PhoneSupportInterfaces;

namespace ReadiumPhoneSupport
{
    [ComVisibleAttribute(true)]
    public class XmlElement : IXmlElement
    {
        internal XElement _base;

        internal XmlElement(XElement linq)
        {
            _base = linq;
        }

        /// <summary>
        /// Returns the value of the attribute.
        /// </summary>
        /// <param name="attributeName">The name of the required attribute.</param>
        /// <returns>The string value of the attribute.</returns>
        public string GetAttribute(string attributeName)
        {
            return GetAttributeNode(attributeName).Value;
        }

        /// <summary>
        /// Returns the value of the attribute.
        /// </summary>
        /// <param name="namespaceUri">The namespace of the attribute to get.</param>
        /// <param name="attributeName">The name of the attribute without the namespace prefix.</param>
        /// <returns>The string value of the attribute.</returns>
        public string GetAttributeNS(object namespaceUri, string attributeName)
        {
            return GetAttributeNodeNS(namespaceUri, attributeName).Value;
        }

        /// <summary>
        /// Returns the attribute node.
        /// </summary>
        /// <param name="attributeName">The name of the required attribute.</param>
        /// <returns>The attribute pointer.</returns>
        public IXmlAttribute GetAttributeNode(string attributeName)
        {
            return NodeConversion.ConvertNode(_base.Attribute(attributeName)) as XmlAttribute;
        }

        /// <summary>
        /// Returns the specified attribute from the specified namespace.
        /// </summary>
        /// <param name="namespaceUri">The namespace of the attribute to get.</param>
        /// <param name="attributeName">The name of the attribute without the namespace prefix.</param>
        /// <returns>The returned attribute pointer.</returns>
        public IXmlAttribute GetAttributeNodeNS(object namespaceUri, string attributeName)
        {
            return NodeConversion.ConvertNode(_base.Attribute(XNamespace.Get(namespaceUri.ToString()) + attributeName)) as XmlAttribute;
        }

        /// <summary>
        /// Returns a collection of elements that have the specified name.
        /// </summary>
        /// <param name="tagName">The element name to find. 
        /// The value * returns all elements in the document.</param>
        /// <returns>The collection of elements that match the specified name.</returns>
        public IXmlNodeList GetElementsByTagName(string tagName)
        {
            if (tagName == "*")
                return new XmlNodeList(_base.Descendants());

            var matched = from XElement element in _base.Descendants()
                          where element.Name.LocalName == tagName
                          select element;
            return new XmlNodeList(matched);
        }

        /// <summary>
        /// Removes or replaces the named attribute.
        /// </summary>
        /// <param name="attributeName">The name of the attribute to be removed.</param>
        public void RemoveAttribute(string attributeName)
        {
            _base.Attribute(attributeName).Remove();
        }

        /// <summary>
        /// Removes the specified attribute from this element in the specified namespace.
        /// </summary>
        /// <param name="namespaceUri">The namespace of attribute to be removed.</param>
        /// <param name="attributeName">The name of the attribute without the namespace prefix.</param>
        public void RemoveAttributeNS(object namespaceUri, string attributeName)
        {
            _base.Attribute(XNamespace.Get(namespaceUri.ToString()) + attributeName).Remove();
        }

        /// <summary>
        /// Removes the specified attribute from this element.
        /// </summary>
        /// <param name="attributeNode">The attribute to be removed.</param>
        public IXmlAttribute RemoveAttributeNode(IXmlAttribute attributeNode)
        {
            XmlAttribute attr = attributeNode as XmlAttribute;
            if (attributeNode.ParentNode != this)
                return null;

            attr._base.Remove();
            return attributeNode;
        }

        /// <summary>
        /// Sets the value of the named attribute.
        /// </summary>
        /// <param name="attributeName">The name of the required attribute.</param>
        /// <param name="attributeValue">The new value for the attribute.</param>
        public void SetAttribute(string attributeName, string attributeValue)
        {
            _base.SetAttributeValue(attributeName, attributeValue);
        }

        /// <summary>
        /// Sets the value of the named attribute
        /// </summary>
        /// <param name="namespaceUri">The namespace of the attribute being set.</param>
        /// <param name="attributeName">The name of the attribute being set.</param>
        /// <param name="attributeValue">The new value for the attribute.</param>
        public void SetAttributeNS(object namespaceUri, string attributeName, string attributeValue)
        {
            _base.SetAttributeValue(XNamespace.Get(namespaceUri.ToString()) + attributeName, attributeValue);
        }

        /// <summary>
        /// Sets or updates the supplied attribute node on this element.
        /// </summary>
        /// <param name="attributeNode">A pointer to the new attribute.</param>
        /// <returns>The returned pointer to the previous attribute (if any) with the same name.</returns>
        public IXmlAttribute SetAttributeNode(IXmlAttribute attributeNode)
        {
            XmlAttribute attr = attributeNode as XmlAttribute;
            XmlAttribute existing = GetAttributeNode(attributeNode.LocalName.ToString()) as XmlAttribute;
            if (existing != null)
                existing._base.Remove();
            _base.Add(attr._base);
            return existing;
        }

        /// <summary>
        /// Sets or updates the supplied attribute node on this element.
        /// </summary>
        /// <param name="attributeNode">The node to add to the collection.You set the namespace when
        /// you create the node using the CreateAttributeNS method.</param>
        /// <returns>The returned pointer to the previous attribute (if any) with the same name.</returns>
        public IXmlAttribute SetAttributeNodeNS(IXmlAttribute attributeNode)
        {
            XmlAttribute attr = attributeNode as XmlAttribute;
            XAttribute existing = _base.Attribute(attr._base.Name);
            if (existing != null)
                existing.Remove();
            _base.Add(attr._base);
            return NodeConversion.ConvertNode(existing) as XmlAttribute;
        }

        /// <summary>
        /// Gets the element name.
        /// </summary>
        public string TagName
        {
            get { return _base.Name.ToString(); }
        }

        /// <summary>
        /// Appends a new child node as the last child of the node.
        /// </summary>
        /// <param name="newNode">The new child node to be appended to the end of the list of children of this node.</param>
        /// <returns>The new child node successfully appended to the list. If null, no object is created.</returns>
        public IXmlNode AppendChild(IXmlNode newNode)
        {
            _base.Add(newNode.GetLinqObject());
            return newNode;
        }

        /// <summary>
        /// Clones a new node.
        /// </summary>
        /// <param name="deep">A flag that indicates whether to recursively clone all nodes that are
        /// descendants of this node. If true, this method creates a clone of the complete tree below
        /// this node. If false, this method clones this node and its attributes only.</param>
        /// <returns>The newly created clone node.</returns>
        public IXmlNode CloneNode(bool deep)
        {
            if (deep)
                return NodeConversion.ConvertNode(new XElement(_base));

            XElement newElem = new XElement(_base.Name);
            foreach (XAttribute attr in _base.Attributes())
            {
                newElem.SetAttributeValue(attr.Name, attr.Value);
            }

            return NodeConversion.ConvertNode(newElem);
        }

        /// <summary>
        /// Determines whether a node has children.
        /// </summary>
        /// <returns>True if this node has children; otherwise false.</returns>
        public bool HasChildNodes()
        {
            return _base.Nodes().Count() > 0;
        }

        /// <summary>
        /// Inserts a child node to the left of the specified node, or at the end of the child node list.
        /// </summary>
        /// <param name="newChild">The address of the new node to be inserted. The node passed here must
        /// be a valid child of the current XML DOM document node. For example, if the current node is an
        /// attribute, you cannot pass another attribute in the newChild parameter, because an attribute
        /// cannot have an attribute as a child. If newChild is a DOCUMENT_FRAGMENT node type, all its
        /// children are inserted in order before referenceChild. If newChild is already in the tree, it
        /// is first removed before it is reinserted before the referenceChild node. Read-only nodes,
        /// such as NODE_DOCUMENT_TYPE and NODE_ENTITY nodes, cannot be passed in the newChild parameter.</param>
        /// <param name="referenceNode">The reference node. The node specified is where the newChild node
        /// is to be inserted to the left as the preceding sibling in the child list. The node passed here
        /// must be a either a child node of the current node or null. If the value is null, the newChild
        /// node is inserted at the end of the child list. If the referenceChild node is not a child of
        /// the current node, an error is returned.</param>
        /// <returns>On success, the child node that was inserted. If null, no object is created.</returns>
        public IXmlNode InsertBefore(IXmlNode newChild, IXmlNode referenceNode)
        {
            if (newChild.GetLinqObject() == referenceNode.GetLinqObject())
                return newChild;        // remove + insert would leave the document unchanged

            var myChildren = ChildNodes.ToList<IXmlNode>();
            int idx = myChildren.IndexOf(referenceNode);
            if (idx == -1)
                throw new ArgumentException("The supplied referenceNode is not a child of this XmlElement.");

            int removeIdx = -1;
            if (newChild.NodeType != NodeType.DocumentFragmentNode)
                removeIdx = _base.Nodes().ToList<XNode>().IndexOf(newChild.GetLinqObject() as XNode);

            switch (newChild.NodeType)
            {
                case NodeType.DocumentNode:
                case NodeType.EntityNode:
                    {
                        throw new ArgumentException("This node type cannot be used as the child of an XmlDocument.");
                    }
                case NodeType.AttributeNode:
                case NodeType.TextNode:
                case NodeType.CDATANode:
                case NodeType.EntityReferenceNode:
                case NodeType.ElementNode:
                    {
                        // break to use the generalized insertion algorithm below
                        break;
                    }
                case NodeType.DocumentFragmentNode:
                    {
                        XContainer container = newChild.GetLinqObject() as XContainer;

                        // we insert the fragments child nodes -- must contain at most one element, and
                        // that only if we have no root element already
                        int newElementCount = container.Elements().Count();
                        if (newElementCount > 1)
                            throw new ArgumentException("Cannot have more than one element child of an XmlDocument.");

                        // specialized insertion algorithm: insert all children of the fragment before the reference node
                        // NB: we remove any nodes which are in the list already
                        var newChildren = newChild.ChildNodes;
                        foreach (IXmlNode newNode in newChildren)
                        {
                            int thisNodeExistingIdx = myChildren.IndexOf(newNode);
                            if (thisNodeExistingIdx < idx)
                                idx--;      // drop insertion index to account for the (pending) removal
                        }

                        if (idx < 0)
                            idx = 0;       // shouldn't happen, but let's be safe...

                        // now remove and insert each node from the child list above.
                        // this is because XML To Linq only implements Add() for head & tail.
                        foreach (IXmlNode newNode in newChildren)
                        {
                            myChildren.Remove(newNode);
                        }
                        myChildren.InsertRange(idx, newChildren);

                        // convert to a list of the underlying XObject types
                        var linqNodes = myChildren.Select<IXmlNode, XObject>((IXmlNode input) =>
                        {
                            return input.GetLinqObject();
                        });

                        // tell the XDocument object to replace all its children with the new (correctly-ordered) list
                        _base.ReplaceNodes(linqNodes);

                        // done!
                        return newChild;
                    }
                default:
                    break;  // all other non-element applicable node types end up here
            }

            // now the algorithm:
            // we have a List<> of our child nodes.
            // we will update the index of the reference node to account for any removal.
            // we will insert the new node into the list.
            // the list is used to replace all children of this XDocument.

            if (removeIdx >= 0)
            {
                if (removeIdx < idx)
                    idx--;
                myChildren.Remove(newChild);
            }

            myChildren.Insert(idx, newChild);

            // convert to a list of the underlying XObject types
            var newLinqNodes = myChildren.Select<IXmlNode, XObject>((IXmlNode input) =>
            {
                return input.GetLinqObject();
            });

            // tell the XDocument object to replace all its children with the new (correctly-ordered) list
            _base.ReplaceNodes(newLinqNodes);

            // done!
            return newChild;
        }

        /// <summary>
        /// Normalizes all descendant elements by combining two or more adjacent text nodes into one unified text node.
        /// </summary>
        public void Normalize()
        {
            XmlText prior = null;
            var children = ChildNodes.ToList<IXmlNode>();
            var toRemove = new List<IXmlNode>();

            foreach (IXmlNode child in children)
            {
                switch (child.NodeType)
                {
                    case NodeType.SignificantWhitespaceNode:
                    case NodeType.TextNode:
                        {
                            if (prior != null)
                            {
                                prior.AppendData((child as IXmlText).Data);
                                toRemove.Add(child);
                                continue;
                            }
                            else
                            {
                                prior = child as XmlText;
                            }

                            break;
                        }
                    case NodeType.WhitespaceNode:
                        {
                            if (prior != null)
                            {
                                // collapsible whitespace becomes a space character
                                prior.AppendData(" ");
                                toRemove.Add(child);
                                continue;
                            }
                            else
                            {
                                prior = child as XmlText;
                            }

                            break;
                        }

                    default:
                        prior = null;
                        child.Normalize();
                        break;
                }
            }

            if (toRemove.Count() == 0)
                return;

            children.RemoveAll(c => toRemove.Contains(c));

            var linqNodes = children.Select(n => n.GetLinqObject());
            _base.ReplaceNodes(linqNodes);
        }

        /// <summary>
        /// Removes the specified child node from the list of children and returns it.
        /// </summary>
        /// <param name="childNode">The child node to be removed from the list of children of this node.</param>
        /// <returns>The removed child node. If null, the childNode object is not removed.</returns>
        public IXmlNode RemoveChild(IXmlNode childNode)
        {
            if (childNode.ParentNode != this)
                return null;

            var linq = childNode.GetLinqObject() as XNode;
            if (linq == null)
                return null;

            linq.Remove();
            return childNode;
        }

        /// <summary>
        /// Replaces the specified old child node with the supplied new child node.
        /// </summary>
        /// <param name="newChild">The new child that is to replace the old child.
        /// If null, the referenceChild parameter is removed without a replacement.</param>
        /// <param name="referenceChild">The old child that is to be replaced by the new child.</param>
        /// <returns>The old child that is replaced. If null, no object is created.</returns>
        public IXmlNode ReplaceChild(IXmlNode newChild, IXmlNode referenceChild)
        {
            if (referenceChild.ParentNode != this)
                return null;
            if (newChild == null)
                return RemoveChild(referenceChild);

            var children = _base.Nodes().ToList<XNode>();
            var refLinq = referenceChild.GetLinqObject() as XNode;
            var newLinq = newChild.GetLinqObject() as XNode;
            int idx = children.IndexOf(refLinq);
            if (idx < 0)
                return null;

            children.Remove(referenceChild.GetLinqObject() as XNode);
            children.Insert(idx, newLinq);
            _base.ReplaceNodes(children);
            return referenceChild;
        }

        /// <summary>
        /// Returns the root of the document that contains the node.
        /// </summary>
        public IXmlDocument OwnerDocument
        {
            get { return NodeConversion.ConvertNode(_base.Document) as XmlDocument; }
        }

        /// <summary>
        /// Gets the parent node of the node instance.
        /// </summary>
        /// <result>This property returns NULL.</result>
        public IXmlNode ParentNode
        {
            get { return NodeConversion.ConvertNode(_base.Parent); }
        }

        /// <summary>
        /// Gets the list of attributes of this node.
        /// </summary>
        /// <result>This property returns NULL.</result>
        public IXmlNamedNodeMap Attributes
        {
            get { return new XmlNamedNodeMap(_base.Attributes()); }
        }

        /// <summary>
        /// Gets a list of children in the current node.
        /// </summary>
        public IXmlNodeList ChildNodes
        {
            get { return new XmlNodeList(_base.Nodes()); }
        }


        /// <summary>
        /// Gets the first child node.
        /// </summary>
        public IXmlNode FirstChild
        {
            get { return NodeConversion.ConvertNode(_base.FirstNode); }
        }

        /// <summary>
        /// Gets the last child node.
        /// </summary>
        public IXmlNode LastChild
        {
            get { return NodeConversion.ConvertNode(_base.LastNode); }
        }

        /// <summary>
        /// Gets the local name, which is the local part of a qualified name.
        /// This is called the local part in Namespaces in XML.
        /// </summary>
        /// <result>This property returns NULL.</result>
        public object LocalName
        {
            get { return _base.Name.LocalName; }
        }

        /// <summary>
        /// This method does nothing on an XmlDocument.
        /// </summary>
        public string Prefix
        {
            get
            {
                string uri = _base.Name.Namespace.NamespaceName;
                return (OwnerDocument as XmlDocument)._base.CreateNamespaceManager().LookupPrefix(uri);
            }
            set
            {
                throw new InvalidOperationException();
            }
        }

        /// <summary>
        /// Returns the Uniform Resource Identifier (URI) for the namespace.
        /// </summary>
        /// <result>For convenience, returns the NamespaceUri of the document's root element.</result>
        public object NamespaceUri
        {
            get
            {
                return _base.Name.NamespaceName;
            }
        }

        /// <summary>
        /// Returns the qualified name for attribute, document type, element, entity, or notation nodes.
        /// Returns a fixed string for all other node types.
        /// </summary>
        public string NodeName
        {
            get { return _base.Name.ToString(); }
        }

        /// <summary>
        /// Gets the XML Document Object Model (DOM) node type, which determines valid values and
        /// whether the node can have child nodes.
        /// </summary>
        public NodeType NodeType
        {
            get { return NodeType.ElementNode; }
        }

        /// <summary>
        /// Gets or sets the text associated with the node.
        /// </summary>
        public object NodeValue
        {
            get
            {
                return _base.Value;
            }
            set
            {
                var nodes = ChildNodes;
                if (nodes.Length == 1 && nodes.First() is IXmlText)
                {
                    IXmlText textObj = nodes.First() as IXmlText;
                    textObj.Data = value.ToString();
                }
                else if (nodes.Length == 0)
                {
                    AppendChild(NodeConversion.ConvertNode(new XText(value.ToString())));
                }
            }
        }

        /// <summary>
        /// Gets the next sibling of the node in the parent's child list. This property is not applicable to this class.
        /// </summary>
        public IXmlNode NextSibling
        {
            get { return NodeConversion.ConvertNode(_base.NextNode); }
        }

        /// <summary>
        /// Gets the previous sibling of the node in the parent's child list. This property is not applicable to this class.
        /// </summary>
        public IXmlNode PreviousSibling
        {
            get { return NodeConversion.ConvertNode(_base.PreviousNode); }
        }

        /// <summary>
        /// Applies the specified pattern-matching operation to this node's context and returns
        /// the list of matching nodes as an XmlNodeList.
        /// </summary>
        /// <param name="xpath">Specifies an XPath expression.</param>
        /// <returns>The collection of nodes selected by applying the given pattern-matching operation.
        /// If no nodes are selected, this method returns an empty collection.</returns>
        public IXmlNodeList SelectNodes(string xpath)
        {
            return SelectNodesNS(xpath, null);
        }

        /// <summary>
        /// Applies the specified pattern-matching operation to this node's context and returns the list
        /// of matching nodes as an XmlNodeList.
        /// </summary>
        /// <param name="xpath">Specifies an XPath expression.</param>
        /// <param name="namespaces">Contains a string that specifies namespaces for use in XPath expressions
        /// when it is necessary to define new namespaces externally. Namespaces are defined in the XML style,
        /// as a space-separated list of namespace declaration attributes. You can use this property to set
        /// the default namespace as well.</param>
        /// <returns>The collection of nodes selected by applying the given pattern-matching operation.
        /// If no nodes are selected, returns an empty collection.</returns>
        public IXmlNodeList SelectNodesNS(string xpath, object namespaces)
        {
            IXmlNamespaceResolver resolver = XPathProcessor.MakeNamespaceResolver(namespaces);
            var selected = XPathProcessor.ProcessXPathExpression(_base, xpath, resolver);
            return XPathProcessor.NodeListFromXPathResult(selected);
        }

        /// <summary>
        /// Applies the specified pattern-matching operation to this node's context and returns the first matching node.
        /// </summary>
        /// <param name="xpath">Specifies an XPath expression.</param>
        /// <returns>The first node that matches the given pattern-matching operation.
        /// If no nodes match the expression, the method returns a null value.</returns>
        public IXmlNode SelectSingleNode(string xpath)
        {
            return SelectSingleNodeNS(xpath, null);
        }

        /// <summary>
        /// Applies the specified pattern-matching operation to this node's context and returns the first matching node.
        /// </summary>
        /// <param name="xpath">Specifies an XPath expression.</param>
        /// <param name="namespaces">Contains a string that specifies namespaces for use in XPath expressions
        /// when it is necessary to define new namespaces externally. Namespaces are defined in the XML style,
        /// as a space-separated list of namespace declaration attributes. You can use this property to set
        /// the default namespace as well.</param>
        /// <returns>The first node that matches the given pattern-matching operation.
        /// If no nodes match the expression, the method returns a null value.</returns>
        public IXmlNode SelectSingleNodeNS(string xpath, object namespaces)
        {
            IXmlNamespaceResolver resolver = XPathProcessor.MakeNamespaceResolver(null);
            var selected = XPathProcessor.ProcessXPathExpression(_base, xpath, resolver);
            if (selected is XObject)
            {
                return NodeConversion.ConvertNode(selected as XNode);
            }
            else if (selected is IEnumerable<XObject>)
            {
                var list = selected as IEnumerable<XObject>;
                return NodeConversion.ConvertNode(list.First());
            }

            // not a node? Nothing found?
            return null;
        }

        /// <summary>
        /// Returns the XML representation of the node and all its descendants.
        /// </summary>
        /// <returns>The XML representation of the node and all its descendants.</returns>
        public string GetXml()
        {
            return _base.ToString(SaveOptions.OmitDuplicateNamespaces);
        }

        /// <summary>
        /// Gets the text from inside the XML.
        /// </summary>
        public string InnerText
        {
            get
            {
                return _base.Value;
            }
            set
            {
                this.NodeValue = value;
            }
        }
    }
}