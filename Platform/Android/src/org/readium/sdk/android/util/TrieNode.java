package org.readium.sdk.android.util;

import java.util.ArrayList;
import java.util.List;

//TODO: Do we need this?

/**
 * Copyright 2013 Mantano SAS.
 * User: jmgeffroy
 * Date: 12/06/13
 * Time: 17:00
 */
public class TrieNode
{
    private TrieNode parent;
    private TrieNode[] children;
    private boolean isLeaf;     //Quick way to check if any children exist
    private boolean isWord;     //Does this node represent the last character of a word
    private char character;     //The character this node represents

    /**
     * Constructor for top level root node.
     */
    public TrieNode()
    {
        children = new TrieNode[26];
        isLeaf = true;
        isWord = false;
    }

    /**
     * Constructor for child node.
     */
    public TrieNode(char character)
    {
        this();
        this.character = character;
    }

    /**
     * Adds a word to this node. This method is called recursively and
     * adds child nodes for each successive letter in the word, therefore
     * recursive calls will be made with partial words.
     * @param word the word to add
     */
    protected void addWord(String word)
    {
        if (word.isEmpty())
            return;

        isLeaf = false;
        int charPos = word.charAt(0) - 'a';

        if (children[charPos] == null)
        {
            children[charPos] = new TrieNode(word.charAt(0));
            children[charPos].parent = this;
        }

        if (word.length() > 1)
        {
            children[charPos].addWord(word.substring(1));
        }
        else
        {
            children[charPos].isWord = true;
        }
    }

    /**
     * Returns the child TrieNode representing the given char,
     * or null if no node exists.
     * @param c
     * @return
     */
    protected TrieNode getNode(char c)
    {
        return children[c - 'a'];
    }

    /**
     * Returns a List of String objects which are lower in the
     * hierarchy that this node.
     * @return
     */
    protected List getWords()
    {
        //Create a list to return
        List list = new ArrayList();

        //If this node represents a word, add it
        if (isWord)
        {
            list.add(toString());
        }

        //If any children
        if (!isLeaf)
        {
            //Add any words belonging to any children
            for (int i = 0; i < children.length; i++) {
                TrieNode child = children[i];
                if (child != null)
                {
                    list.addAll(child.getWords());
                }
            }

        }

        return list;

    }



    /**

     * Gets the String that this node represents.
     * For example, if this node represents the character t, whose parent
     * represents the charater a, whose parent represents the character
     * c, then the String would be "cat".
     * @return
     */

    public String toString()

    {
        if (parent == null)
        {
            return "";
        }
        else
        {
            return parent.toString() + new String(new char[] {character});
        }

    }

}