#pragma once

#include "pch.h"
#include <atomic>
#include <mutex>

using ::Platform::Object;
using ::Platform::String;
using ::Windows::Foundation::IAsyncAction;
using ::Windows::Storage::IStorageFile;
using ::Windows::Storage::Streams::IBuffer;
using ::Windows::Foundation::Collections::IVectorView;
using ::Windows::Foundation::Collections::IIterable;

namespace PhoneSupportInterfaces
{
	public interface class ITransform
	{
		property bool CanReuseTransform { bool get(); }
		property bool CanTransformMultipleBlocks { bool get(); }
		property int InputBlockSize { int get(); }
		property int OutputBlockSize { int get(); }

		int TransformBlock(IBuffer^ inputBuffer, int inputOffset, int inputCount,
			IBuffer^ outputBuffer, int outputCount);
		IBuffer^ TransformFinalBlock(IBuffer^ inputBuffer, int inputOffset, int inputCount);
	};

	public interface class ICryptoEngine : public ITransform
    {
		bool Initialize(bool encrypt, IBuffer^ iv, IBuffer^ key);
    };

	public interface class IHasher : public ITransform
	{
		property IBuffer^ Hash { IBuffer^ get(); }
		property int HashSize { int get(); }

		void Initialize();
		void Clear();
		
		IBuffer^ ComputeHash(IBuffer^ buffer);
		IBuffer^ ComputeHash(IBuffer^ buffer, int offset, int count);
	};

	/// <summary>
	/// The type of an IXmlNode, as returned by the NodeType property.
	/// </summary>
	public enum class NodeType
	{
		/// <summary>Default value. Never used by the API.</summary>
		Invalid,
		/// <summary>The node implements IXmlElement.</summary>
		ElementNode,
		/// <summary>The node implements IXmlAttribute.</summary>
		AttributeNode,
		/// <summary>The node implements IXmlText.</summary>
		TextNode,
		/// <summary>The node implements IXmlCDataSection.</summary>
		CDATANode,
		/// <summary>The node implements IXmlEntityReference.</summary>
		EntityReferenceNode,
		/// <summary>The node implements IDtdEntity.</summary>
		EntityNode,
		/// <summary>The node implements IXmlProcessingInstruction.</summary>
		ProcessingInstructionNode,
		/// <summary>The node implements IXmlComment.</summary>
		CommentNode,
		/// <summary>The node implements IXmlDocument.</summary>
		DocumentNode,
		/// <summary>The node implements IXmlDocumentType.</summary>
		DocumentTypeNode,
		/// <summary>The node implements IXmlDocumentFragment.</summary>
		DocumentFragmentNode,
		/// <summary>The node implements IDtdNotation.</summary>
		NotationNode,
		/// <summary>Significant whitespace -- no special node type, may appear as IXmlText</summary>
		SignificantWhitespaceNode,
		/// <summary>Non-significant whitespace -- no special node type, may appear as IXmlText</summary>
		WhitespaceNode
	};

	interface class IXmlNodeList;
	interface class IXmlNamedNodeMap;
	interface class IXmlDocument;
	interface class IXmlNode;
	interface class IXmlDocumentFragment;
	interface class IXmlDomImplementation;
	interface class IXmlDocumentType;
	interface class IXmlElement;
	interface class IXmlAttribute;

	/// <summary>
	/// Encapsulates the methods needed to execute XPath queries on an XML DOM tree or subtree.
	/// </summary>
	public interface class IXmlNodeSelector
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
		IXmlNodeList^ SelectNodes(String^ xpath);

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
		IXmlNodeList^ SelectNodesNS(String^ xpath, [Windows::Foundation::Metadata::VariantAttribute] Object^ namespaces);

