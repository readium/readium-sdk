//
//  IXmlDocument.cs
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
using System.IO;
using System.Threading.Tasks;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Linq;
using System.Xml;
using System.Xml.Linq;
using Windows.Foundation;
using Windows.Foundation.Metadata;
using Windows.Storage;
using System.Net;
using PhoneSupportInterfaces;

namespace ReadiumPhoneSupport
{
    [ComVisibleAttribute(true)]
    public class XmlDocument : IXmlDocument, IXmlDocumentIO
    {
        internal XDocument _base;

        internal XmlDocument(XDocument linq)
        {
            _base = linq;
        }

        /// <summary>
        /// Creates a new instance of the XmlDocument class.
        /// </summary>
        public XmlDocument()
        {
            _base = new XDocument();
        }

        /// <summary>
        /// Creates a new attribute with the specified name.
        /// </summary>
        /// <param name="name">The name of the new attribute object.
        /// This name is subsequently available as the new node's NodeName property.</param>
        /// <returns>The newly created attribute.</returns>
        public IXmlAttribute CreateAttribute(string name)
        {
            return NodeConversion.ConvertNode(new XAttribute(name, "")) as XmlAttribute;
        }

        /// <summary>
        /// Creates a new attribute with the specified namespace and name.
        /// </summary>
        /// <param name="uri">The name of the desired namespace or a null value if no namespace is desired.</param>
        /// <param name="name">The name of the new attribute object.
        /// This name is subsequently available as the new node's NodeName property.</param>
        /// <returns>The newly created attribute.</returns>
        public IXmlAttribute CreateAttributeNS(object uri, string name)
        {
            if (uri == null)
                return CreateAttribute(name);
            return NodeConversion.ConvertNode(new XAttribute(XNamespace.Get(uri.ToString()) + name, "")) as XmlAttribute;
        }

        /// <summary>
        /// Creates a CDATA section node that contains the supplied data.
        /// </summary>
        /// <param name="data">The value to be supplied to the new CDATA section object's NodeValue property.</param>
        /// <returns>The newly created object.</returns>
        public IXmlCDataSection CreateCDataSection(string data)
        {
            return NodeConversion.ConvertNode(new XCData(data)) as XmlCDataSection;
        }

        /// <summary>
        /// Creates a comment node that contains the supplied data.
        /// </summary>
        /// <param name="comment">The value to be supplied to the new comment object's NodeValue.</param>
        /// <returns>The newly created comment node.</returns>
        public IXmlComment CreateComment(string comment)
        {
            return NodeConversion.ConvertNode(new XComment(comment)) as XmlComment;
        }

        /// <summary>
        /// Creates an empty XmlDocumentFragment object.
        /// </summary>
        /// <returns>The newly created object.</returns>
        public IXmlDocumentFragment CreateDocumentFragment()
        {
            return NodeConversion.CreateDocumentFragment();
        }

        /// <summary>
        /// Creates an element node using the specified name.
        /// </summary>
        /// <param name="tagName">The name for the new element node. The string is case-sensitive.
        /// This name is subsequently available as the element node's NodeName property.</param>
        /// <returns>The newly created object.</returns>
        public IXmlElement CreateElement(string tagName)
        {
            return NodeConversion.ConvertNode(new XElement(tagName)) as XmlElement;
        }

        /// <summary>
        /// Creates an element node using the specified namespace and name.
        /// </summary>
        /// <param name="uri">The name of the desired namespace or a null value if no namespace is desired.</param>
        /// <param name="tagName">The name for the new element node. It is case-sensitive.
        /// This name is subsequently available as the element node's NodeName property.</param>
        /// <returns>The newly created object.</returns>
        public IXmlElement CreateElementNS(object uri, string tagName)
        {
            if (uri == null)
                return CreateElement(tagName);
            return NodeConversion.ConvertNode(new XElement(XNamespace.Get(uri.ToString()) + tagName)) as XmlElement;
        }

        /// <summary>
        /// Creates a new XmlEntityReference object.
        /// </summary>
        /// <param name="name">The name of the entity referenced. This name is subsequently available
        /// as the new object's NodeName property.</param>
        /// <returns></returns>
        public IXmlEntityReference CreateEntityReference(string name)
        {
            return NodeConversion.CreateEntityReference(name, new XText(""));
        }

        /// <summary>
        /// Creates a processing instruction node that contains the supplied target and data.
        /// </summary>
        /// <param name="target">The target part of the processing instruction. It supplies the
        /// NodeName property of the new object.</param>
        /// <param name="data">The remainder of the processing instruction preceding the closing ?>
        /// characters. It supplies the NodeValue property for the new object.</param>
        /// <returns>The newly created object.</returns>
        public IXmlProcessingInstruction CreateProcessingInstruction(string target, string data)
        {
            return NodeConversion.ConvertNode(new XProcessingInstruction(target, data)) as XmlProcessingInstruction;
        }

