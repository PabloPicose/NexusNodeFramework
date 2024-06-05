//
// Created by pablo on 5/28/24.
//

#include "Node.h"
#include "Core/Application.h"
#include "Core/NodePtr.h"

namespace IRC {
    Node::~Node() {
        const auto app = Application::instance();
        if (m_isRoot) {
            // print the current thread id
            if (Application::instance()->m_threadId == Application::threadId()) {
                Application::instance()->removeRootNode(this);
            }
        } else if (m_parent) {
            for (auto it = m_parent->m_children.begin(); it != m_parent->m_children.end(); ++it) {
                if (*it == this) {
                    m_parent->m_children.erase(it);
                    break;
                }
            }
        }
        for (auto child: m_children) {
            delete child;
        }
        m_children.clear();
        app->unregisterAliveNode(this);
    }

    void Node::setName(const std::string& name) {
        m_name = name;
    }

    std::string Node::getName() const {
        return m_name;
    }

    void Node::addChild(Node* child) {
        if (!child) {
            std::cerr << "Node::addChild(): Child is nullptr" << std::endl;
            return;
        }
        // if the child is root node we have to remove it from the root nodes
        if (child->isRoot()) {
            child->m_isRoot = false;
            Application::instance()->removeRootNode(child);
        }
        child->m_parent = this;
        m_children.push_back(child);
    }

    Node* Node::getChild(const std::size_t index) const {
        if (index >= m_children.size()) {
            return nullptr;
        }
        return m_children[index];
    }

    bool Node::isChild(Node* child) const {
        if (std::ranges::find(m_children, child) != m_children.end()) {
            return true;
        }
        return false;
    }

    void Node::deleteLater() {
        auto app = Application::instance();
        if (!app) {
            throw std::runtime_error("Application instance not exists");
        }
        app->markToDelete(this);
        if (m_parent) {
            m_parent->pushDeleteChild(this);
        } else {
            app->pushRootNodeDeleteLater(this);
        }
    }

    void Node::update() {
        std::cerr << "Pure virtual function called from Node::update()" << std::endl;
    }

    void Node::run() {

        // delete childs to delete using iterator
        for (auto child: m_childrenToDelete) {
            NodePtr nodePtr(child);
            if (nodePtr) {
                delete child;
            }
        }
        m_childrenToDelete.clear();

        for (auto child: m_children) {
            child->run();
        }
        update();

        m_childrenToDelete.clear();
    }

    void Node::setParent(Node* parent) {
        if (m_parent) {
            for (auto it = m_parent->m_children.begin(); it != m_parent->m_children.end(); ++it) {
                if (*it == this) {
                    m_parent->m_children.erase(it);
                    break;
                }
            }
        } else {
            Application::instance()->removeRootNode(this);
        }
        if (!parent) {
            m_isRoot = true;
            Application::instance()->pushRootNode(this);
        } else {
            m_isRoot = false;
            parent->addChild(this);
        }
    }

    Node* Node::parent() const {
        return m_parent;
    }

    std::size_t Node::childrenCount() const {
        return m_children.size();
    }

    bool Node::isRoot() const {
        return m_isRoot;
    }

    size_t Node::childrenToDeleteCount() const {
        return m_childrenToDelete.size();
    }

    Node::Node(Node* parent) :
        m_parent(parent) {
        const auto app = Application::instance();
        if (!app) {
            throw std::runtime_error("Application instance not created");
        }
        if (m_parent) {
            m_parent->addChild(this);
        } else {
            m_isRoot = true;
            if (app->m_threadId != Application::threadId()) {
                throw std::runtime_error("Node can only be added from the main thread");
            }
            app->pushRootNode(this);
        }
        app->registerAliveNode(this);
    }

    void Node::pushDeleteChild(Node* child) {
        for (auto it = m_children.begin(); it != m_children.end(); ++it) {
            if (*it == child) {
                m_childrenToDelete.push_back(child);
                break;
            }
        }
    }
} // IRC
