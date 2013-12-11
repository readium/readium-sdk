//
//  XmlInterfaces.cs
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
using Windows.Foundation;
using Windows.Foundation.Metadata;
using Windows.Storage;

namespace ReadiumPhoneSupport
{
    [ComVisibleAttribute(true)]
    public enum NodeType
    {
        Invalid = XmlNodeType.None,
        ElementNode = XmlNodeType.Element,
        AttributeNode = XmlNodeType.Attribute,
        TextNode = XmlNodeType.Text,
        CDATANode = XmlNodeType.CDATA,
        EntityReferenceNode = XmlNodeType.EntityReference,
        EntityNode = XmlNodeType.Entity,
        ProcessingInstructionNode = XmlNodeType.ProcessingInstruction,
        CommentNode = XmlNodeType.Comment,
        DocumentNode = XmlNodeType.Document,
        DocumentTypeNode = XmlNodeType.DocumentType,
        DocumentFragmentNode = XmlNodeType.DocumentFragment,
        NotationNode = XmlNodeType.Notation,
        WhitespaceNode = XmlNodeType.Whitespace,
        SignificantWhitespaceNode = XmlNodeType.SignificantWhitespace,
        EndElementNode = XmlNodeType.EndElement,
        EndEntityNode = XmlNodeType.EndEntity,
        XmlDeclarationNode = XmlNodeType.XmlDeclaration
    }

    /// <summary>
    /// Encapsulates the methods needed to execute XPath queries on an XML DOM tree or subtree.
    /// </summary>
    [ComVisibleAttribute(true)]
    public interface IXmlNodeSelector
    {
        /// <summary>
        /// Applies the specified pattern-matching operation to this node's context and 
        /// returns the list of matching nodes as an XmlNodeList.
        /// </summary>
        /// <param name="xpath">Specifies an XPath expression.</param>
        /// <returns>
        /// The collection of nodes selected by applying the given pattern-matching operation.
        /// If no nodes are selected, this method returns an empty collection.
        /// </returns>
        IXmlNodeList SelectNodes(string xpath);

        /// <summary>
        /// Applies the specified pattern-matching operation to this node's context and
        /// returns the list of matching nodes as an XmlNodeList.
        /// </summary>
        /// <param name="xpath">Specifies an XPath expresssion.</param>
        /// <param name="namespaces">Contains a string that specifies namespaces for use 
        /// in XPath expressions when it is necessary to define new namespaces externally.
        /// Namespaces are defined in the XML style, as a space-separated list of namespace
        /// declaration attributes. You can use this property to set the default namespace
        /// as well.</param>
        /// <returns>The collection of nodes selected by applying the given pattern-matching
        /// operation. If no nodes are selected, returns an empty collection.</returns>
        IXmlNodeList SelectNodesNS(string xpath, [Windows.Foundation.Metadata.VariantAttribute] object namespaces);

        /// <summary>
        /// Applies the specified pattern-matching operation to this node's context and 
        /// returns the first matching node.
        /// </summary>
        /// <param name="xpath">Specifies an XPath expression.</param>
        /// <returns>The first node that matches the given pattern-matching operation.
        /// If no nodes match the expression, the method returns a null value.</returns>
        IXmlNode SelectSingleNode(string xpath);

        /// <summary>
        /// Applies the specified pattern-matching operation to this node's context and
        /// returns the first matching node.
        /// </summary>
        /// <param name="xpath">Specifies an XPath expression.</param>
        /// <param name="namespaces">Contains a string that specifies the namespaces to
        /// use in XPath expressions when it is necessary to define new namespaces externally.
        /// Namespaces are defined in the XML style, as a space-separated list of namespace
        /// declaration attributes. You can use this property to set the default namespace
        /// as well.</param>
        /// <returns>The first node that matches the given pattern-matching operation.
        /// If no nodes match the expression, this method returns a null value.</returns>
        IXmlNode SelectSingleNodeNS(string xpath, [Windows.Foundation.Metadata.VariantAttribute] object namespaces);
    }

    [ComVisibleAttribute(true)]
    public interface IXmlNodeSerializer
    {
        /// <summary>
        /// Returns the XML representation of the node and all its descendants.
        /// </summary>
        /// <returns>The XML representation of the node and all its descendants.</returns>
        string GetXml();

        /// <summary>
        /// Gets and sets the text from inside the XML.
        /// </summary>
        /// <result>The text from inside the XML. Returns an empty string if there is no text.</result>
        string InnerText { get; set; }
    }