        /// <summary>
        /// Creates a text node that contains the supplied data.
        /// </summary>
        /// <param name="data">The value to be supplied to the new text object's NodeValue.</param>
        /// <returns>The newly created object.</returns>
        public IXmlText CreateTextNode(string data)
        {
            return NodeConversion.ConvertNode(new XText(data)) as XmlText;
        }

        /// <summary>
        /// Returns the element that matches the ID attribute.
        /// </summary>
        /// <param name="elementId">The ID to match.</param>
        /// <returns>The element that matches the supplied ID. If no elements match, this method returns Null.</returns>
        public IXmlElement GetElementById(string elementId)
        {
            var matched = from XElement element in _base.Descendants()
                          where element.Attribute("id").Value == elementId
                          select element;
            if (matched == null || matched.Count() == 0)
                return null;
            return NodeConversion.ConvertNode(matched.First()) as XmlElement;
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
        /// Imports a node from another document to this document.
        /// </summary>
        /// <param name="node">The object to be cloned.</param>
        /// <param name="deep">If true, any children of node will be cloned as well.
        /// If false, no children of the node will be cloned.</param>
        /// <returns>The imported node.</returns>
        public IXmlNode ImportNode(IXmlNode node, bool deep)
        {
            if (node.OwnerDocument == this)
                return node;

            // very simple -- the hard work is handled by the nodes themselves
            // the cloned nodes, whether deep or shallow, will not have a parent element or document.
            return node.CloneNode(deep);
        }

        /// <summary>
        /// For XML, this property gets the address of the node that specifies the DTD.
        /// This property returns Null for for HTML documents and XML documents without a DTD.
        /// </summary>
        public IXmlDocumentType Doctype
        {
            get { return NodeConversion.ConvertNode(_base.DocumentType) as XmlDocumentType; }
        }

        /// <summary>
        /// Gets the root element of the document.
        /// </summary>
        public IXmlElement DocumentElement
        {
            get { return NodeConversion.ConvertNode(_base.Root) as XmlElement; }
        }

        /// <summary>
        /// Returns the URL for the last loaded XML document.
        /// </summary>
        public object DocumentUri
        {
            get { return new Uri(_base.BaseUri); }
        }

        /// <summary>
        /// Gets the Implementation object for the document.
        /// </summary>
        public IXmlDomImplementation Implementation
        {
            get { return new XmlDomImplementation(); }
        }

        /// <summary>
        /// Appends a new child node as the last child of the node.
        /// </summary>
        /// <param name="newNode">The new child node to be appended to the end of the list of children of this node.</param>
        /// <returns>The new child node successfully appended to the list. If null, no object is created.</returns>
        public IXmlNode AppendChild(IXmlNode newNode)
        {
            if (_base.Root != null && newNode.NodeType == NodeType.ElementNode)
                throw new InvalidOperationException("Cannot append a second element to a Document node.");
            _base.Add(newNode.GetLinqObject());
            return newNode;
        }


        /// <summary>
        /// Clones a new node. This method is not applicable to this class.
        /// </summary>
        public IXmlNode CloneNode(bool deep)
        {
            throw new InvalidOperationException();
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
                throw new ArgumentException("The supplied referenceNode is not a child of this XmlDocument.");

            int removeIdx = -1;
            if (newChild.NodeType != NodeType.DocumentFragmentNode)
                removeIdx = _base.Nodes().ToList<XNode>().IndexOf(newChild.GetLinqObject() as XNode);

            switch (newChild.NodeType)
            {
                case NodeType.AttributeNode:
                case NodeType.TextNode:
                case NodeType.CDATANode:
                case NodeType.DocumentNode:
                case NodeType.EntityNode:
                case NodeType.EntityReferenceNode:
                    {
                        throw new ArgumentException("This node type cannot be used as the child of an XmlDocument.");
                    }
                case NodeType.ElementNode:
                    {
                        var root = DocumentElement;
                        if (root != null)
                        {
                            if (root != newChild)
                                throw new ArgumentException("Cannot add a second element as a child of an XmlDocument.");
                        }

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

                        var root = DocumentElement as XmlElement;
                        if (newElementCount == 1 && root != null)
                        {
                            XElement elem = container.Elements().First();
                            if (elem != root._base)
                                throw new ArgumentException("Cannot have more than one element child of an XmlDocument");
                        }

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
            get { return this; }
        }

        /// <summary>
        /// Gets the parent node of the node instance.
        /// </summary>
        /// <result>This property returns NULL.</result>
        public IXmlNode ParentNode
        {
            get { return null; }
        }

        /// <summary>
        /// Gets the list of attributes of this node.
        /// </summary>
        /// <result>This property returns NULL.</result>
        public IXmlNamedNodeMap Attributes
        {
            get { return null; }
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
            get { return null; }
        }

        /// <summary>
        /// This method does nothing on an XmlDocument.
        /// </summary>
        public string Prefix
        {
            get
            {
                return null;
            }
            set
            {
                // does nothing
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
                var root = DocumentElement;
                if (root == null)
                    return null;
                else
                    return root.NamespaceUri;
            }
        }

        /// <summary>
        /// Returns the qualified name for attribute, document type, element, entity, or notation nodes.
        /// Returns a fixed string for all other node types.
        /// </summary>
        public string NodeName
        {
            get { return "DOCUMENT_NODE"; }
        }

        /// <summary>
        /// Gets the XML Document Object Model (DOM) node type, which determines valid values and
        /// whether the node can have child nodes.
        /// </summary>
        public NodeType NodeType
        {
            get { return NodeType.DocumentNode; }
        }

        /// <summary>
        /// Gets or sets the text associated with the node. This is a no-op for XmlDocument.
        /// </summary>
        public object NodeValue
        {
            get
            {
                return null;
            }
            set
            {
                // does nothing
            }
        }

        /// <summary>
        /// Gets the next sibling of the node in the parent's child list. This property is not applicable to this class.
        /// </summary>
        public IXmlNode NextSibling
        {
            get { return null; }
        }

        /// <summary>
        /// Gets the previous sibling of the node in the parent's child list. This property is not applicable to this class.
        /// </summary>
        public IXmlNode PreviousSibling
        {
            get { return null; }
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
                return null;
            }
            set
            {
                throw new InvalidOperationException();
            }
        }

        public void LoadXml(string xml)
        {
            XDocument aDoc = XDocument.Parse(xml);
            if (aDoc != null)
                _base = aDoc;
        }

        public void LoadXml(string xml, IXmlLoadSettings loadSettings)
        {
            XmlLoadSettings concrete = loadSettings as XmlLoadSettings;
            XDocument aDoc = XDocument.Parse(xml, concrete.LinqLoadOptions);
            if (aDoc != null)
                _base = aDoc;
        }

        private async Task _SaveToFileAsync(IStorageFile file)
        {
            var stream = await file.OpenStreamForWriteAsync();
            _base.Save(stream, SaveOptions.OmitDuplicateNamespaces);
        }

        public IAsyncAction SaveToFileAsync(IStorageFile file)
        {
            return _SaveToFileAsync(file).AsAsyncAction();
        }

        private static async Task<IXmlDocument> _LoadFromFileAsync(IStorageFile file)
        {
            var stream = await file.OpenStreamForReadAsync();
            if (stream == null)
                return null;

            XDocument aDoc = XDocument.Load(stream);
            if (aDoc == null)
                return null;

            return new XmlDocument(aDoc);
        }

        public static IAsyncOperation<IXmlDocument> LoadFromFileAsync(IStorageFile file)
        {
            return _LoadFromFileAsync(file).AsAsyncOperation<IXmlDocument>();
        }

        private static async Task<IXmlDocument> _LoadFromFileAsync(IStorageFile file, XmlLoadSettings loadSettings)
        {
            var stream = await file.OpenStreamForReadAsync();
            if (stream == null)
                return null;

            XDocument aDoc = XDocument.Load(stream, loadSettings.LinqLoadOptions);
            if (aDoc == null)
                return null;

            return new XmlDocument(aDoc);
        }

        public static IAsyncOperation<IXmlDocument> LoadFromFileAsync(IStorageFile file, IXmlLoadSettings loadSettings)
        {
            return _LoadFromFileAsync(file, loadSettings as XmlLoadSettings).AsAsyncOperation<IXmlDocument>();
        }

        private static IXmlDocument _LoadFromUriAsync(Uri uri, XmlLoadSettings loadSettings)
        {
            AsyncUriLoader loader = new AsyncUriLoader();
            Stream stream = loader.GetStreamForUri(uri);
            if (stream == null)
                return null;

            XDocument aDoc = XDocument.Load(stream, (loadSettings == null ? LoadOptions.None : loadSettings.LinqLoadOptions));
            if (aDoc == null)
                return null;

            return new XmlDocument(aDoc);
        }

        public static IAsyncOperation<IXmlDocument> LoadFromUriAsync(Uri uri)
        {
            return new Task<IXmlDocument>(() => { return _LoadFromUriAsync(uri, null); }).AsAsyncOperation<IXmlDocument>();
        }

        public static IAsyncOperation<IXmlDocument> LoadFromUriAsync(Uri uri, IXmlLoadSettings loadSettings)
        {
            return new Task<IXmlDocument>(() => { return _LoadFromUriAsync(uri, loadSettings as XmlLoadSettings); }).AsAsyncOperation<IXmlDocument>();
        }
    }
}