//
//  XPathToLinq.cs
//  ReadiumPhoneSupport
//
//  Created by Jim Dovey on 2013-12-05.
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
    public class XPathException : Exception
    {
        public XPathException() { }
        public XPathException(string message) : base(message) { }
        public XPathException(string message, Exception inner) : base(message, inner) { }
    }
    
    internal class XPathProcessor
    {
        public static object ProcessXPathExpression(XNode node, string xpath, IXmlNamespaceResolver nsResolver)
        {
            char[] separator = {'/'};
            List<string> remainingXPathNodes = xpath.Split(separator).ToList<string>();
            return ProcessNextXPathComponent(node, remainingXPathNodes.GetEnumerator(), nsResolver);
        }

        public static IXmlNamespaceResolver MakeNamespaceResolver(object namespaceList)
        {
            XmlNamespaceManager ret = new XmlNamespaceManager(new NameTable());
            if (namespaceList == null)
                return ret;

            if (namespaceList is string)
            {
                char[] separator = {' '};
                namespaceList = (namespaceList as string).Split(separator, StringSplitOptions.RemoveEmptyEntries);
            }
            
            if (namespaceList is IEnumerable<string>)
            {
                foreach (string str in (namespaceList as IEnumerable<string>))
                {
                    var pair = SplitXmlnsDeclaration(str);
                    if (pair.Key != null)
                        ret.AddNamespace(pair.Key, pair.Value);
                }
            }
            else if (namespaceList is IEnumerable<KeyValuePair<string, string>>)
            {
                // assuming prefix->uri
                foreach (KeyValuePair<string, string> pair in (namespaceList as IEnumerable<KeyValuePair<string, string>>))
                {
                    ret.AddNamespace(pair.Key, pair.Value);
                }
            }
            else if (namespaceList is IEnumerable<KeyValuePair<string, Uri>>)
            {
                foreach (KeyValuePair<string, Uri> pair in (namespaceList as IEnumerable<KeyValuePair<string, Uri>>))
                {
                    ret.AddNamespace(pair.Key, pair.Value.ToString());
                }
            }
            else if (namespaceList is IEnumerable<KeyValuePair<Uri, string>>)
            {
                foreach (KeyValuePair<Uri, string> pair in (namespaceList as IEnumerable<KeyValuePair<Uri, string>>))
                {
                    ret.AddNamespace(pair.Value, pair.Key.ToString());
                }
            }

            return ret;
        }

        internal static IXmlNodeList NodeListFromXPathResult(object selected)
        {
            if (selected == null)
                return new XmlNodeList(null);

            var type = selected.GetType();

            List<XObject> linqList = null;
            if (selected is List<XObject>)
            {
                linqList = selected as List<XObject>;
            }
            else if (type.IsArray)
            {
                linqList = new List<XObject>(selected as XObject[]);
            }
            else if (type.IsEnum)
            {
                linqList = new List<XObject>(selected as IEnumerable<XObject>);
            }
            else if (selected is XObject)
            {
                linqList = new List<XObject>(1);
                linqList.Add(selected as XObject);
            }

            return new XmlNodeList(linqList);
        }

        private static object ProcessNextXPathComponent(object current, IEnumerator<string> remainingXPathNodes, IXmlNamespaceResolver nsResolver)
        {
            string thisXPathNode = remainingXPathNodes.Current;
            bool more = remainingXPathNodes.MoveNext();
            bool deep = false;

            if (thisXPathNode.Length == 0 && more)
            {
                deep = true;
                thisXPathNode = remainingXPathNodes.Current;
                more = remainingXPathNodes.MoveNext();
            }

            if (thisXPathNode == ".")
            {
                return ProcessNextXPathComponent(current, remainingXPathNodes, nsResolver);
            }
            else if (thisXPathNode == "..")
            {
                object parents = GetParents(current);
                return ProcessNextXPathComponent(parents, remainingXPathNodes, nsResolver);
            }

            object ret = null;
            if (deep)
            {
                ret = GetMatchingDescendants(current, thisXPathNode, nsResolver);
            }
            else
            {
                ret = GetMatchingChildElements(current, thisXPathNode, nsResolver);
            }

            if (more)
                return ProcessNextXPathComponent(ret, remainingXPathNodes, nsResolver);

            return ret;
        }

        private static XName XNameFromString(string name, IXmlNamespaceResolver nsResolver)
        {
            int sep = name.IndexOf(':');
            if (sep == -1)
                return name;

            string prefix = name.Substring(0, sep), localname = name.Substring(sep + 1);
            return XNamespace.Get(nsResolver.LookupNamespace(prefix)) + localname;
        }

        private static object GetParents(object current)
        {
            IEnumerable<XNode> nodes = current as IEnumerable<XNode>;
            if (nodes == null)
                throw new XPathException("Step-to-parent on a non-node-list type");

            return nodes.Select(n => n.Parent);
        }

        private static object GetMatchingDescendants(object current, string thisXPathNode, IXmlNamespaceResolver nsResolver)
        {
            XContainer container = current as XContainer;
            if (container == null)
                throw new XPathException("Cannot step inside a this type of node.");

            string elementName = thisXPathNode;
            int matchClauseStart = thisXPathNode.IndexOf('[');
            if (matchClauseStart >= 0)
                elementName = thisXPathNode.Substring(0, matchClauseStart);

            object matched = null;
            if (elementName == "*")
                matched = container.Descendants();
            else
                matched = container.Descendants(XNameFromString(elementName, nsResolver));

            if (matchClauseStart == -1)
                return matched;

            char[] clauseSeparators = { '[', ']' };
            string[] clauses = thisXPathNode.Substring(matchClauseStart).Split(clauseSeparators, StringSplitOptions.RemoveEmptyEntries);
            foreach (string clause in clauses)
            {
                matched = FilterCurrentObject(matched, clause, nsResolver);
                if (matched == null)
                    return null;
            }

            return matched;
        }

        private static object GetMatchingChildElements(object current, string thisXPathNode, IXmlNamespaceResolver nsResolver)
        {
            XContainer container = current as XContainer;
            if (container == null)
                throw new XPathException("Cannot step inside this type of node.");

            string elementName = thisXPathNode;
            int matchClauseStart = thisXPathNode.IndexOf('[');
            if (matchClauseStart >= 0)
                elementName = thisXPathNode.Substring(0, matchClauseStart);

            object matched = null;
            if (elementName == "*")
                matched = container.Elements();
            else
                matched = container.Elements(XNameFromString(elementName, nsResolver));

            if (matchClauseStart == -1)
                return matched;

            char[] clauseSeparators = { '[', ']' };
            string[] clauses = thisXPathNode.Substring(matchClauseStart).Split(clauseSeparators, StringSplitOptions.RemoveEmptyEntries);
            foreach (string clause in clauses)
            {
                matched = FilterCurrentObject(matched, clause, nsResolver);
                if (matched == null)
                    return null;
            }

            return matched;
        }

        private static object FilterCurrentObject(object current, string clause, IXmlNamespaceResolver nsResolver)
        {
            IEnumerable<XElement> elements = current as IEnumerable<XElement>;
            if (elements == null)
                throw new XPathException("Filter clause on a non-element-list type");

            // split the clause into left & right
            string[] operators = {"=", "==", "!=", ">", "<", ">=", "<="};
            string[] components = clause.Split(operators, StringSplitOptions.RemoveEmptyEntries);
            if (components.Length != 2)
                throw new XPathException("Could not parse clause: " + clause);

            string name = components[0], value = components[1];
            int operatorStart = name.Length;
            int operatorEnd = clause.Length - value.Length;
            string theOperator = clause.Substring(operatorStart, operatorEnd - operatorStart);

            bool isAttribute = name.StartsWith("@");
            if (isAttribute)
                name = name.Substring(1);

            XName qname = XNameFromString(name, nsResolver);

            return elements.Where(e => e.CompareChildValue(qname, isAttribute, theOperator, value));
        }

        private static KeyValuePair<string, string> SplitXmlnsDeclaration(string decl)
        {
            // split on the '='
            char[] eq = { '=' };
            string[] components = decl.Split(eq, 2);
            if (components.Length < 2)
                return new KeyValuePair<string, string>();

            string prefix = components[0], uri = components[1];

            if (uri.StartsWith("\"") && uri.EndsWith("\""))
                uri = uri.Substring(1, uri.Length - 2);

            if (prefix == "xmlns")
                prefix = "";
            else if (prefix.StartsWith("xmlns:"))
                prefix = prefix.Substring(6);

            return new KeyValuePair<string, string>(prefix, uri);
        }
    }
}