    [ComVisibleAttribute(true)]
    public interface IXmlNode : IXmlNodeSelector, IXmlNodeSerializer
    {
        /// <summary>
        /// Appends a new node as the last child of the node.
        /// </summary>
        /// <param name="newNode">
        /// The new child node to be appended to the end of the list of children of this node.
        /// </param>
        /// <returns>
        /// The new child node successfully appended to the list. If null, no object is created.
        /// </returns>
        IXmlNode AppendChild(IXmlNode newNode);

        /// <summary>
        /// Clones a new node.
        /// </summary>
        /// <param name="deep">
        /// A flag that indicates whether to recursively clone all nodes that are descendants of
        /// this node. If true, this method creates a clone of the complete tree below this node.
        /// If false, this method clones this node and its attributes only.
        /// </param>
        /// <returns>The newly created clone node.</returns>
        IXmlNode CloneNode(bool deep);

        /// <summary>
        /// Determines whether a node has children.
        /// </summary>
        /// <returns>True if this node has children; otherwise false.</returns>
        bool HasChildNodes();

        /// <summary>
        /// Inserts a child node to the left of the specified node, or at the end of the list.
        /// </summary>
        /// <param name="newChild">
        /// The address of the new node to be inserted. The node passed here must be a valid
        /// child of the current XML DOM document node. For example, if the current node is an
        /// attribute, you cannot pass another attribute in the newChild parameter, because an
        /// attribute cannot have an attribute as a child. If newChild is a DOCUMENT_FRAGMENT
        /// node type, all its children are inserted in order before referenceChild. If newChild
        /// is already in the tree, it is first removed before it is reinserted before the 
        /// referenceChild node. Read-only nodes, such as NODE_DOCUMENT_TYPE and NODE_ENTITY
        /// nodes, cannot be passed in the newChild parameter.
        /// </param>
        /// <param name="referenceNode">
        /// The reference node. The node specified is where the newChild node is to be inserted
        /// to the left as the preceding sibling in the child list. The node passed here must be
        /// a either a child node of the current node or null. If the value is null, the newChild
        /// node is inserted at the end of the child list. If the referenceChild node is not a
        /// child of the current node, an error is returned.
        /// </param>
        /// <returns>On success, the child node that was inserted. If null, no object is created.</returns>
        IXmlNode InsertBefore(IXmlNode newChild, IXmlNode referenceNode);

        /// <summary>
        /// Normalizes all descendant elements by combining two or more adjacent text nodes
        /// into one unified text node.
        /// </summary>
        void Normalize();

        /// <summary>
        /// Removes the specified child node from the list of children and returns it.
        /// </summary>
        /// <param name="childNode">The child node to be removed from the list of children of this node.</param>
        /// <returns>The removed child node. If null, the childNode object is not removed.</returns>
        IXmlNode RemoveChild(IXmlNode childNode);

        /// <summary>
        /// Replaces the specified old child node with the supplied new child node.
        /// </summary>
        /// <param name="newChild">
        /// The new child that is to replace the old child. If null, the referenceChild parameter
        /// is removed without a replacement.
        /// </param>
        /// <param name="referenceChild">The old child that is to be replaced by the new child.</param>
        /// <returns>The old child that is replaced. If null, no object is created.</returns>
        IXmlNode ReplaceChild(IXmlNode newChild, IXmlNode referenceChild);

        /// <summary>
        /// Returns the root of the document that contains the node.
        /// </summary>
        XmlDocument OwnerDocument { get; }

        /// <summary>
        /// Gets the parent node of the node instance.
        /// </summary>
        IXmlNode ParentNode { get; }

        /// <summary>
        /// Gets the list of attributes of this node.
        /// </summary>
        XmlNamedNodeMap Attributes { get; }

        /// <summary>
        /// Gets a list of children in the current node.
        /// </summary>
        XmlNodeList ChildNodes { get; }

        /// <summary>
        /// Gets the first child node.
        /// </summary>
        IXmlNode FirstChild { get; }

        /// <summary>
        /// Gets the last child node.
        /// </summary>
        IXmlNode LastChild { get; }

        /// <summary>
        /// Gets the local name, which is the local part of a qualified name.
        /// This is called the local part in Namespaces in XML.
        /// </summary>
        object LocalName { get; }

        /// <summary>
        /// Gets or sets the namespace prefix.
        /// </summary>
        /// <result>
        /// The namespace prefix specified on the element, attribute, or entity reference.
        /// For example, for the element <xxx:yyy>, this property returns xxx. It returns
        /// an empty string, "", if no prefix is specified.
        /// </result>
        string Prefix { get; set; }