		/// <summary>
		/// Applies the specified pattern-matching operation to this node's context and 
		/// returns the first matching node.
		/// </summary>
		/// <param name="xpath">Specifies an XPath expression.</param>
		/// <returns>The first node that matches the given pattern-matching operation.
		/// If no nodes match the expression, the method returns a null value.</returns>
		IXmlNode^ SelectSingleNode(String^ xpath);

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
		IXmlNode^ SelectSingleNodeNS(String^ xpath, [Windows::Foundation::Metadata::VariantAttribute] Object^ namespaces);
	};

	/// <summary>
	/// Encapsulates the methods needed to serialize a DOM tree or subtree to a string representation.
	/// </summary>
	public interface class IXmlNodeSerializer
	{
		/// <summary>
		/// Returns the XML representation of the node and all its descendants.
		/// </summary>
		/// <returns>The XML representation of the node and all its descendants.</returns>
		String^ GetXml();

		/// <summary>
		/// Gets and sets the text from inside the XML.
		/// </summary>
		/// <result>The text from inside the XML. Returns an empty string if there is no text.</result>
		property String^ InnerText { String^ get(); void set(String^ str); }
	};

	/// <summary>
	/// Contains information for the entire Document Object Model. This interface represents a single
	/// node in the document tree. While all objects that implement this interface expose methods for
	/// dealing with children, not all objects that implement this interface may have children.
	/// </summary>
	public interface class IXmlNode : public IXmlNodeSelector, IXmlNodeSerializer
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
		IXmlNode^ AppendChild(IXmlNode^ newNode);

		/// <summary>
		/// Clones a new node.
		/// </summary>
		/// <param name="deep">
		/// A flag that indicates whether to recursively clone all nodes that are descendants of
		/// this node. If true, this method creates a clone of the complete tree below this node.
		/// If false, this method clones this node and its attributes only.
		/// </param>
		/// <returns>The newly created clone node.</returns>
		IXmlNode^ CloneNode(bool deep);

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
		IXmlNode^ InsertBefore(IXmlNode^ newChild, IXmlNode^ referenceNode);

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
		IXmlNode^ RemoveChild(IXmlNode^ childNode);

		/// <summary>
		/// Replaces the specified old child node with the supplied new child node.
		/// </summary>
		/// <param name="newChild">
		/// The new child that is to replace the old child. If null, the referenceChild parameter
		/// is removed without a replacement.
		/// </param>
		/// <param name="referenceChild">The old child that is to be replaced by the new child.</param>
		/// <returns>The old child that is replaced. If null, no object is created.</returns>
		IXmlNode^ ReplaceChild(IXmlNode^ newChild, IXmlNode^ referenceChild);

		/// <summary>
		/// Returns the root of the document that contains the node.
		/// </summary>
		property IXmlDocument^ OwnerDocument { IXmlDocument^ get(); }

		/// <summary>
		/// Gets the parent node of the node instance.
		/// </summary>
		property IXmlNode^ ParentNode { IXmlNode^ get(); }

		/// <summary>
		/// Gets the list of attributes of this node.
		/// </summary>
		property IXmlNamedNodeMap^ Attributes { IXmlNamedNodeMap^ get(); }

		/// <summary>
		/// Gets a list of children in the current node.
		/// </summary>
		property IXmlNodeList^ ChildNodes { IXmlNodeList^ get(); }

		/// <summary>
		/// Gets the first child node.
		/// </summary>
		property IXmlNode^ FirstChild { IXmlNode^ get(); }

		/// <summary>
		/// Gets the last child node.
		/// </summary>
		property IXmlNode^ LastChild { IXmlNode^ get(); }

		/// <summary>
		/// Gets the local name, which is the local part of a qualified name.
		/// This is called the local part in Namespaces in XML.
		/// </summary>
		property Object^ LocalName { Object^ get(); }

		/// <summary>
		/// Gets or sets the namespace prefix.
		/// </summary>
		/// <result>
		/// The namespace prefix specified on the element, attribute, or entity reference.
		/// For example, for the element <xxx:yyy>, this property returns xxx. It returns
		/// an empty string, "", if no prefix is specified.
		/// </result>
		property String^ Prefix { String^ get(); void set(String^ str); }

		/// <summary>
		/// Returns the Uniform Resource Identifier (URI) for the namespace.
		/// </summary>
		/// <result>The Uri for the namespace. This refers to the uuu portion of the namespace declaration xmlns:nnn=uuu.</result>
		property Object^ NamespaceUri { Object^ get(); }

		/// <summary>
		/// Returns the qualified name for attribute, document type, element, entity, or notation nodes.
		/// Returns a fixed string for all other node types.
		/// </summary>
		property String^ NodeName { String^ get(); }

		/// <summary>
		/// Gets the XML Document Object Model (DOM) node type, which determines valid values and
		/// whether the node can have child nodes.
		/// </summary>
		property PhoneSupportInterfaces::NodeType NodeType { PhoneSupportInterfaces::NodeType get(); }

		/// <summary>
		/// Gets or sets the text associated with the node.
		/// </summary>
		property Object^ NodeValue { Object^ get(); void set(Object^ value); }

		/// <summary>
		/// Gets the next sibling of the node in the parent's child list.
		/// </summary>
		/// <result>The right sibling of this node.</result>
		property IXmlNode^ NextSibling { IXmlNode^ get(); }

		/// <summary>
		/// Gets the previous sibling of the node in the parent's child list.
		/// </summary>
		/// <result>The left sibling of this node.</result>
		property IXmlNode^ PreviousSibling { IXmlNode^ get(); }
	};

	public interface class IDtdEntity : public IXmlNode
	{
		property Object^ NotationName { Object^ get(); }
		property Object^ PublicId{ Object^ get(); }
		property Object^ SystemId{ Object^ get(); }
	};

	public interface class IDtdNotation : public IXmlNode
	{
		property Object^ PublicId { Object^ get(); }
		property Object^ SystemId { Object^ get(); }
	};

	public interface class IXmlEntityReference : public IXmlNode
	{};

	public interface class IXmlProcessingInstruction : public IXmlNode
	{
		property String^ Data { String^ get(); void set(String^ str); }
		property String^ Target { String^ get(); }
	};

	public interface class IXmlAttribute : public IXmlNode
	{
		property String^ Name { String^ get(); }
		property bool Specified { bool get(); }
		property String^ Value{ String^ get(); void set(String^ str); }
	};

	/// <summary>
	/// Provides text manipulation methods that are used by several objects.
	/// </summary>
	public interface class IXmlCharacterData : public IXmlNode
	{
		/// <summary>
		/// Appends the supplied string to the existing string data.
		/// </summary>
		/// <param name="data">The data to be appended to the existing string.</param>
		void AppendData(String^ data);

		/// <summary>
		/// Deletes specified data.
		/// </summary>
		/// <param name="offset">The offset, in characters, at which to start deleting the string data.</param>
		/// <param name="count">The number of characters to delete.</param>
		void DeleteData(UINT offset, UINT count);

		/// <summary>
		/// Inserts a string at the specified offset.
		/// </summary>
		/// <param name="offset">The offset, in characters, at which to insert the supplied string data.</param>
		/// <param name="data">The data to be inserted into the existing string.</param>
		void InsertData(UINT offset, String^ data);

		/// <summary>
		/// Replaces the specified number of characters with the supplied string.
		/// </summary>
		/// <param name="offset">The offset, in characters, at which to start replacing string data.</param>
		/// <param name="count">The number of characters to replace.</param>
		/// <param name="data">The new data that replaces the old string data.</param>
		void ReplaceData(UINT offset, UINT count, String^ data);

		/// <summary>
		/// Retrieves a substring of the full string from the specified range.
		/// </summary>
		/// <param name="offset">Specifies the offset, in characters, from the beginning of the string.
		/// An offset of zero indicates copying from the start of the data.</param>
		/// <param name="count">Specifies the number of characters to retrieve from the specified offset.</param>
		/// <returns>The returned substring.</returns>
		String^ SubstringData(UINT offset, UINT count);

		/// <summary>
		/// Gets or sets the node data depending on the node type.
		/// </summary>
		/// <result>The node data.</result>
		property String^ Data { String^ get(); void set(String^ str); }

		/// <summary>
		/// Gets the length of the data, in Unicode characters.
		/// </summary>
		property UINT Length { UINT get(); }
	};

	/// <summary>
	/// Represents the text content of an element or attribute.
	/// </summary>
	public interface class IXmlText : public IXmlCharacterData
	{
		/// <summary>
		/// Splits this text node into two text nodes at the specified offset and inserts the new
		/// text node into the tree as a sibling that immediately follows this node.
		/// </summary>
		/// <param name="offset">The number of characters at which to split this text node into
		/// two nodes, starting from zero.</param>
		/// <returns>The new text node.</returns>
		IXmlText^ SplitText(UINT offset);
	};

	public interface class IXmlCDataSection : public IXmlCharacterData {};

	public interface class IXmlElement : public IXmlNode
	{
		String^ GetAttribute(String^ attributeName);
		String^ GetAttributeNS([Windows::Foundation::Metadata::VariantAttribute] Object^ namespaceUri, String^ attributeName);
		IXmlAttribute^ GetAttributeNode(String^ attributeName);
		IXmlAttribute^ GetAttributeNodeNS([Windows::Foundation::Metadata::VariantAttribute] Object^ namespaceUri, String^ attributeName);
		IXmlNodeList^ GetElementsByTagName(String^ tagName);
		void RemoveAttribute(String^ attributeName);
		void RemoveAttributeNS([Windows::Foundation::Metadata::VariantAttribute] Object^ namespaceUri, String^ attributeName);
		IXmlAttribute^ RemoveAttributeNode(IXmlAttribute^ attributeNode);
		void SetAttribute(String^ attributeName, String^ attributeValue);
		void SetAttributeNS([Windows::Foundation::Metadata::VariantAttribute] Object^ namespaceUri, String^ attributeName, String^ attributeValue);
		IXmlAttribute^ SetAttributeNode(IXmlAttribute^ attributeNode);
		IXmlAttribute^ SetAttributeNodeNS(IXmlAttribute^ attributeNode);
		property String^ TagName { String^ get(); }
	};

	public interface class IXmlComment : public IXmlCharacterData
	{};

	public interface class IXmlDocument : public IXmlNode
	{
		IXmlAttribute^ CreateAttribute(String^ name);
		IXmlAttribute^ CreateAttributeNS([Windows::Foundation::Metadata::VariantAttribute] Object^ uri, String^ name);
		IXmlCDataSection^ CreateCDataSection(String^ data);
		IXmlComment^ CreateComment(String^ comment);
		IXmlDocumentFragment^ CreateDocumentFragment();
		IXmlElement^ CreateElement(String^ tagName);
		IXmlElement^ CreateElementNS([Windows::Foundation::Metadata::VariantAttribute] Object^ uri, String^ tagName);
		IXmlEntityReference^ CreateEntityReference(String^ name);
		IXmlProcessingInstruction^ CreateProcessingInstruction(String^ target, String^ data);
		IXmlText^ CreateTextNode(String^ data);
		IXmlElement^ GetElementById(String^ elementId);
		IXmlNodeList^ GetElementsByTagName(String^ tagName);
		IXmlNode^ ImportNode(IXmlNode^ node, bool deep);
		property IXmlDocumentType^ Doctype { IXmlDocumentType^ get(); }
		property IXmlElement^ DocumentElement { IXmlElement^ get(); }
		property Object^ DocumentUri { Object^ get(); }
		property IXmlDomImplementation^ Implementation { IXmlDomImplementation^ get(); }
	};

	public interface class IXmlDocumentFragment : public IXmlNode
	{};

	public interface class IXmlDocumentType : public IXmlNode
	{
		property IXmlNamedNodeMap^ Entities { IXmlNamedNodeMap^ get(); }
		property String^ Name { String^ get(); }
		property IXmlNamedNodeMap^ Notations { IXmlNamedNodeMap^ get(); }
	};

	public interface class IXmlLoadSettings
	{
		property bool ElementContentWhiteSpace { bool get(); void set(bool val); }
		property UINT MaxElementDepth { UINT get(); void set(UINT val); }
		property bool ProhibitDtd { bool get(); void set(bool val); }
		property bool ResolveExternals { bool get(); void set(bool val); }
		property bool ValidateOnParse { bool get(); void set(bool val); }
	};

	public interface class IXmlDocumentIO
	{
		void LoadXml(String^ xml);
		void LoadXml(String^ xml, IXmlLoadSettings^ loadSettings);
		IAsyncAction^ SaveToFileAsync(IStorageFile^ file);
		/*
		static IAsyncOperation<XmlDocument> LoadFromFileAsync(IStorageFile file);
		static IAsyncOperation<XmlDocument> LoadFromFileAsync(IStorageFile file, XmlLoadSettings loadSettings);
		static IAsyncOperation<XmlDocument> LoadFromUriAsync(Uri uri);
		static IAsyncOperation<XmlDocument> LoadFromUriAsync(Uri uri, XmlLoadSettings loadSettings);
		* */
	};

	public interface class IXmlDomImplementation
	{
		bool HasFeature(String^ feature, [Windows::Foundation::Metadata::VariantAttribute] Object^ version);
	};

	public interface class IXmlNamedNodeMap : public IVectorView<IXmlNode^>, public IIterable<IXmlNode^>
	{
		IXmlNode^ GetNamedItem(String^ name);
		IXmlNode^ GetNamedItemNS([Windows::Foundation::Metadata::VariantAttribute] Object^ namespaceUri, String^ name);
		IXmlNode^ Item(UINT index);
		IXmlNode^ RemoveNamedItem(String^ name);
		IXmlNode^ RemoveNamedItemNS([Windows::Foundation::Metadata::VariantAttribute] Object^ namespaceUri, String^ name);
		IXmlNode^ SetNamedItem(IXmlNode^ node);
		IXmlNode^ SetNamedItemNS(IXmlNode^ node);
		property UINT Length { UINT get(); }
	};

	public interface class IXmlNodeList : public IVectorView<IXmlNode^>, public IIterable<IXmlNode^>
	{
		IXmlNode^ Item(UINT index);
		property UINT Length { UINT get(); }
	};

	public interface class IXmlDocumentFactory
	{
		IXmlLoadSettings^ CreateLoadSettings();
		IXmlDocument^ CreateXmlDocument();
	};

	public interface class ICryptoFactory
	{
		ICryptoEngine^ CreateAES128();
		IHasher^ CreateSHA1();
		IHasher^ CreateSHA256();
	};

	public ref class FactoryGlue sealed
	{
	private:
		IXmlDocumentFactory^	_xmlFactory;
		ICryptoFactory^			_cryptoFactory;

	public:
		static FactoryGlue^ Singleton();

		FactoryGlue();
		virtual ~FactoryGlue();

		property IXmlDocumentFactory^ XmlFactory
		{
			IXmlDocumentFactory^ get();
			void set(IXmlDocumentFactory^ obj);
		}

		property ICryptoFactory^ CryptoFactory
		{
			ICryptoFactory^ get();
			void set(ICryptoFactory^ obj);
		}
	};
}