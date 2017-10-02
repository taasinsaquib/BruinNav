//
//  MyMap.h
//  Project4
//
//  Created by Taasin Saquib on 3/8/17.
//  Copyright © 2017 Taasin Saquib. All rights reserved.
//
// Skeleton for the MyMap class template.  You must implement the first six
// member functions.

#ifndef MyMap_h
#define MyMap_h

#include <queue>
#include "support.h"
template<typename KeyType, typename ValueType>

class MyMap
{
public:
    MyMap();
    ~MyMap();
    void clear();
    int size() const;
    
    void associate(const KeyType& key, const ValueType& value);
    
    // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;
    
    // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
    }
    
    // C++11 syntax for preventing copying and assignment
    MyMap(const MyMap&) = delete;
    MyMap& operator=(const MyMap&) = delete;
    
private:
    
    //Node to hold the values mapped
    struct Node{
        KeyType key;
        ValueType value;
        Node* left;
        Node* right;
    };
    
    Node* root;
    int m_size;
    
};

template<typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::MyMap(){
    
    root = nullptr;
    m_size = 0;
}

template<typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::~MyMap(){
    clear();
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::clear(){
    
    if(root == nullptr)
        return;
    
    // Create an empty queue for level order traversal
    std::queue<Node *> q;
    
    // Do level order traversal starting from the root
    q.push(root);
    
    while (!q.empty()){
        Node *node = q.front();
        q.pop();
        if (node->left != nullptr)
            q.push(node->left);
        if (node->right != nullptr)
            q.push(node->right);
        
        delete node;
    }
}

template<typename KeyType, typename ValueType>
int MyMap<KeyType, ValueType>::size() const{
    
    return m_size;
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value){
    
    //if tree has no nodes, create a new one
    if(root == nullptr){
        
        Node* n = new Node;
        root  = n;
        
        n -> key = key;
        n -> value = value;
        n -> left = nullptr;
        n -> right = nullptr;
        
        m_size++;
        return;
    }
    
    Node* curr = root;
    
    //loop through the tree
    while(curr != nullptr){
        
        //update the old node
        if(curr -> key == key){
            curr -> value = value;
            return;
        }
        
        if(curr -> key > key){
            
            //if there isn't a left branch, add one
            if(curr -> left == nullptr){
                Node* n = new Node;
                
                n -> key = key;
                n -> value = value;
                n -> left = nullptr;
                n -> right = nullptr;
                
                curr -> left = n;
                m_size++;
                return;
            }
            
            //else, continue to next node
            curr = curr -> left;
            
        }
        
        if(curr -> key < key){
            
            //if there isn't a right branch, add one
            if(curr -> right == nullptr){
                Node* n = new Node;
                
                n -> key = key;
                n -> value = value;
                n -> left = nullptr;
                n -> right = nullptr;
                
                curr -> right = n;
                m_size++;
                return;
            }
            
            //else, continue to next node
            curr = curr -> right;
        }
    }
    m_size++;
}

template<typename KeyType, typename ValueType>
const ValueType* MyMap<KeyType, ValueType>::find(const KeyType& key) const{
    
    if(root == nullptr) //no values in tree
        return nullptr;
    
    Node* curr = root;
    
    //loop through tree
    while(curr != nullptr){
        
        //found the key
        if(curr -> key == key){
            return &(curr -> value);
        }
        
        //go right
        if(curr -> key > key)
            curr = curr -> left;
        
        //go left
        else if(curr -> key < key)
            curr = curr -> right;
    }
    return nullptr;
}

#endif /* MyMap_h */