        /// <summary>
        /// Returns the Uniform Resource Identifier (URI) for the namespace.
        /// </summary>
        /// <result>The Uri for the namespace. This refers to the uuu portion of the namespace declaration xmlns:nnn=uuu.</result>
        object NamespaceUri { get; }

        /// <summary>
        /// Returns the qualified name for attribute, document type, element, entity, or notation nodes.
        /// Returns a fixed string for all other node types.
        /// </summary>
        string NodeName { get; }

        /// <summary>
        /// Gets the XML Document Object Model (DOM) node type, which determines valid values and
        /// whether the node can have child nodes.
        /// </summary>
        NodeType NodeType { get; }

        /// <summary>
        /// Gets or sets the text associated with the node.
        /// </summary>
        object NodeValue { get; set; }

        /// <summary>
        /// Gets the next sibling of the node in the parent's child list.
        /// </summary>
        /// <result>The right sibling of this node.</result>
        IXmlNode NextSibling { get; }

        /// <summary>
        /// Gets the previous sibling of the node in the parent's child list.
        /// </summary>
        /// <result>The left sibling of this node.</result>
        IXmlNode PreviousSibling { get; }
    }

    [ComVisibleAttribute(true)]
    public interface IDtdEntity : IXmlNode
    {
        object NotationName { get; }
        object PublicId { get; }
        object SystemId { get; }
    }

    [ComVisibleAttribute(true)]
    public interface IDtdNotation : IXmlNode
    {
        object PublicId { get; }
        object SystemId { get; }
    }

    [ComVisibleAttribute(true)]
    public interface IXmlEntityReference : IXmlNode
    { }

    [ComVisibleAttribute(true)]
    public interface IXmlProcessingInstruction : IXmlNode
    {
        string Data { get; set; }
        string Target { get; }
    }

    [ComVisibleAttribute(true)]
    public interface IXmlAttribute : IXmlNode
    {
        string Name { get; }
        bool Specified { get; }
        string Value { get; set; }
    }

    /// <summary>
    /// Provides text manipulation methods that are used by several objects.
    /// </summary>
    [ComVisibleAttribute(true)]
    public interface IXmlCharacterData : IXmlNode
    {
        /// <summary>
        /// Appends the supplied string to the existing string data.
        /// </summary>
        /// <param name="data">The data to be appended to the existing string.</param>
        void AppendData(string data);

        /// <summary>
        /// Deletes specified data.
        /// </summary>
        /// <param name="offset">The offset, in characters, at which to start deleting the string data.</param>
        /// <param name="count">The number of characters to delete.</param>
        void DeleteData(uint offset, uint count);

        /// <summary>
        /// Inserts a string at the specified offset.
        /// </summary>
        /// <param name="offset">The offset, in characters, at which to insert the supplied string data.</param>
        /// <param name="data">The data to be inserted into the existing string.</param>
        void InsertData(uint offset, string data);

        /// <summary>
        /// Replaces the specified number of characters with the supplied string.
        /// </summary>
        /// <param name="offset">The offset, in characters, at which to start replacing string data.</param>
        /// <param name="count">The number of characters to replace.</param>
        /// <param name="data">The new data that replaces the old string data.</param>
        void ReplaceData(uint offset, uint count, string data);

        /// <summary>
        /// Retrieves a substring of the full string from the specified range.
        /// </summary>
        /// <param name="offset">Specifies the offset, in characters, from the beginning of the string.
        /// An offset of zero indicates copying from the start of the data.</param>
        /// <param name="count">Specifies the number of characters to retrieve from the specified offset.</param>
        /// <returns>The returned substring.</returns>
        string SubstringData(uint offset, uint count);

        /// <summary>
        /// Gets or sets the node data depending on the node type.
        /// </summary>
        /// <result>The node data.</result>
        string Data { get; set; }

        /// <summary>
        /// Gets the length of the data, in Unicode characters.
        /// </summary>
        uint Length { get; }
    }

    /// <summary>
    /// Represents the text content of an element or attribute.
    /// </summary>
    [ComVisibleAttribute(true)]
    public interface IXmlText : IXmlCharacterData
    {
        /// <summary>
        /// Splits this text node into two text nodes at the specified offset and inserts the new
        /// text node into the tree as a sibling that immediately follows this node.
        /// </summary>
        /// <param name="offset">The number of characters at which to split this text node into
        /// two nodes, starting from zero.</param>
        /// <returns>The new text node.</returns>
        IXmlText SplitText(uint offset);
    }

    [ComVisibleAttribute(true)]
    public interface IXmlCDataSection : IXmlText { }

    [ComVisibleAttribute(true)]
    public interface IXmlElement : IXmlNode
    {
        string GetAttribute(string attributeName);
        string GetAttributeNS([VariantAttribute] object namespaceUri, string attributeName);
        XmlAttribute GetAttributeNode(string attributeName);
        XmlAttribute GetAttributeNodeNS([VariantAttribute] object namespaceUri, string attributeName);
        XmlNodeList GetElementsByTagName(string tagName);
        void RemoveAttribute(string attributeName);
        void RemoveAttributeNs([VariantAttribute] object namespaceUri, string attributeName);
        XmlAttribute RemoveAttributeNode(XmlAttribute attributeNode);
        void SetAttribute(string attributeName, string attributeValue);
        void SetAttributeNS([VariantAttribute] object namespaceUri, string attributeName, string attributeValue);
        XmlAttribute SetAttributeNode(XmlAttribute attributeNode);
        XmlAttribute SetAttributeNodeNS(XmlAttribute attributeNode);
        string TagName { get; }
    }

    [ComVisibleAttribute(true)]
    public interface IXmlComment : IXmlCharacterData
    { }

    [ComVisibleAttribute(true)]
    public interface IXmlDocument : IXmlNode
    {
        XmlAttribute CreateAttribute(string name);
        XmlAttribute CreateAttributeNS(object uri, string name);
        XmlCDataSection CreateCDataSection(string data);
        XmlComment CreateComment(string comment);
        XmlDocumentFragment CreateDocumentFragment();
        XmlElement CreateElement(string tagName);
        XmlElement CreateElementNS(object uri, string tagName);
        XmlEntityReference CreateEntityReference(string name);
        XmlProcessingInstruction CreateProcessingInstruction(string target, string data);
        XmlText CreateTextNode(string data);
        XmlElement GetElementById(string elementId);
        XmlNodeList GetElementsByTagName(string tagName);
        IXmlNode ImportNode(IXmlNode node, bool deep);
        XmlDocumentType Doctype { get; }
        XmlElement DocumentElement { get; }
        object DocumentUri { get; }
        XmlDomImplementation Implementation { get; }
    }

    [ComVisibleAttribute(true)]
    public interface IXmlDocumentFragment : IXmlNode
    { }

    [ComVisibleAttribute(true)]
    public interface IXmlDocumentType : IXmlNode
    {
        XmlNamedNodeMap Entities { get; }
        string Name { get; }
        XmlNamedNodeMap Notations { get; }
    }

    [ComVisibleAttribute(true)]
    public interface IXmlLoadSettings
    {
        bool ElementContentWhiteSpace { get; set; }
        uint MaxElementDepth { get; set; }
        bool ProhibitDtd { get; set; }
        bool ResolveExternals { get; set; }
        bool ValidateOnParse { get; set; }
    }

    [ComVisibleAttribute(true)]
    public interface IXmlDocumentIO
    {
        void LoadXml(string xml);
        void LoadXml(string xml, XmlLoadSettings loadSettings);
        IAsyncAction SaveToFileAsync(IStorageFile file);
        /*
        static IAsyncOperation<XmlDocument> LoadFromFileAsync(IStorageFile file);
        static IAsyncOperation<XmlDocument> LoadFromFileAsync(IStorageFile file, XmlLoadSettings loadSettings);
        static IAsyncOperation<XmlDocument> LoadFromUriAsync(Uri uri);
        static IAsyncOperation<XmlDocument> LoadFromUriAsync(Uri uri, XmlLoadSettings loadSettings);
         * */
    }

    [ComVisibleAttribute(true)]
    public interface IXmlDomImplementation
    {
        bool HasFeature(string feature, [VariantAttribute] object version);
    }

    [ComVisibleAttribute(true)]
    public interface IXmlNamedNodeMap : IReadOnlyList<IXmlNode>, IEnumerable<IXmlNode>
    {
        IXmlNode GetNamedItem(string name);
        IXmlNode GetNamedItemNS(object namespaceUri, string name);
        IXmlNode Item(uint index);
        IXmlNode RemoveNamedItem(string name);
        IXmlNode RemoveNamedItemNS(object namespaceUri, string name);
        IXmlNode SetNamedItem(IXmlNode node);
        IXmlNode SetNamedItemNS(IXmlNode node);
        uint Length { get; }
    }

    [ComVisibleAttribute(true)]
    public interface IXmlNodeList : IReadOnlyList<IXmlNode>, IEnumerable<IXmlNode>
    {
        IXmlNode Item(uint index);
        uint Length { get; }
    }
}